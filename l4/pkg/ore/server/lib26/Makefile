PKGDIR	?= ../..
L4DIR	?= $(PKGDIR)/../..

-include $(OBJ_BASE)/pkg/ore/server/Makeconf.bid.local
# the default is to build the listed directories, provided that they
# contain a Makefile. If you need to change this, uncomment the following
# line and adapt it.
ifeq ($(CONFIG_ORE_DDE24),)
TARGET = server-worker linuxemul net
endif

include $(L4DIR)/mk/subdir.mk
