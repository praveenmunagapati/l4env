/**
 * \file	roottask/server/src/pager.c
 * \brief	page fault handling
 *
 * \date	05/10/2004
 * \author	Frank Mehnert <fm3@os.inf.tu-dresden.de>
 * \author      Torsten Frenzel <frenzel@os.inf.tu-dresden.de>
 *
 * Page fault handling is based on memmory resource handling.
 * The pager also handles I/O resources.
 **/
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include <l4/sys/types.h>
#include <l4/sys/syscalls.h>
#include <l4/sys/ipc.h>
#include <l4/sys/kdebug.h>
#include <l4/sys/kernel.h>
#include <l4/util/mb_info.h>
#include <l4/util/l4_macros.h>
#include <l4/rmgr/proto.h>

#include "iomap.h"
#include "memmap.h"
#include "memmap_lock.h"
#include "pager.h"
#include "quota.h"
#include "version.h"
#include "rmgr.h"
#include "vm.h"

l4_kernel_info_t *kip;
last_pf_t        last_pf[RMGR_TASK_MAX];
l4_uint32_t      memmap_lock = -1;
int              have_io;
int              no_pentium;

/**
 * Displays the first 100 characters of an error message.
 */
static void
print_err(char *fmt,...)
{
  va_list ap;
  static char errmsg[100];
  va_start(ap,fmt);
  vsnprintf(errmsg,sizeof(errmsg),fmt,ap);
  outstring(errmsg);
  va_end(ap);
}


/**
 * invalidate the ``last page fault'' array.
 **/
void
pager_init(void)
{
  unsigned a;

  for (a = 0; a < RMGR_TASK_MAX; a++)
    {
      last_pf[a].pfa = -1;
      last_pf[a].eip = -1;
    }
}

/**
 * invalidate the ''last page fault`` for a task.
 */
void
reset_pagefault(int task)
{
  last_pf[task].pfa = -1;
  last_pf[task].eip = -1;
}

/**
 * Checks whether a double pagefault occured or not,
 * but only if no_checkdpf is not set. XXX
 * Returns 0 because the pagefault is not handled.
 */
static int
check_double_pagefault(l4_threadid_t t,l4_umword_t pfa,
		       l4_umword_t *d1,l4_umword_t *d2)
{
  /* check for double page fault */
  last_pf_t *last_pfp = last_pf + t.id.task;

  /* note that requests for a specific superpage have set the eip
   * to L4_LOG2_SUPERPAGESIZE << 2! */
  if (EXPECT_FALSE( (*d1==last_pfp->pfa) && (*d2==last_pfp->eip)) )
    {
      *d1 &= L4_PAGEMASK;
      print_err("\r\n"
		"RMGR: task "l4util_idfmt" at %08x is trying to get page %08x",
		l4util_idstr(t), *d2, *d1);

      if (*d1 < mem_high + ram_base)
	{
	  owner_t owner = memmap_owner_page(*d1);
	  if (owner == O_RESERVED)
	    print_err(" which is reserved");
	  else
	    print_err(" allocated by task %x", owner);

	  if (!quota_alloc_mem(t.id.task, *d1, L4_PAGESIZE))
	    print_err("\r\n      (Cause: quota exceeded)");
	}
      else
	print_err(" (memhigh=%08x)", mem_high);

      print_err("\r\n");

      enter_kdebug("double page fault");
    }

  last_pfp->pfa = *d1;
  last_pfp->eip = *d2;

  return 0;
}

/**
 * Find a free page.
 */
static inline int
find_free(l4_threadid_t t,l4_umword_t pfa,
	  l4_umword_t *d1,l4_umword_t *d2)
{
  /* XXX this routine should be in the memmap_*() interface because we
     don't know about quotas here.  we can easily select a free page
     which we later can't allocate because we're out of quota. */

  l4_addr_t address;

  for (address = ram_base; address - ram_base < mem_high;
       address += L4_SUPERPAGESIZE)
    {
      if (memmap_nrfree_superpage(address) == 0)
	continue;

      for (;address - ram_base < mem_high; address += L4_PAGESIZE)
	{
	  if (! quota_check_mem(t.id.task, address, L4_PAGESIZE))
	    continue;
	  if (memmap_owner_page(address) != O_FREE)
	    continue;

	  /* found! */
	  *d1 = address;
	  *d2 = l4_fpage(address, L4_LOG2_PAGESIZE,
	                 L4_FPAGE_RW, L4_FPAGE_MAP).fpage;

	  return 1;
	}
    }
  return 0;
}

