#
# libavcodec Makefile
# (c) 2000-2003 Fabrice Bellard
#
PKGDIR	= ../../..

include config.mak

#VPATH=$(SRC_PATH)/libavcodec

# NOTE: -I.. is needed to include config.h
CFLAGS= $(OPTFLAGS) -Wall -I.. -I$(SRC_PATH) -I../../../include -I../../../lib/include -DHAVE_AV_CONFIG_H -D_GNU_SOURCE
#org: CFLAGS= $(OPTFLAGS) -Wall -DHAVE_AV_CONFIG_H -I.. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE

OBJS= common.o utils.o mem.o allcodecs.o \
      mpegvideo.o jrevdct.o jfdctfst.o jfdctint.o\
      mpegaudio.o ac3enc.o mjpeg.o resample.o dsputil.o \
      motion_est.o imgconvert.o imgresample.o \
      mpeg12.o mpegaudiodec.o pcm.o simple_idct.o \
      ratecontrol.o adpcm.o eval.o dv.o error_resilience.o \
      fft.o mdct.o mace.o huffyuv.o cyuv.o opts.o raw.o h264.o golomb.o \
      vp3.o asv1.o 4xm.o cabac.o ffv1.o ra144.o ra288.o vcr1.o cljr.o \
      roqvideo.o dpcm.o interplayvideo.o xan.o

ifeq ($(AMR_NB),yes)
ifeq ($(AMR_NB_FIXED),yes)
OBJS+= amr.o
AMREXTRALIBS+= amr/*.o
AMRLIBS=amrlibs
CLEANAMR=cleanamr
else
OBJS+= amr.o amr_float/sp_dec.o amr_float/sp_enc.o amr_float/interf_dec.o amr_float/interf_enc.o
CLEANAMR=cleanamrfloat
endif
endif

ASM_OBJS=

# codecs which are patented in some non free countries like the us
ifeq ($(CONFIG_RISKY),yes)
OBJS+= h263.o msmpeg4.o h263dec.o svq1.o rv10.o wmadec.o indeo3.o
endif

ifeq ($(HAVE_XVMC_ACCEL),yes)
OBJS+= xvmcvideo.o
endif

# currently using liba52 for ac3 decoding
ifeq ($(CONFIG_AC3),yes)
OBJS+= a52dec.o

# using builtin liba52 or runtime linked liba52.so.0
ifneq ($(CONFIG_A52BIN),yes)
OBJS+= liba52/bit_allocate.o liba52/bitstream.o liba52/downmix.o \
	liba52/imdct.o  liba52/parse.o liba52/crc.o liba52/resample.o
endif
endif

ifeq ($(CONFIG_FAAD),yes)
OBJS+= faad.o
ifeq ($(CONFIG_FAADBIN),yes)
# no libs needed
else
EXTRALIBS += -lfaad
endif
endif

ifeq ($(CONFIG_PP),yes)
ifeq ($(SHARED_PP),yes)
EXTRALIBS += -lpostproc
else
# LIBS += libpostproc/libpostproc.a ... should be fixed
OBJS += libpostproc/postprocess.o
endif
endif

ifeq ($(CONFIG_MP3LAME),yes)
OBJS += mp3lameaudio.o
EXTRALIBS += -lmp3lame
endif

ifeq ($(CONFIG_VORBIS),yes)
OBJS += oggvorbis.o
EXTRALIBS += -lvorbis -lvorbisenc
endif

ifeq ($(TARGET_GPROF),yes)
CFLAGS+=-p
LDFLAGS+=-p
endif

# i386 mmx specific stuff
ifeq ($(TARGET_MMX),yes)
OBJS += i386/fdct_mmx.o i386/cputest.o \
	i386/dsputil_mmx.o i386/mpegvideo_mmx.o \
	i386/idct_mmx.o i386/motion_est_mmx.o \
	i386/simple_idct_mmx.o i386/fft_sse.o
ifdef TARGET_BUILTIN_VECTOR
i386/fft_sse.o: CFLAGS+= -msse
endif
endif

# armv4l specific stuff
ifeq ($(TARGET_ARCH_ARMV4L),yes)
ASM_OBJS += armv4l/jrevdct_arm.o armv4l/simple_idct_arm.o
OBJS += armv4l/dsputil_arm.o armv4l/mpegvideo_arm.o
endif

# sun mediaLib specific stuff
# currently only works when libavcodec is used in mplayer
ifeq ($(HAVE_MLIB),yes)
OBJS += mlib/dsputil_mlib.o
CFLAGS += $(MLIB_INC)
endif

SRCS := $(OBJS:.o=.c) $(ASM_OBJS:.o=.S)
OBJS := $(OBJS) $(ASM_OBJS)

LIB= $(LIBPREF)avcodec$(LIBSUF)
ifeq ($(BUILD_SHARED),yes)
SLIB= $(SLIBPREF)avcodec$(SLIBSUF)
endif
TESTS= imgresample-test dct-test motion-test fft-test

all: $(LIB) $(SLIB)

amrlibs:
	$(MAKE) -C amr spclib fipoplib

tests: apiexample cpuid_test $(TESTS)

$(LIB): $(OBJS) $(AMRLIBS)
	rm -f $@
	$(AR) rc $@ $(OBJS) $(AMREXTRALIBS)
	$(RANLIB) $@

$(SLIB): $(OBJS)
ifeq ($(CONFIG_WIN32),yes)
	$(CC) $(SHFLAGS) -Wl,--output-def,$(@:.dll=.def) -o $@ $(OBJS) $(EXTRALIBS) $(AMREXTRALIBS)
	-lib /machine:i386 /def:$(@:.dll=.def)
else
	$(CC) $(SHFLAGS) -o $@ $(OBJS) $(EXTRALIBS) $(AMREXTRALIBS)
endif

dsputil.o: dsputil.c dsputil.h

libpostproc/libpostproc.a:
	$(MAKE) -C libpostproc

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $^ 1>.depend

dep:	depend

clean: $(CLEANAMR)
	rm -f *.o *.d *~ .depend $(LIB) $(SLIB) *.so i386/*.o i386/*~ \
	   armv4l/*.o armv4l/*~ \
	   mlib/*.o mlib/*~ \
	   alpha/*.o alpha/*~ \
	   ppc/*.o ppc/*~ \
	   ps2/*.o ps2/*~ \
	   sh4/*.o sh4/*~ \
	   liba52/*.o liba52/*~ \
	   apiexample $(TESTS)
	$(MAKE) -C libpostproc clean

distclean: clean
	rm -f Makefile.bak .depend

cleanall: clean
	rm -f Makefile.bak .depend

cleanamr:
	$(MAKE) -C amr clean

cleanamrfloat:
	rm -f amr_float/*.o

# api example program
apiexample: apiexample.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $< $(LIB) $(EXTRALIBS) -lm

# cpuid test
cpuid_test: i386/cputest.c
	$(CC) $(CFLAGS) -D__TEST__ -o $@ $<

# testing progs

imgresample-test: imgresample.c
	$(CC) $(CFLAGS) -DTEST -o $@ $^ -lm

dct-test: dct-test.o fdctref.o $(LIB)
	$(CC) -o $@ $^ -lm

motion-test: motion_test.o $(LIB)
	$(CC) -o $@ $^ -lm

fft-test: fft-test.o $(LIB)
	$(CC) -o $@ $^ -lm

install:


#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif