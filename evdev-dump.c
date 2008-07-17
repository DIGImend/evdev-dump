/*
 * Copyright (c) 2005-2008 Nikolai Kondrashov
 *
 * This file is part of digimend-diag.
 *
 * Digimend-diag is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Digimend-diag is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with digimend-diag; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>


/* Taken from linux/time.h */
#define NSEC_PER_SEC	1000000000L
#define NSEC_PER_USEC	1000L
static inline long long
timeval_to_ns(const struct timeval *tv)
{
	return ((long long) tv->tv_sec * NSEC_PER_SEC) +
		tv->tv_usec * NSEC_PER_USEC;
}


/** Maximum number of input events read in one read call. */
#define MAX_EVENTS 64

typedef struct input_event input_event;

typedef struct input {
    int             fd;                 /**< File descriptor */
    input_event     buf[MAX_EVENTS];    /**< Event buffer */
    size_t          num;                /**< Number of events in
                                             the buffer */
    input_event    *e;                  /**< Next event pointer */
} input;


/** Open an input device. */
input *
open_input(const char *filename)
{
    int     fd;
    input  *i;

    if ((fd = open(filename, O_RDONLY)) < 0)
        return NULL;

    i = malloc(sizeof(*i));
    if (i == NULL)
        return NULL;

    i->fd   = fd;
    i->num  = 0;
    i->e    = i->buf;

    return i;
}


/** Close an input device. */
void
close_input(input *i)
{
    close(i->fd);
    free(i);
}


/** Read event from an input device. */
int
read_input(input *i, const input_event **pe)
{
    int rc;

    i->e++;

    if ((size_t)(i->e - i->buf) >= i->num)
    {
        rc = read(i->fd, i->buf, sizeof(i->buf));

        if (rc < 0)
            return -1;
        if (rc == 0)
            return 0;
        if ((rc % sizeof(input_event)) != 0)
            return -1;

        i->num = rc / sizeof(input_event);

        i->e = i->buf;
    }

    if (pe != NULL)
        *pe = i->e;

    return 1;
}

/* Include generated event2str function. */
#include "event2str.c"

int
main(int argc, const char **argv)
{
    const char         *filename;
    input              *i;
    const input_event  *e;
    const char         *type_str;
    const char         *code_str;
    int                 rc;

    program_invocation_name = program_invocation_short_name;

    if (argc != 2)
    {
        fprintf(stderr,
                "Usage: %s <event device>\n",
                program_invocation_short_name);
        exit(1);
    }

    filename = argv[1];

    i = open_input(filename);
    if (i == NULL)
        error(1, errno, "Failed to open input device %s", filename);

    setlinebuf(stdout);

    while ((rc = read_input(i, &e)) > 0)
    {
        event2str(e, &type_str, &code_str);

        fprintf(stdout,
                "%.16llX %s(0x%.2X): %s(0x%.3X): 0x%.8X\n",
                (unsigned long long)timeval_to_ns(&e->time),
                type_str, e->type, code_str, e->code, e->value);
    }

    if (rc < 0)
        error(1, errno, "Failed to read input device %s", filename);

    close_input(i);

    return 0;
}


