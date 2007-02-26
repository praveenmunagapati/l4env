#ifndef _DRAWMETHODS_H
#define _DRAWMETHODS_H

#include "config.h"

#define DRAWMETHOD_NORMAL(adr,col) {*(adr) = (col);}

#ifdef HAVE_MMX
#include "mmx.h"

#define DRAWMETHOD_PLUS(_out,_backbuf,_col) \
{\
movd_m2r (_backbuf, mm0); \
paddusb_m2r (_col, mm0); \
movd_r2m (mm0, _out); \
}

/*
paddusb (_out
__asm__		("movd %%eax,%%mm0\n movd %%edx,%%mm1\n paddusb %%mm1,%%mm0\n movd %%mm0,%%eax"\
:"=eax"(_out):"eax"(_backbuf),"edx"(_col));\
}
*/

#else
#define DRAWMETHOD_PLUS(_out,_backbuf,_col) \
{\
      int tra=0,i=0;\
      unsigned char *bra = (unsigned char*)&(_backbuf);\
      unsigned char *dra = (unsigned char*)&(_out);\
      unsigned char *cra = (unsigned char*)&(_col);\
      for (;i<4;i++) {\
				tra = *cra;\
				tra += *bra;\
				if (tra>255) tra=255;\
				*dra = tra;\
				++dra;++cra;++bra;\
			}\
}
#endif

#define DRAWMETHOD_OR(adr,col) {*(adr)|=(col);}

#ifdef HAVE_MMX
#define DRAWMETHOD_DONE() {__asm__ __volatile__ ("emms");}
#else
#define DRAWMETHOD_DONE() {}
#endif

#ifndef DRAWMETHOD
#define DRAWMETHOD DRAWMETHOD_PLUS(*p,*p,col)
//#define DRAWMETHOD *p=col

static void inline
draw_line (int *data, int x1, int y1, int x2, int y2, int col, int screenx,
	   int screeny)
{
  int x, y, dx, dy, yy, xx;	// am, tmp;
  int *p;

//   DATA32 *p;
//   DATA8 aaa, nr, ng, nb, rr, gg, bb, aa, na;

  if ((y1 < 0) || (y2 < 0) || (x1 < 0) || (x2 < 0) || (y1 >= screeny)
      || (y2 >= screeny) || (x1 >= screenx) || (x2 >= screenx))
    return;

#if 0
  /* clip to top edge  */
  if ((y1 < 0) && (y2 < 0))
    return;

  if (y1 < 0)
  {
    x1 += (y1 * (x1 - x2)) / (y2 - y1);
    y1 = 0;
  }
  if (y2 < 0)
  {
    x2 += (y2 * (x1 - x2)) / (y2 - y1);
    y2 = 0;
  }

  /* clip to bottom edge  */
  if ((y1 >= screeny) && (y2 >= screeny))
    return;
  if (y1 >= screeny)
  {
    x1 -= ((screeny - y1) * (x1 - x2)) / (y2 - y1);
    y1 = screeny - 1;
  }
  if (y2 >= screeny)
  {
    x2 -= ((screeny - y2) * (x1 - x2)) / (y2 - y1);
    y2 = screeny - 1;
  }
  /* clip to left edge   */
  if ((x1 < 0) && (x2 < 0))
    return;
  if (x1 < 0)
  {
    y1 += (x1 * (y1 - y2)) / (x2 - x1);
    x1 = 0;
  }
  if (x2 < 0)
  {
    y2 += (x2 * (y1 - y2)) / (x2 - x1);
    x2 = 0;
  }
  /* clip to right edge  */
  if ((x1 >= screenx) && (x2 >= screenx))
    return;
  if (x1 >= screenx)
  {
    y1 -= ((screenx - x1) * (y1 - y2)) / (x2 - x1);
    x1 = screenx - 1;
  }
  if (x2 >= screenx)
  {
    y2 -= ((screenx - x2) * (y1 - y2)) / (x2 - x1);
    x2 = screenx - 1;
  }
#endif
  dx = x2 - x1;
  dy = y2 - y1;
  if (x1 > x2)
  {
    int tmp;

    tmp = x1;
    x1 = x2;
    x2 = tmp;
    tmp = y1;
    y1 = y2;
    y2 = tmp;
    dx = x2 - x1;
    dy = y2 - y1;
  }

  /* vertical line */
  if (dx == 0)
  {
    if (y1 < y2)
    {
      p = &(data[(screenx * y1) + x1]);
      for (y = y1; y <= y2; y++)
      {
	DRAWMETHOD;
	p += screenx;
      }
    }
    else
    {
      p = &(data[(screenx * y2) + x1]);
      for (y = y2; y <= y1; y++)
      {
	DRAWMETHOD;
	p += screenx;
      }
    }
    return;
  }
  /* horizontal line */
  if (dy == 0)
  {
    if (x1 < x2)
    {
      p = &(data[(screenx * y1) + x1]);
      for (x = x1; x <= x2; x++)
      {
	DRAWMETHOD;
	p++;
      }
      return;
    }
    else
    {
      p = &(data[(screenx * y1) + x2]);
      for (x = x2; x <= x1; x++)
      {
	DRAWMETHOD;
	p++;
      }
      return;
    }
  }
  /* 1    */
  /* \   */
  /* \  */
  /* 2 */
  if (y2 > y1)
  {
    /* steep */
    if (dy > dx)
    {
      dx = ((dx << 16) / dy);
      x = x1 << 16;
      for (y = y1; y <= y2; y++)
      {
	xx = x >> 16;
	p = &(data[(screenx * y) + xx]);
	DRAWMETHOD;
	if (xx < (screenx - 1))
	{
	  p++;
//                                 DRAWMETHOD;
	}
	x += dx;
      }
      return;
    }
    /* shallow */
    else
    {
      dy = ((dy << 16) / dx);
      y = y1 << 16;
      for (x = x1; x <= x2; x++)
      {
	yy = y >> 16;
	p = &(data[(screenx * yy) + x]);
	DRAWMETHOD;
	if (yy < (screeny - 1))
	{
	  p += screeny;
//                                       DRAWMETHOD;
	}
	y += dy;
      }
    }
  }
  /* 2 */
  /* /  */
  /* /   */
  /* 1    */
  else
  {
    /* steep */
    if (-dy > dx)
    {
      dx = ((dx << 16) / -dy);
      x = (x1 + 1) << 16;
      for (y = y1; y >= y2; y--)
      {
	xx = x >> 16;
	p = &(data[(screenx * y) + xx]);
	DRAWMETHOD;
	if (xx < (screenx - 1))
	{
	  p--;
//                                 DRAWMETHOD;
	}
	x += dx;
      }
      return;
    }
    /* shallow */
    else
    {
      dy = ((dy << 16) / dx);
      y = y1 << 16;
      for (x = x1; x <= x2; x++)
      {
	yy = y >> 16;
	p = &(data[(screenx * yy) + x]);
	DRAWMETHOD;
	if (yy < (screeny - 1))
	{
	  p += screeny;
//                                 DRAWMETHOD;
	}
	y += dy;
      }
      return;
    }
  }
}
#endif

#endif