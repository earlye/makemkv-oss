/*
 * log functions
 * Copyright (c) 2003 Michel Bardiaux
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * logging functions
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <libavutil/log.h>
#include "ffabicfg.h"

#ifndef FFABI_HAVE_AV_LOG_FORMAT_LINE

void av_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix)
{
    AVClass* avc = ptr ? *(AVClass **) ptr : NULL;
    line[0] = 0;
    if (*print_prefix && avc) {
        if (avc->parent_log_context_offset) {
            AVClass** parent = *(AVClass ***) (((uint8_t *) ptr) +
                                   avc->parent_log_context_offset);
            if (parent && *parent) {
                snprintf(line, line_size, "[%s @ %p] ",
                         (*parent)->item_name(parent), parent);
            }
        }
        snprintf(line + strlen(line), line_size - strlen(line), "[%s @ %p] ",
                 avc->item_name(ptr), ptr);
    }

    vsnprintf(line + strlen(line), line_size - strlen(line), fmt, vl);

    *print_prefix = strlen(line) && line[strlen(line) - 1] == '\n';
}

#endif