/**
 * Handle a pagefault which needs simply a free page.
 * This maps some free page back to sender.
 */
static inline int
handle_anypage(l4_threadid_t t,l4_umword_t pfa,
	       l4_umword_t *d1,l4_umword_t *d2)
{
  int v;

  if ((v = vm_find(t.id.task, pfa) > -1))
      pfa += vm_get_offset(v);

  if (find_free(t, pfa, d1, d2) && memmap_alloc_page(*d1, t.id.task))
    return 1;

  return 0;
}


/**
 * A request of a kernel info page. This works always...
 */
static inline int
handle_kip(l4_threadid_t t,l4_umword_t pfa,
	   l4_umword_t *d1,l4_umword_t *d2)
{
  *d1 = 0;
  *d2 = l4_fpage((l4_umword_t) kip, L4_LOG2_PAGESIZE,
		L4_FPAGE_RO, L4_FPAGE_MAP).fpage;
  return 1;
}

static inline int
handle_tbuf_status(l4_threadid_t t,l4_umword_t pfa,
		   l4_umword_t *d1, l4_umword_t *d2)
{
  l4_msgdope_t result;
  static l4_addr_t scratch = 0x40400000; /* XXX hardcoded */

  l4_fpage_unmap(l4_fpage(scratch, L4_LOG2_SUPERPAGESIZE, 0, 0),
		 L4_FP_FLUSH_PAGE|L4_FP_ALL_SPACES);
  l4_ipc_call(my_pager, L4_IPC_SHORT_MSG,
	      1, 0xff,
	      L4_IPC_MAPMSG(scratch, L4_LOG2_PAGESIZE),
	      d1, d2, L4_IPC_NEVER, &result);

  if (*d1 == 0 && *d2 == 0)
    /* no tbuf status from sigma0 */
    return 0;

  /* grant the tbuf status to the subtask */
  *d1 = pfa;
  *d2 = l4_fpage(scratch, L4_LOG2_PAGESIZE,
		 L4_FPAGE_RW, L4_FPAGE_GRANT).fpage;
  return 1;
}

/**
 * Trys to map a superpage.
 * Returns 1 if this was successfull, zero otherwise.
 */
static inline int
try_superpage_maping(l4_threadid_t t,l4_umword_t pfa,
		     l4_umword_t *d1,l4_umword_t *d2)
{
  /* map a specific page */
  if ((*d1 & 1) && (*d2 == (L4_LOG2_SUPERPAGESIZE << 2)))
    {
      /* superpage request */
      if (!no_pentium && memmap_alloc_superpage(pfa, t.id.task))
	{
	  *d1 &= L4_SUPERPAGEMASK;
	  *d2 = l4_fpage(*d1, L4_LOG2_SUPERPAGESIZE,
			 L4_FPAGE_RW, L4_FPAGE_MAP).fpage;

	  /* flush the superpage first so that contained 4K pages which
	   * already have been mapped to the task don't disturb the 4MB
	   * mapping */
	  l4_fpage_unmap(l4_fpage(*d1, L4_LOG2_SUPERPAGESIZE, 0, 0),
			 L4_FP_FLUSH_PAGE|L4_FP_OTHER_SPACES);
	  return 1;
	}
    }
  return 0;
}

/**
 * A request of a physical 4 KB page at pfa.
 */
static inline int
handle_physicalpage(l4_threadid_t t,l4_umword_t pfa,
		    l4_umword_t *d1,l4_umword_t *d2)
{
  int v;

  /* check if address lays inside our memory. we do that
   * here because the memmap_* functions do not check
   * about that */
  if (EXPECT_FALSE(pfa >= mem_high + ram_base || pfa < ram_base))
    return check_double_pagefault(t,pfa,d1,d2);

  if (EXPECT_FALSE(pfa < L4_PAGESIZE))
    /* from now on, we do not implicit map page 0. To
     * map page 0, do it explicit by rmgr_get_page0() */
    return check_double_pagefault(t,pfa,d1,d2);

  if ((v = vm_find(t.id.task, pfa) > -1))
      pfa += vm_get_offset(v);

  if (try_superpage_maping(t,pfa,d1,d2))
    return 1;

  /* failing a superpage allocation, try a single page
     allocation */
  if (memmap_alloc_page(*d1, t.id.task))
    {
      (*d1) &= L4_PAGEMASK;
      (*d2) = l4_fpage(pfa, L4_LOG2_PAGESIZE,
		       L4_FPAGE_RW, L4_FPAGE_MAP).fpage;
      return 1;
    }

  if (pfa >= (mem_lower << 10) && pfa < (1 << 20))
    {
      /* This is for the BIOS adapter area.
       * Page faults are OK here, because every task should
       * e.g. write in the video memory.
       */
      (*d1) &= L4_PAGEMASK;
      (*d2) = l4_fpage(*d1, L4_LOG2_PAGESIZE,
		       L4_FPAGE_RW, L4_FPAGE_MAP).fpage;
      return 1;
    }

  return check_double_pagefault(t,pfa,d1,d2);
}

