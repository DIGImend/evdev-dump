# Copyright (c) 2005-2008 Nikolai Kondrashov
#
# This file is part of digimend-diag.
#
# Digimend-diag is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Digimend-diag is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with digimend-diag; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

PREFIX=/usr/local

build: evdev-dump

install: build
	install -m755 evdev-dump ${PREFIX}/bin/evdev-dump

evdev-dump: event2str.c evdev-dump.c
	gcc -Wall -Wextra -o evdev-dump evdev-dump.c

event2str.c: make_event2str_c /usr/include/linux/input.h
	bash ./make_event2str_c /usr/include/linux/input.h > event2str.c

clean:
	rm -f event2str.c evdev-dump
