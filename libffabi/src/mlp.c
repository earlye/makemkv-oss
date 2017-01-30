/*
 * MLP codec common code
 * Copyright (c) 2007-2008 Ian Caulfield
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

#include <stdint.h>

#include <libavutil/crc.h>
#include <libavutil/intreadwrite.h>
#include <lgpl/ffabi.h>
#include "internal.h"

#define CRC_TABLE_SIZE 1024

static AVCRC crc_2D[CRC_TABLE_SIZE];

av_cold void ffabi_ff_mlp_init_crc(void)
{
    av_crc_init(crc_2D, 0, 16, 0x002D, sizeof(crc_2D));
}

uint16_t ffm_mlp_checksum16(const uint8_t *buf, unsigned int buf_size)
{
    uint16_t crc;

    crc = av_crc(crc_2D, 0, buf, buf_size - 2);
    crc ^= AV_RL16(buf + buf_size - 2);
    return crc;
}