/**
 * A request of a 4 MB superpage in the adapter area
 * at pfa + 0x40000000.
 *
 * The adapter area is between 2 and 4 GB but it is mapped in the
 * sigma0 protocol to the range 1 GB until 3 GB.
 */
static inline int
handle_adapterpage(l4_threadid_t t,l4_umword_t pfa,
		   l4_umword_t *d1, l4_umword_t *d2)
{
  /* XXX UGLY HACK! */
  l4_msgdope_t result;

  static l4_addr_t scratch = 0x40000000; /* XXX hardcoded */

  pfa &= L4_SUPERPAGEMASK;

  // we need no size check, because adapter pages work always
  if (memmap_alloc_superpage(pfa + 0x40000000, t.id.task))
    {
      /* map the superpage into a scratch area */
      l4_fpage_unmap(l4_fpage(scratch, L4_LOG2_SUPERPAGESIZE, 0,0),
		     L4_FP_FLUSH_PAGE|L4_FP_ALL_SPACES);
      l4_ipc_call(my_pager, L4_IPC_SHORT_MSG,
		  pfa, 0,
		  L4_IPC_MAPMSG(scratch, L4_LOG2_SUPERPAGESIZE),
		  d1, d2, L4_IPC_NEVER, &result);

      /* grant the superpage to the subtask */
      *d1 = pfa;
      *d2 = l4_fpage(scratch, L4_LOG2_SUPERPAGESIZE,
		     L4_FPAGE_RW, L4_FPAGE_GRANT).fpage;
      return 1;
    }

  return 0;
}

/*
   handle IO page faults sender sends:
               4    16    4   6   2
	     +---+------+---+---+--+
   d1        | F | port | 0 | s |~~|
             +---+------+---+---+--+
             +---------------------+
   d2        |    EIP (ignored)    |
             +---------------------+
*/
static inline int
handle_io(l4_threadid_t t,l4_umword_t pfa,
	  l4_umword_t *d1,l4_umword_t *d2)
{
#ifdef ARCH_x86
  unsigned i;
  unsigned port = ((l4_fpage_t)(*d1)).iofp.iopage;
  unsigned size = ((l4_fpage_t)(*d1)).iofp.iosize;
  last_pf_t *last_pfp = last_pf + t.id.task;

//  owner_short_name(t.id.task, n1, sizeof(n1));

  /* Don't try to allocate any port if a task ``only'' wants to execute
   * cli or sti since there may be more than one task trying to do this */
  if (EXPECT_FALSE (!have_io))
    {
      print_err("RMGR: Task #%02x asks for I/O ports %04x-%04x but we "
		"don't own any\n", t.id.task, port, port+(1<<size)-1);
      last_pfp->pfa = *d1;
      last_pfp->eip = *d2;
      return 0;
    }

  if (port == 0 && size == L4_WHOLE_IOADDRESS_SPACE)
    {
      if (EXPECT_FALSE(!quota_check_allow_cli(t.id.task)))
	{
	  print_err("RMGR: Task #%02x is not allowed to execute cli/sti\n",
		    t.id.task);
	  if (EXPECT_FALSE(last_pfp->pfa == *d1 && last_pfp->eip == *d2))
	    enter_kdebug("double I/O fault");

	  last_pfp->pfa = *d1;
	  last_pfp->eip = *d2;
	  return 0;
	}

      print_err("RMGR: Sending all ports (for cli/sti) to task #%02x\n",
	        t.id.task);
      *d2 = l4_iofpage(port, size, 0).fpage;
      *d1 = 0;
      return 1;
    }

  for (i=port; i<port+(1<<size); i++)
    {
      if (EXPECT_FALSE(!iomap_alloc_port(i, t.id.task)))
	{
	  last_pf_t *last_pfp = last_pf + t.id.task;
	  owner_t owner       = iomap_owner_port(i);

	  if (EXPECT_FALSE(*d1==last_pfp->pfa && *d2==last_pfp->eip))
	    {
	      print_err("\r\n"
			"RMGR: task "l4util_idfmt" at %08x is trying to "
			"get I/O port %04x",
			l4util_idstr(t), *d2, i);

	      if (owner == O_RESERVED)
		print_err(" which is reserved\r\n");
	      else
		print_err(" allocated by task %x\r\n", owner);

	      enter_kdebug("double page fault");
	    }

//	  owner_short_name(owner, n2, sizeof(n2));
	  print_err("RMGR: Cannot send port %04x to task #%02x, "
		    "owner is #%02x \n",
		    i, t.id.task, owner);
	  last_pfp->pfa = *d1;
	  last_pfp->eip = *d2;
	  return 0;
	}
    }

  print_err("RMGR: Sending ports %04x-%04x to task #%02x\n",
      port, port+(1<<size)-1, t.id.task);
  *d2 = l4_iofpage(port, size, 0).fpage;
  *d1 = 0;
#endif /* ARCH_x86 */
  return 1;
}


