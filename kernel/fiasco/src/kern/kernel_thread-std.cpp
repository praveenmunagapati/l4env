IMPLEMENTATION:

#include "config.h"
#include "initcalls.h"
#include "mem_layout.h"
#include "task.h"
#include "thread.h"
#include "types.h"
#include "space_index.h"

IMPLEMENT 
void
Kernel_thread::init_workload()
{
  //
  // allow the boot task to create more tasks
  //
  for (unsigned i = Config::boot_taskno + 1; 
       i < Space_index::Max_space_number;
       i++)
    check (Space_index (i).set_chief
	   (space_index(), Space_index (Config::boot_taskno)));

  //
  // create sigma0
  //

  // sigma0's chief is the boot task
  Space_index (Config::sigma0_id.task()).set_chief
    (space_index(), Space_index (Config::sigma0_id.chief()));

  sigma0_task = new Task (Config::sigma0_id.task());
  sigma0_task->initialize();
  sigma0_space = sigma0_task->mem_space();

  Thread *sigma0_thread = id_to_tcb (Config::sigma0_id);
  {
    sigma0_thread->enforce_tcb_alloc();

    Lock_guard <Thread_lock> guard (sigma0_thread->thread_lock());

    new (Config::sigma0_id) Thread (sigma0_task, Config::sigma0_id,
				    Config::sigma0_prio, Config::sigma0_mcp);

    Address sp = init_workload_s0_stack();
    sigma0_thread->initialize (Kip::k()->sigma0_ip, sp, 0, 0, 0);
  }
  
  //
  // create the boot task
  //

  // the boot task's chief is the kernel
  Space_index (Config::boot_id.task()).set_chief
    (space_index(), Space_index (Config::boot_id.chief()));
  
  Task *boot_task = new Task (Config::boot_id.task());
  boot_task->initialize();
  sigma0_thread->setup_task_caps (boot_task, 0, 0);

  Thread *boot_thread = id_to_tcb (Config::boot_id);
  {
    boot_thread->enforce_tcb_alloc();

    Lock_guard <Thread_lock> guard (boot_thread->thread_lock());

    new (Config::boot_id) Thread (boot_task, Config::boot_id,
				  Config::boot_prio, Config::boot_mcp);

    boot_thread->initialize(Kip::k()->root_ip,
			    Kip::k()->root_sp,
			    sigma0_thread, 0, 0);
  }
}

IMPLEMENTATION [ia32,amd64]:

PRIVATE inline
Address
Kernel_thread::init_workload_s0_stack()
{
  // push address of kernel info page to sigma0's stack
  Address sp = Kip::k()->sigma0_sp - sizeof(Mword);
  *Mem_layout::boot_data (reinterpret_cast<Address*>(sp))
    = Kmem::virt_to_phys (Kip::k());
  return sp;
}

IMPLEMENTATION [ux,arm]:

PRIVATE inline
Address
Kernel_thread::init_workload_s0_stack()
{ return Kip::k()->sigma0_sp; }
