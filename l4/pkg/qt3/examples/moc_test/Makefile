PKGDIR		?= ../..
L4DIR		?= $(PKGDIR)/../..

SYSTEMS		= x86-l4v2
DEFAULT_RELOC	= 0x2800000

TARGET		= qt3_moc
MODE		= l4env_base

SRC_CC		= main.cc
SRC_MOC		= class_c.h

include $(PKGDIR)/mk/qt3.mk
include $(L4DIR)/mk/prog.mk