void
pager(void)
{
  l4_umword_t d1, d2, pfa;
  void *desc;
  int err;
  int handled;
  l4_threadid_t t;
  l4_msgdope_t result;

  /* we (the main thread of this task) will serve as a pager for our
     children. */

  /* now start serving the subtasks */
  for (;;)
    {
      err = l4_ipc_wait(&t, 0, &d1, &d2, L4_IPC_NEVER, &result);

      while (!err)
	{
	  //printf("Root: Received PF from " l4util_idfmt ": pfa: %08x ip: %08x ret = %d\n", l4util_idstr(t), d1, d2, err);
	  /* we must synchronise the access to memmap functions */
	  enter_memmap_functions(RMGR_LTHREAD_PAGER, rmgr_super_id);

	  /* we received a paging request here */
	  /* handle the sigma0 protocol */
	  handled = 0;

	  if (t.id.task <= O_MAX)
	    /* valid requester */
	    {
	      pfa = d1 & 0xfffffffc;

	      if (d1 == 0xfffffffc)
		handled=handle_anypage(t,pfa,&d1,&d2);
	      else if (d1 == 1 && (d2 & 0xff) == 1)
		handled=handle_kip(t,pfa,&d1,&d2);
	      else if (d1 == 1 && (d2 & 0xff) == 0xff)
		handled=handle_tbuf_status(t,pfa,&d1,&d2);
#ifdef ARCH_x86
	      else if (pfa < 0x40000000)
		handled=handle_physicalpage(t,pfa,&d1,&d2);
#else
	      else if (pfa > ram_base && (pfa - ram_base) < (64 << 20))
		handled=handle_physicalpage(t,pfa,&d1,&d2);
#endif
#ifdef ARCH_x86
	      else if (pfa >= 0x40000000 && pfa < 0xC0000000 && !(d1 & 1))
		handled=handle_adapterpage(t,pfa,&d1,&d2);
	      else if ((l4_version == VERSION_FIASCO)
		       && l4_is_io_page_fault(d1))
		handled=handle_io(t,pfa,&d1,&d2);
#endif
	      else
	        print_err("\r\nRMGR: can't handle d1=0x%08x, d2=0x%08x "
			  "from thread="l4util_idfmt"\n",
			  d1, d2, l4util_idstr(t));
	    }
	  else
	    /* OOPS.. can't map to this sender. */
	    ;

	  if (handled)
	    {
	      desc = L4_IPC_SHORT_FPAGE;
	    }
	  else
	    {
	      // something goes wrong
	      d1 = d2 = 0;
	      desc = L4_IPC_SHORT_MSG;
	    }

	  leave_memmap_functions(RMGR_LTHREAD_PAGER, rmgr_super_id);

	  /* send reply and wait for next message */
	  //printf("Root: reply to " l4util_idfmt " d1=%08x d2=%08x\n", l4util_idstr(t), d1, d2);
	  err = l4_ipc_reply_and_wait(t, desc, d1, d2,
				      &t, 0, &d1, &d2,
				      L4_IPC_SEND_TIMEOUT_0,
				      /* snd timeout = 0 */
				      &result);

	  /* send error while granting?  flush fpage! */
	  if (err == L4_IPC_SETIMEOUT
	      && desc == L4_IPC_SHORT_FPAGE
	      && (d2 & 1))
	    l4_fpage_unmap((l4_fpage_t) d2, L4_FP_FLUSH_PAGE|L4_FP_ALL_SPACES);
	}
    }
}
