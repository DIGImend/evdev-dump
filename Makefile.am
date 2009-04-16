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

bin_PROGRAMS = evdev-dump

evdev_dump_SOURCES = evdev-dump.c
nodist_evdev_dump_SOURCES = event2str.inc
BUILT_SOURCES = event2str.inc
CLEANFILES = event2str.inc

event2str.inc: gen_event2str_inc.sh @INPUT_H_PATH@
	@BASH@ $^ > $@
