# Copyright (c) 2009 Nikolai Kondrashov
#
# This file is part of evdev-dump.
#
# Evdev-dump is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Evdev-dump is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with evdev-dump; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(evdev-dump, 0.2)
AC_CONFIG_AUX_DIR([auxdir])
AM_INIT_AUTOMAKE([1.9 -Wall foreign])
AC_CONFIG_SRCDIR([evdev-dump.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_CHECK_PROG(BASH, bash, `which bash`, /bin/bash)

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h unistd.h linux/input.h])

# Check for input files.
AC_CHECK_FILE([/usr/include/linux/input.h],
              [INPUT_H_PATH="/usr/include/linux/input.h"], \
              [AC_MSG_ERROR([We need /usr/include/linux/input.h!])])
AC_SUBST([INPUT_H_PATH])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
