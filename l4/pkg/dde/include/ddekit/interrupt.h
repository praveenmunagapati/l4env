/*
 * \brief   Hardware-interrupt subsystem
 * \author  Thomas Friebel <tf13@os.inf.tu-dresden.de>
 * \author  Christian Helmuth <ch12@os.inf.tu-dresden.de>
 * \date    2007-01-26
 *
 * DDEKit supports registration of one handler function per interrupt. If any
 * specific DDE implementation needs to register more than one handler,
 * multiplexing has to be implemented there!
 *
 * FIXME IRQ detach missing
 */

#ifndef _ddekit_interrupt_h
#define _ddekit_interrupt_h

#include <l4/dde/ddekit/thread.h>

/**
 * Attach to hardware interrupt
 *
 * \param irq          IRQ number to attach to
 * \param shared       set to 1 if interrupt sharing is supported; set to 0
 *                     otherwise
 * \param thread_init  called just after DDEKit internal init and before any
 *                     other function
 * \param handler      IRQ handler for interrupt irq
 * \param priv         private token (argument for thread_init and handler)
 *
 * \return pointer to interrupt thread created
 */
ddekit_thread_t *ddekit_interrupt_attach(int irq, int shared,
                                         void(*thread_init)(void *),
                                         void(*handler)(void *), void *priv);

#endif