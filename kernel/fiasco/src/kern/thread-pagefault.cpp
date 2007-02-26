IMPLEMENTATION[pagefault]:

#include <cstdio>

#include "cpu.h"
#include "kdb_ke.h"
#include "kmem.h"
#include "processor.h"
#include "config.h"
#include "kdb_ke.h"
#include "regdefs.h"
#include "std_macros.h"
#include "thread.h"
#include "vmem_alloc.h"


/** 
 * The global page fault handler switch.
 * Handles page-fault monitoring, classification of page faults based on
 * virtual-memory area they occured in, page-directory updates for kernel
 * faults, IPC-window updates, and invocation of paging function for
 * user-space page faults (handle_page_fault_pager).
 * @param pfa page-fault virtual address
 * @param error_code CPU error code
 * @return true if page fault could be resolved, false otherwise
 * @exception longjmp longjumps to recovery location if page-fault
 *                    handling fails (i.e., return value would be false),
 *                    but recovery location has been installed           
 */
IMPLEMENT inline NEEDS[<cstdio>, "regdefs.h", "kdb_ke.h","processor.h","config.h",
		       "std_macros.h","vmem_alloc.h",Thread::page_fault_log]
bool Thread::handle_page_fault (Address pfa,
				Mword error_code,
				Mword pc)
{

  //  printf("*P[%x,%x,%x]\n", pfa, error_code & 0xffff, pc);
#if 0
  printf("Translation error ? %x\n"
	 "  current space has mapping : %08x\n"
	 "  Kernel space has mapping  : %08x\n",
	 PF::is_translation_error(error_code),
	 current_space()->lookup((void*)pfa),
	 Space::kernel_space()->lookup((void*)pfa));
#endif

  if (Config::monitor_page_faults)
    {
      if (_last_pf_address == pfa && _last_pf_error_code == error_code)
        {
          if (!log_page_fault())
            printf("*P[%x,%x,%x]\n", pfa, error_code & 0xffff, pc);
          else
            putchar('\n');

          kdb_ke("PF happened twice");
        }

      _last_pf_address = pfa;
      _last_pf_error_code = error_code;

      // (See also corresponding code in Thread::handle_ipc_page_fault()
      //                          and in Thread::handle_slow_trap.)
    }

  // TODO: put this into a debug_page_fault_handler
  if (EXPECT_FALSE (log_page_fault()))
    page_fault_log (pfa, error_code, pc);

  L4_msgdope ipc_code(0);

  // Check for page fault in user memory area
  if (EXPECT_TRUE (!Kmem::is_kmem_page_fault(pfa, error_code)))
    {
      // Make sure that we do not handle page faults that do not
      // belong to this thread.
      assert (space() == current_space());

      if (EXPECT_FALSE (space()->is_sigma0()))
        {
          // special case: sigma0 can map in anything from the kernel
	  if(handle_sigma0_page_fault(pfa))
            return true;

          ipc_code.error (L4_msgdope::REMAPFAILED);
          goto error;
        }

      // user mode page fault -- send pager request
      if (!(ipc_code = handle_page_fault_pager(pfa, error_code)).has_error())
        return true;

      goto error;
    }

  // Check for page fault in small address space
  else if (EXPECT_FALSE (Kmem::is_smas_page_fault(pfa, error_code)))
    {
      if (handle_smas_page_fault (pfa, error_code, ipc_code))
        return true;

      goto error;
    }
  
  // Check for page fault in kernel memory region caused by user mode
  else if (EXPECT_FALSE(PF::is_usermode_error(error_code)
			/*(error_code & PF_ERR_USERMODE)*/))
    return false;             // disallow access after mem_user_max

  // Check for page fault in IO bit map or in delimiter byte behind IO bitmap
  // assume it is caused by an input/output instruction and fall through to
  // handle_slow_trap
  else if (EXPECT_FALSE(Kmem::is_io_bitmap_page_fault(pfa, error_code)))
    return false;

  // We're in kernel code faulting on a kernel memory region

  // Check for page fault in IPC window. Mappings for this should never
  // be present in the global master page dir (case below), because the
  // IPC window mappings are always flushed on context switch.
  else if (EXPECT_TRUE (Kmem::is_ipc_page_fault(pfa, error_code)))
    {
      if (!(ipc_code = handle_ipc_page_fault(pfa, error_code)).has_error())
        return true;

      goto error;
    }

  // A page is not present but a mapping exists in the global page dir.
  // Update our page directory by copying from the master pdir
  // This is the only path that should be executed with interrupts
  // disabled if the page faulter also had interrupts disabled.   
  // thread_page_fault() takes care of that.
  else 
#ifdef CONFIG_ARM
  if (PF::is_translation_error(error_code) &&
      Space::kernel_space()->lookup((void*)pfa) != (Mword)-1)
#else
  if((!(error_code & PF_ERR_PRESENT)) &&
      Kmem::virt_to_phys (reinterpret_cast<void*>(pfa)) != 0xffffffff)
#endif
    {
      //puts("Update kernel mappings!!");
      current_space()->kmem_update((void*)pfa);
      return true;
    }

  // Check for page fault in kernel's TCB area
  else if (Kmem::is_tcb_page_fault(pfa, error_code))
    {
      if (PF::is_translation_error(error_code))   // page not present
        {
          // in case of read fault, just map in the shared zero page
          // otherwise -> allocate
          if (!Vmem_alloc::page_alloc((void*)(pfa & Config::PAGE_MASK), 0,
                                      PF::is_read_error(error_code) ?
				      Vmem_alloc::ZERO_MAP:
				      Vmem_alloc::ZERO_FILL)) 
            panic("can't alloc kernel page");
        }
      else
        { 
          // protection fault
          // this can only be because we have the zero page mapped
          Vmem_alloc::page_free(reinterpret_cast<void*>(pfa & Config::PAGE_MASK), 0);
          if (! Vmem_alloc::page_alloc((void*)(pfa & Config::PAGE_MASK), 0,  
                                       Vmem_alloc::ZERO_FILL))
            {
              // error could mean: someone else was faster allocating
              // a page there, or we just don't have any pages left; verify
#ifdef CONFIG_ARM
	      if (Space::kernel_space()->lookup((void*)pfa) != (Mword)-1) 
		panic("can't alloc kernel page");
#else
              if (Kmem::virt_to_phys(reinterpret_cast<void*>(pfa)) 
                  == 0xffffffff)
                panic("can't alloc kernel page");
#endif

              // otherwise, there's a page mapped.  continue
            }
        }

      current_space()->kmem_update((void*)pfa);
      return true;
    }

     
  printf ("KERNEL: no page fault handler for 0x%x, error 0x%x, pc %08x\n",
          pfa, error_code, pc);

  // An error occurred.  Our last chance to recover is an exception
  // handler a kernel function may have set.
 error:

  if (_recover_jmpbuf)
    longjmp (*_recover_jmpbuf, ipc_code.raw());

  return false;
}