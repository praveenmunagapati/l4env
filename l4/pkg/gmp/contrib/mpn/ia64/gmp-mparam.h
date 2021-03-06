/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#define BITS_PER_MP_LIMB 64
#define BYTES_PER_MP_LIMB 8

/* Generated by tuneup.c, 2002-02-12, gcc 2.96 */

#define MUL_KARATSUBA_THRESHOLD          47
#define MUL_TOOM3_THRESHOLD             288

#define SQR_BASECASE_THRESHOLD           25
#define SQR_KARATSUBA_THRESHOLD         128
#define SQR_TOOM3_THRESHOLD             250

#define DIV_SB_PREINV_THRESHOLD               0  /* (preinv always) */
#define DIV_DC_THRESHOLD                     48
#define POWM_THRESHOLD                  520

#define GCD_ACCEL_THRESHOLD               3
#define GCDEXT_THRESHOLD                 24
#define JACOBI_BASE_METHOD                2

#define DIVREM_1_NORM_THRESHOLD           0  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD         0  /* always */
#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define USE_PREINV_DIVREM_1               1  /* preinv always */
#define USE_PREINV_MOD_1                  1  /* preinv always */
#define DIVREM_2_THRESHOLD                0  /* preinv always */
#define DIVEXACT_1_THRESHOLD              0  /* always */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always */

#define GET_STR_DC_THRESHOLD       14
#define GET_STR_PRECOMPUTE_THRESHOLD     19
#define SET_STR_THRESHOLD             24003

#define MUL_FFT_TABLE  { 912, 1696, 3904, 7424, 19456, 45056, 0 }
#define MUL_FFT_MODF_THRESHOLD          944
#define MUL_FFT_THRESHOLD              7808

#define SQR_FFT_TABLE  { 848, 1696, 3648, 7424, 15360, 45056, 0 }
#define SQR_FFT_MODF_THRESHOLD          864
#define SQR_FFT_THRESHOLD              6272
