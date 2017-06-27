/** @file
 * @brief evdev-dump - entry point
 *
 * Copyright (C) 2005-2010 Nikolai Kondrashov
 *
 * This file is part of evdev-dump.
 *
 * Evdev-dump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Evdev-dump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with evdev-dump; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @author Nikolai Kondrashov <spbnick@gmail.com>
 *
 * @(#) $Id$
 */

#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

#define ERROR(_fmt, _args...) \
    fprintf(stderr, _fmt "\n", ##_args)

#define FAILURE(_fmt, _args...) \
    fprintf(stderr, "Failed to " _fmt "\n", ##_args)

#define ERRNO_FAILURE(_fmt, _args...) \
    FAILURE(_fmt ": %s", ##_args, strerror(errno))

#define ERROR_CLEANUP(_fmt, _args...) \
    do {                                \
        ERROR(_fmt, ##_args);           \
        goto cleanup;                   \
    } while (0)

#define FAILURE_CLEANUP(_fmt, _args...) \
    do {                                \
        FAILURE(_fmt, ##_args);         \
        goto cleanup;                   \
    } while (0)

#define ERRNO_FAILURE_CLEANUP(_fmt, _args...) \
    do {                                        \
        ERRNO_FAILURE(_fmt, ##_args);           \
        goto cleanup;                           \
    } while (0)

static void
usage(FILE *stream)
{
    fprintf(
        stream,
"Usage: %s [OPTION]... <device> [device]...\n"
"Dump event device(s).\n"
"\n"
"Arguments:\n"
"  device           event device path, e.g. /dev/input/event1\n"
"\n"
"Options:\n"
"  -h, --help       this help message\n"
"  -p, --paused     start with the output paused\n"
"  -f, --feedback   enable feedback: for every event dumped\n"
"                   a dot is printed to stderr\n"
"  -g, --grab       grab the input device with ioctl\n"
"\n"
"Signals:\n"
"  USR1/USR2        pause/resume the output\n"
"\n",
        program_invocation_short_name);
}


/**< "output paused" flag - non-zero if paused */
static volatile sig_atomic_t paused = 0;

static void
pause_sighandler(int signum)
{
    (void)signum;
    paused = 1;
}

static void
resume_sighandler(int signum)
{
    (void)signum;
    paused = 0;
}


/**< "dump feedback" flag - non-zero if feedback is enabled */
static volatile sig_atomic_t feedback = 0;

static volatile sig_atomic_t grab = 0;

/* Include generated event2str function. */
#include "event2str.inc"


static void
dump(const char *path, const struct input_event *e)
{
    const char *type_str;
    const char *code_str;
    char        type_buf[7];
    char        code_buf[7];

    event2str(e, &type_str, &code_str);

    if (type_str == NULL)
    {
        snprintf(type_buf, sizeof(type_buf), "0x%.2hX", e->type);
        type_str = type_buf;
    }

    if (code_str == NULL)
    {
        snprintf(code_buf, sizeof(code_buf), "0x%.4hX", e->code);
        code_str = code_buf;
    }

    fprintf(stdout,
            "%-18s %llu.%.6u %s %s 0x%.8X\n",
            path,
            (long long unsigned int)e->time.tv_sec,
            (unsigned int)e->time.tv_usec,
            type_str, code_str,
            e->value);
    fflush(stdout);
}


static int
run(char **path_list, size_t num)
{
    static const char   dot             = '.';
    int                 result          = 1;
    int                 fd_list[num];
    int                 max_fd;
    int                 fd;
    size_t              i;
    fd_set              read_set;
    int                 rc;
    struct input_event  e;

    assert(path_list != NULL);
    assert(num > 0);
    assert(num < FD_SETSIZE);

    /* Initialize the FD list */
    for (i = 0; i < num; i++)
        fd_list[i] = -1;

    /* Open the devices */
    for (i = 0; i < num; i++)
    {
        fd = open(path_list[i], O_RDONLY);
        fd_list[i] = fd;
        if (fd < 0 || fd >= FD_SETSIZE)
        {
            if (fd >= FD_SETSIZE)
                errno = EMFILE;
            ERRNO_FAILURE_CLEANUP("open \"%s\"", path_list[i]);
        }
        if (grab) {
             ioctl(fd, EVIOCGRAB, grab);
        }
    }

    /* Dump the devices */
    while (true)
    {
        /* Fill the FD read set */
        max_fd = -1;
        FD_ZERO(&read_set);
        for (i = 0; i < num; i++)
        {
            fd = fd_list[i];
            if (fd >= 0)
            {
                FD_SET(fd, &read_set);
                if (fd > max_fd)
                    max_fd = fd;
            }
        }

        /* If there were no valid FDs */
        if (max_fd < 0)
            break;

        /* Wait for input */
        rc = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (rc <= 0)
        {
            if (errno != EINTR)
                ERRNO_FAILURE_CLEANUP("wait for input");
            continue;
        }

        /* Dump ready descriptors */
        for (i = 0; i < num; i++)
        {
            fd = fd_list[i];

            if (!FD_ISSET(fd, &read_set))
                continue;

            rc = read(fd, &e, sizeof(e));
            if (rc < 0)
                ERRNO_FAILURE_CLEANUP("read \"%s\"", path_list[i]);
            else if (rc == 0)
            {
                close(fd);
                fd_list[i] = -1;
            }
            else
            {
                if (rc != sizeof(e))
                    ERROR_CLEANUP("Short read from \"%s\"", path_list[i]);
                if (!paused)
                {
                    dump(path_list[i], &e);
                    if (feedback)
                        write(STDERR_FILENO, &dot, sizeof(dot));
                }
            }
        }
    }

    result = 0;

cleanup:

    for (i = 0; i < num; i++)
    {
        fd = fd_list[i];
        if (fd >= 0)
            close(fd);
    }

    return result;
}


typedef enum opt_val {
    OPT_VAL_HELP        = 'h',
    OPT_VAL_PAUSED      = 'p',
    OPT_VAL_FEEDBACK    = 'f',
    OPT_VAL_GRAB        = 'g',
} opt_val;


int
main(int argc, char **argv)
{
    static const struct option long_opt_list[] = {
        {.val       = OPT_VAL_HELP,
         .name      = "help",
         .has_arg   = no_argument,
         .flag      = NULL},
        {.val       = OPT_VAL_PAUSED,
         .name      = "paused",
         .has_arg   = no_argument,
         .flag      = NULL},
        {.val       = OPT_VAL_FEEDBACK,
         .name      = "feedback",
         .has_arg   = no_argument,
         .flag      = NULL},
        {.val       = OPT_VAL_GRAB,
         .name      = "grab",
         .has_arg   = no_argument,
         .flag      = NULL},
        {.val       = 0,
         .name      = NULL,
         .has_arg   = 0,
         .flag      = NULL}
    };

    static const char  *short_opt_list = "hpfg";
    int                 c;
    struct sigaction    sa;

#define USAGE_ERROR(_fmt, _args...) \
    do {                                        \
        fprintf(stderr, _fmt "\n", ##_args);    \
        usage(stderr);                          \
        return 1;                               \
    } while (0)

    /*
     * Parse command line arguments
     */
    while ((c = getopt_long(argc, argv,
                            short_opt_list, long_opt_list, NULL)) >= 0)
    {
        switch (c)
        {
            case OPT_VAL_HELP:
                usage(stdout);
                return 0;
                break;
            case OPT_VAL_PAUSED:
                paused = 1;
                break;
            case OPT_VAL_FEEDBACK:
                feedback = 1;
                break;
            case OPT_VAL_GRAB:
                grab = 1;
                break;
            case '?':
                usage(stderr);
                return 1;
                break;
        }
    }

    /*
     * Validate positional parameters
     */
    if (optind >= argc)
        USAGE_ERROR("Not enough arguments");
    if ((optind - argc) >= FD_SETSIZE)
        USAGE_ERROR("Too many arguments");

    /*
     * Setup SIGUSR1/SIGUSR2 to pause/resume the output
     */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = pause_sighandler;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler = resume_sighandler;
    sigaction(SIGUSR2, &sa, NULL);

    /* Make stdout buffered - we will flush it explicitly */
    setbuf(stdout, NULL);

    return run(argv + optind, argc - optind);
}


