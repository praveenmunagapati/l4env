PKGDIR		?= ../..
L4DIR		?= $(PKGDIR)/../..

TARGET		= simple_ts_exit
DEFAULT_RELOC	= 0x00A00000

SRC_C		= main.c
LIBS		= -lgeneric_ts -ldm_phys 

include $(L4DIR)/mk/prog.mk
