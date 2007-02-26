/*
 * \brief   DOpE thread handling module
 * \date    2002-11-13
 * \author  Norman Feske <nf2@inf.tu-dresden.de>
 *
 * Component that provides functions to  handle
 * threads and related things (e.g. semaphores)
 * to the other components of DOpE.
 */

/*
 * Copyright (C) 2002-2003  Norman Feske  <nf2@os.inf.tu-dresden.de>
 * Technische Universitaet Dresden, Operating Systems Research Group
 *
 * This file is part of the DOpE package, which is distributed under
 * the  terms  of the  GNU General Public Licence 2.  Please see the
 * COPYING file for details.
 */

#define THREAD void
#define MUTEX struct mutex

#include <l4/thread/thread.h>
#include <l4/semaphore/semaphore.h>
#include "dopestd.h"
#include "thread.h"

MUTEX {
	l4semaphore_t sem;
	s8 locked_flag;
};



int init_thread(struct dope_services *d);




static unsigned long hex2u32(char *s) {
	int i;
	unsigned long result=0;
	for (i=0;i<8;i++,s++) {
		if (!(*s)) return result;
		result = result*16 + (*s & 0xf);
		if (*s > '9') result += 9;
	}
	return result;
}


/*********************/
/* SERVICE FUNCTIONS */
/*********************/

/*** CREATE NEW THREAD AND RETURN THREAD IDENTIFIER ***/
static THREAD *create_thread(void (*entry)(void *),void *arg) {
	return (THREAD *)l4thread_create(entry,arg,L4THREAD_CREATE_ASYNC);
}



/*** CREATE NEW MUTEX AND SET IT UNLOCKED ***/
static MUTEX *create_mutex(int init) {
	MUTEX *result;

	result = (MUTEX *)malloc(sizeof(MUTEX));
	if (!result) {
		INFO(printf("Thread(create_mutex): out of memory!\n");)
		return NULL;
	}

	if (init) {
		result->sem = L4SEMAPHORE_LOCKED;
		result->locked_flag = 1;

	} else {
		result->sem = L4SEMAPHORE_UNLOCKED;
		result->locked_flag = 0;
	}
	return (MUTEX *)result;
}



/*** DESTROY MUTEX ***/
static void destroy_mutex(MUTEX *m) {
	if (!m) return;
	free(m);
}


/*** LOCK MUTEX ***/
static void mutex_down(MUTEX *m) {
	if (!m) return;
	l4semaphore_down(&m->sem);
	m->locked_flag = 1;
}


/*** UNLOCK MUTEX ***/
static void mutex_up(MUTEX *m) {
	if (!m) return;
	l4semaphore_up(&m->sem);
	m->locked_flag = 0;
}


/*** TEST IF MUTEX IS LOCKED ***/
static s8 mutex_is_down(MUTEX *m) {
	if (!m) return 0;
	return m->locked_flag;
}


/*** CONVERT IDENTIFIER TO THREAD ID ***/
static int ident2thread(u8 *ident, THREAD *dst) {
	l4_threadid_t *tid = (l4_threadid_t *)dst;
	int i;
	if (!ident || !tid) return -1;

	/* check if identifier string length is valid */
	for (i=0;i<24;i++) {
		if (!ident[i]) return -1;
	}

	tid->lh.low  = hex2u32(ident+7);
	tid->lh.high = hex2u32(ident+16);
	return 0;
}


/****************************************/
/*** SERVICE STRUCTURE OF THIS MODULE ***/
/****************************************/

static struct thread_services services = {
	create_thread,
	create_mutex,
	destroy_mutex,
	mutex_down,
	mutex_up,
	mutex_is_down,
	ident2thread,
};


/**************************/
/*** MODULE ENTRY POINT ***/
/**************************/

int init_thread(struct dope_services *d) {
	d->register_module("Thread 1.0",&services);
	return 1;
}
