PKGDIR        ?= ../..
L4DIR         ?= $(PKGDIR)/../..
SYSTEMS       ?= x86-l4v2 x86-linux amd64-l4v2 arm-l4v2

TARGET         = libcommon.a
INSTALL_TARGET =
PRIVATE_INCDIR = $(PKGDIR)/server/include \
                 $(PKGDIR)/include

include $(L4DIR)/mk/Makeconf

ifeq ($(ARCH),arm)
COL_MODE       = multi
endif
ifeq ($(ARCH),amd64)
COL_MODE       = multi
endif

SRC_C          = appman.c     background.c button.c    scale.c       \
                 cache.c      clipping.c   container.c conv_fnt.c    \
                 scheduler.c  fontman.c    frame.c     grid.c        \
                 hashtab.c    keymap.c     label.c     variable.c    \
                 main.c       pool.c       viewman.c   redraw.c      \
                 tick.c       script.c     scrollbar.c relax.c       \
                 tokenizer.c  userstate.c  widman.c    window.c      \
                 winlayout.c  vscreen.c    gfx.c       loaddisplay.c \
                 gfx_img16.c  scope.c     gfx_img32.c  gfx_yuv420.c  \
                 dopestd.c    screen.c    vtextscreen.c entry.c

ifeq ($(COL_MODE),multi)
SRC_CC         += gfx_scr_gen.cc
else
SRC_C          += gfx_scr16.c
endif

#include $(L4DIR)/pkg/petze/mk/petze.mk
include $(L4DIR)/mk/lib.mk

