#include <l4/cxx/thread.h>
#include <l4/cxx/iostream.h>
#include <l4/cxx/l4iostream.h>

#include <l4/sys/syscalls.h>
#include <l4/sys/ipc.h>
#include <l4/util/util.h>


//  using ::l4_threadid_t;
//  typedef int l4_threadid_t;

namespace L4 {
  

  unsigned Thread::_lthread;
  l4_threadid_t Thread::_pager = L4_INVALID_ID_INIT;
  l4_threadid_t Thread::_master = L4_INVALID_ID_INIT;
  l4_threadid_t Thread::_preempter = L4_INVALID_ID_INIT;

  void Thread::start()
  {
    if(l4_thread_equal(self(),_master)) {
      run();
    } else {
      l4_umword_t dummy;
      l4_threadid_t pa = _pager;
      l4_threadid_t pr = _preempter;
      l4_msgdope_t res;
      *(((l4_umword_t*)_stack)--) = (l4_umword_t)this;
      l4_thread_ex_regs(self(), 
                        (l4_umword_t)start_cxx_thread, 
                        (l4_umword_t)_stack,
                        &pr,
                        &pa, &dummy, &dummy, &dummy );
      
      if(l4_ipc_send(self(), 0, 0, 0, L4_IPC_NEVER, &res )!=0)
        L4::cerr << "ERROR: (master) error while thread handshake: "
                 << _master << "->" << self() << "\n";
    }
  }

  void Thread::execute()
  {
    l4_threadid_t src;
    l4_msgdope_t res;
    l4_umword_t d1,d2;
    if(l4_ipc_wait(&src, 0, &d1, &d2, L4_IPC_NEVER, &res)!=0)
      L4::cerr << "ERROR: (slave) error while thread handshake: " 
               << self() << "\n";
    run();
    shutdown();
  };

  void Thread::shutdown()
  {
    l4_sleep_forever();
  }

  Thread::~Thread()
  {
#if 0
    L4::cerr << "~Thread[" << self() << "]() called from " 
             << l4_myself() << " @" << L4::hex 
             << __builtin_return_address(0) << "\n";
#endif
    if(!l4_thread_equal(self(),_master))
      {
        l4_threadid_t pa = L4_INVALID_ID_INIT;
        l4_threadid_t pr = L4_INVALID_ID_INIT;
        l4_umword_t dummy;
        l4_thread_ex_regs(self(), 
                          (l4_umword_t)kill_cxx_thread, 
                          (l4_umword_t)-1,
                          &pr,
                          &pa,  &dummy, &dummy, &dummy );
      }
  }

  void Thread::kill_cxx_thread()
  {
    l4_sleep_forever();
  }
 

};
