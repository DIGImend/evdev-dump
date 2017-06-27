# Copyright (c) 2009-2010 Nikolai Kondrashov
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
AC_INIT(evdev-dump, 1.0)
AC_CONFIG_AUX_DIR([auxdir])
AM_INIT_AUTOMAKE([1.9 -Wall foreign])
AC_CONFIG_SRCDIR([evdev-dump.c])
AC_CONFIG_HEADER([config.h])

# To have empty CFLAGS instead of undefined and '-g -O2' by default
CFLAGS=$CFLAGS
CFLAGS="-Os -Wall -Wextra -Werror $CFLAGS"
CPPFLAGS="-D_GNU_SOURCE -DNDEBUG $CPPFLAGS"

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_PROG(BASH, bash)
if test -z "$BASH"; then
    AC_MSG_ERROR([bash not found])
fi
AC_PATH_PROG(GAWK, gawk)
if test -z "$GAWK"; then
    AC_MSG_ERROR([gawk not found])
fi

# Checks for libraries.

# Checks for features
AC_ARG_ENABLE(
    debug,
    AS_HELP_STRING([--enable-debug], [enable debugging features]),
    [], [enable_debug="no"])

# Output features to preprocessor and compiler
if test "$enable_debug" = "yes"; then
    CPPFLAGS="$CPPFLAGS -UNDEBUG"
    CFLAGS="$CFLAGS -g -O0"
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([linux/input.h])

# Check for input files.
AC_CHECK_FILE([/usr/include/linux/input-event-codes.h],
              [INPUT_H_PATH="/usr/include/linux/input-event-codes.h"], \
              [AC_MSG_ERROR([/usr/include/linux/input-event-codes.h not found])])
AC_SUBST([INPUT_H_PATH])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
