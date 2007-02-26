/* vi: set sw=4 ts=4: */
/*
 * sethostname() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>
_syscall2(int, sethostname, const char *, name, size_t, len);