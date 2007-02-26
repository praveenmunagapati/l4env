#include <l4/util/irq.h>
#include <l4/log/l4log.h>
#include "config.h"
#ifdef OMEGA0_USE_PIC_LOCKING
#include "lock.h"
#endif
#include "globals.h"
#include "pic.h"

#ifdef OMEGA0_USE_PIC_LOCKING
/* locking stuff */
static wq_lock_queue_base pic_wq = {NULL};
#endif

void irq_mask(int irq){
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_queue_elem wqe;
#endif

  #ifdef OMEGA0_DEBUG_PIC
  LOGl("(%x) called", irq);
  #endif

#ifdef OMEGA0_USE_PIC_LOCKING
  if(wq_lock_lock(&pic_wq, &wqe)) LOGl("error locking pic-waitqueue");
#endif

  l4util_cli();
  if(irq<8){
    l4util_out8(l4util_in8(0x21) | (1<<irq), 0x21);  
  } else {
    l4util_out8(l4util_in8(0xa1) | (1<<(irq-8)), 0xa1);  
  }
  l4util_sti();
  
#ifdef OMEGA0_USE_PIC_LOCKING
  if(wq_lock_unlock(&pic_wq, &wqe)) LOGl("error unlocking pic-waitqueue");
#endif
}

void irq_unmask(int irq){
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_queue_elem wqe;
#endif

  #ifdef OMEGA0_DEBUG_PIC
  LOGl("(%x) called", irq);
  #endif

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_lock(&pic_wq, &wqe);
#endif
  
  l4util_cli();
  if(irq<8){
    l4util_out8(l4util_in8(0x21) & ~(1<<irq), 0x21);  
  } else {
    l4util_out8(l4util_in8(0xa1) & ~(1<<(irq-8)), 0xa1);
  }
  l4util_sti();
  
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_unlock(&pic_wq, &wqe);
#endif
}

void irq_ack(int irq){
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_queue_elem wqe;
#endif

  #ifdef OMEGA0_DEBUG_PIC
  LOGl("(%x) called", irq);
  #endif

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_lock(&pic_wq, &wqe);
#endif
  
  l4util_cli();
  #ifdef OMEGA0_STRATEGY_SPECIFIC_EOI
    if (irq > 7){
      l4util_out8(0x60|(irq&7),0xA0);
      l4util_out8(0x0B,0xA0);
      if (l4util_in8(0xA0) == 0)  l4util_out8(0x62, 0x20);
    }else{
      l4util_out8(0x60|irq,0x20);
    }
  #else
    if (irq > 7){
      l4util_out8(0x20, 0xA0);
      l4util_out8(0x0B, 0xA0);
      if (l4util_in8(0xA0) == 0)  l4util_out8(0x20, 0x20);
    }else{
      l4util_out8(0x20,0x20);
    }
  #endif
  l4util_sti();
  
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_unlock(&pic_wq, &wqe);
#endif
}

/* return the interrupt service register (isr). This register holds the
   irq's which are accepted by the processor and not acknowledged.  if
   master!=0, return master isr, else return slave isr */
int pic_isr(int master){
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_queue_elem wqe;
#endif
  int dat;

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_lock(&pic_wq, &wqe);
#endif
  
  l4util_cli();
  if (master){
    l4util_out8(0xb,0xA0);
    dat = l4util_in8(0xa0);
  }else{
    l4util_out8(0xb,0x20);
    dat = l4util_in8(0x20);
  }
  l4util_sti();

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_unlock(&pic_wq, &wqe);
#endif
  
  return dat;
}

/* return the interrupt request register (irr). This register holds the
   irq's which are requested by hardware but not delivered to the processor. 
   if master!=0, return master irr, else return slave irr */
int pic_irr(int master){
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_queue_elem wqe;
#endif
  int dat;

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_lock(&pic_wq, &wqe);
#endif
  
  l4util_cli();
  if (master){
    l4util_out8(0xa,0xA0);
    dat = l4util_in8(0xa0);
  }else{
    l4util_out8(0xa,0x20);
    dat = l4util_in8(0x20);
  }
  l4util_sti();

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_unlock(&pic_wq, &wqe);
#endif
  
  return dat;
}

/* return the interrupt mask register.
   if master!=0, return master isr, else return slave isr */
int pic_imr(int master){
#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_queue_elem wqe;
#endif
  int dat;

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_lock(&pic_wq, &wqe);
#endif
  
  l4util_cli();
  if (master){
    dat = l4util_in8(0xa1);
  }else{
    dat = l4util_in8(0x21);
  }
  l4util_sti();

#ifdef OMEGA0_USE_PIC_LOCKING
  wq_lock_unlock(&pic_wq, &wqe);
#endif
  
  return dat;
}
