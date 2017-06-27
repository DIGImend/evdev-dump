Evdev-dump
==========

Evdev-dump is a very simple utility for dumping input event device streams.

There exists a similar tool called
[`evtest`](http://cgit.freedesktop.org/~whot/evtest/). Compared to evdev-dump
it can also list device names and dump device capabilities, but can't dump more
than one device at once. It is found in many distributions as a separate
package with the same name.

Usage
-----
Here is the output of evdev-dump --help:

    $ evdev-dump --help
    Usage: evdev-dump [OPTION]... <device> [device]...
    Dump event device(s).

    Arguments:
      device           event device path, e.g. /dev/input/event1

    Options:
      -h, --help       this help message
      -p, --paused     start with the output paused
      -f, --feedback   enable feedback: for every event dumped
                       a dot is printed to stderr
      -g, --grab       grab the input device(s) with ioctl

    Signals:
      USR1/USR2        pause/resume the output

An example of the output:

    $ sudo evdev-dump /dev/input/event*
    /dev/input/event8  1280238140.003969 EV_MSC MSC_SCAN 0x0000001E
    /dev/input/event8  1280238140.003995 EV_KEY KEY_A 0x00000001
    /dev/input/event8  1280238140.004001 EV_SYN SYN_REPORT 0x00000000
    /dev/input/event8  1280238140.063443 EV_MSC MSC_SCAN 0x0000001E
    /dev/input/event8  1280238140.063473 EV_KEY KEY_A 0x00000000
    /dev/input/event8  1280238140.063481 EV_SYN SYN_REPORT 0x00000000
    /dev/input/event12 1280238150.525246 EV_MSC MSC_SCAN 0x00090001
    /dev/input/event12 1280238150.525262 EV_KEY BTN_MOUSE 0x00000001
    /dev/input/event12 1280238150.525294 EV_SYN SYN_REPORT 0x00000000
    /dev/input/event12 1280238150.693247 EV_MSC MSC_SCAN 0x00090001
    /dev/input/event12 1280238150.693267 EV_KEY BTN_MOUSE 0x00000000
    /dev/input/event12 1280238150.693307 EV_SYN SYN_REPORT 0x00000000
    ^C
