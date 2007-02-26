/*
 * \brief   DOpE input module
 * \date    2002-11-13
 * \author  Norman Feske <nf2@inf.tu-dresden.de>
 */

/*
 * Copyright (C) 2002-2003  Norman Feske  <nf2@os.inf.tu-dresden.de>
 * Technische Universitaet Dresden, Operating Systems Research Group
 *
 * This file is part of the DOpE package, which is distributed under
 * the  terms  of the  GNU General Public Licence 2.  Please see the
 * COPYING file for details.
 */

/*** L4 INCLUDES ***/
#include <l4/thread/thread.h>
#include <l4/generic_io/libio.h>
#include <l4/input/libinput.h>

/*** LOCAL INCLUDES ***/
#include "dopestd.h"
#include "keymap.h"
#include "event.h"
#include "widget.h"
#include "window.h"
#include "scrdrv.h"
#include "input.h"

/* XXX in startup.c (we need a property mgr?) */
extern l4io_info_t *l4io_page;

static long scr_w=200,scr_h=200;
static long curr_mx,curr_my,curr_mb;
static char keytab[KEY_MAX];

static struct scrdrv_services *scrdrv;
static struct keymap_services *keymap;

int init_input(struct dope_services *d);

/*************************/
/*** SERVICE FUNCTIONS ***/
/*************************/


static long get_mx(void) {return curr_mx;}
static long get_my(void) {return curr_my;}
static long get_mb(void) {return curr_mb;}


/*** REQUEST INPUT EVENTS AND UPDATE MOUSE POSITION/FOCUS ***/
static void update(WIDGET *dst) {
	static int rd, i;
	long new_mx=curr_mx;
	long new_my=curr_my;
	WINDOW *w;
	EVENT e;
	static l4input_t ev[64];

	if (!l4input_ispending()) return;
	rd = l4input_flush(ev, 64);
	for (i = 0; i < rd; i++) {
		switch (ev[i].type) {
		case EV_REL:    /* relative position changed */
			if (ev[i].code) {
				/* y motion */
				new_my+=ev[i].value;
			} else {
				/* x motion */
				new_mx+=ev[i].value;
			}
			break;
		case EV_KEY:    /* key pressed or leaved */
			if (ev[i].value) {

				/* key or button pressed */
				INFO(printf("Mouse(update): pressed\n");)

				if (dst) {
					w = (WINDOW *)dst->gen->get_window(dst);
					if (w) w->win->activate(w);
				}

				if (ev[i].code == BTN_LEFT)  curr_mb = curr_mb | 0x01;
				if (ev[i].code == BTN_RIGHT) curr_mb = curr_mb | 0x02;
				e.type=EVENT_PRESS;
				e.code=ev[i].code;
				keytab[ev[i].code]=1;
				if (dst) dst->gen->handle_event(dst,&e);

			} else {

				/* key or button button released */
				INFO(printf("Mouse(update): released\n");)
				if (ev[i].code == BTN_LEFT)  curr_mb = curr_mb & 0x00fe;
				if (ev[i].code == BTN_RIGHT) curr_mb = curr_mb & 0x00fd;
				e.type=EVENT_RELEASE;
				e.code=ev[i].code;
				keytab[ev[i].code]=0;
				if (dst) dst->gen->handle_event(dst,&e);
			}
			break;
		}
	}

	if ((new_mx!=curr_mx) || (new_my!=curr_my)) {
		curr_mx=new_mx;
		curr_my=new_my;

		if (curr_mx<0) curr_mx=0;
		if (curr_mx>=scr_w) curr_mx=scr_w-1;
		if (curr_my<0) curr_my=0;
		if (curr_my>=scr_h) curr_my=scr_h-1;
	}

}


/*** FORCE A NEW MOUSE POSITION ***/
static void set_pos(long new_mx,long new_my) {
	curr_mx = new_mx;
	curr_my = new_my;
	update(NULL);
}


/*** GET CURRENT STATE OF THE KEY WITH THE SPECIFIED KEYCODE ***/
static long get_keystate(long keycode) {
	if (keycode>=KEY_MAX) return 0;
	return keytab[keycode];
}


/*** DETERMINE ASCII VALUE OF A KEY WHILE TAKING MODIFIER KEYS INTO ACCOUNT ***/
static char get_ascii(long keycode) {
	long switches=0;
	if (keycode>=KEY_MAX) return 0;
	if (keytab[42] ) switches = switches | KEYMAP_SWITCH_LSHIFT;
	if (keytab[54] ) switches = switches | KEYMAP_SWITCH_RSHIFT;
	if (keytab[29] ) switches = switches | KEYMAP_SWITCH_LCONTROL;
	if (keytab[97] ) switches = switches | KEYMAP_SWITCH_RCONTROL;
	if (keytab[56] ) switches = switches | KEYMAP_SWITCH_ALT;
	if (keytab[100]) switches = switches | KEYMAP_SWITCH_ALTGR;
	return keymap->get_ascii(keycode,switches);
}


static void update_properties(void) {
	scr_w = scrdrv->get_scr_width();
	scr_h = scrdrv->get_scr_height();

	if (curr_mx<0) curr_mx=0;
	if (curr_mx>=scr_w) curr_mx=scr_w-1;
	if (curr_my<0) curr_my=0;
	if (curr_my>=scr_h) curr_my=scr_h-1;
}



/****************************************/
/*** SERVICE STRUCTURE OF THIS MODULE ***/
/****************************************/

static struct input_services input = {
	get_mx,
	get_my,
	get_mb,
	set_pos,
	get_keystate,
	get_ascii,
	update,
	update_properties
};



/**************************/
/*** MODULE ENTRY POINT ***/
/**************************/

int init_input(struct dope_services *d) {
	int init_ret;
        /* XXX this works only if OMEGA0 is not running stand-alone */
	init_ret=l4input_init(l4io_page ? l4io_page->omega0 : 0,
                              L4THREAD_DEFAULT_PRIO,(void *)0);
	INFO(printf("Input(init): l4input_init() returned %d\n",init_ret);)

	scrdrv = d->get_module("ScreenDriver 1.0");
	keymap = d->get_module("Keymap 1.0");

	d->register_module("Input 1.0",&input);
	return 1;
}
