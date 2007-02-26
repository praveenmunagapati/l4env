/*
 * \brief   DOpE gfx 8bit indexed image handler module
 * \date    2003-04-07
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

#include "dopestd.h"
#include "gfx_handler.h"
#include "gfx.h"

struct gfx_ds_data {
	s32 w,h;            /* width and height of the image */
	u8 *pixels;        /* 8bit indices of the pixels */
};

int init_gfximg8i(struct dope_services *d);

/*****************************/
/*** GFX HANDLER FUNCTIONS ***/
/*****************************/

static s32 img_get_width(struct gfx_ds_data *img) {
	return img->w;
}

static s32 img_get_height(struct gfx_ds_data *img) {
	return img->h;
}

static s32 img_get_type(struct gfx_ds_data *img) {
	return GFX_IMG_TYPE_INDEX8;
}

static void img_destroy(struct gfx_ds_data *img) {
	free(img);
}

static void *img_map(struct gfx_ds_data *img) {
	return img->pixels;
}

/*************************/
/*** SERVICE FUNCTIONS ***/
/*************************/


static struct gfx_ds_data *create(s32 width, s32 height) {
	struct gfx_ds_data *new;
	new = malloc(sizeof(struct gfx_ds_data) + width*height);
	if (!new) return NULL;
	new->w = width;
	new->h = height;
	new->pixels = (u8 *)((adr)new + sizeof(struct gfx_ds_data));
	return new;
}

static s32 register_gfx_handler(struct gfx_ds_handler *handler) {
	handler->get_width  = img_get_width;
	handler->get_height = img_get_height;
	handler->get_type   = img_get_type;
	handler->destroy    = img_destroy;
	handler->map        = img_map;
	return 0;
}


/****************************************/
/*** SERVICE STRUCTURE OF THIS MODULE ***/
/****************************************/

static struct gfx_handler_services services = {
	create,
	register_gfx_handler,
};


/**************************/
/*** MODULE ENTRY POINT ***/
/**************************/

int init_gfximg8i(struct dope_services *d) {
	d->register_module("GfxImage8i 1.0",&services);
	return 1;
}
