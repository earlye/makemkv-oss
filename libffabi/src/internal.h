/*
    libMakeMKV - MKV multiplexer library

    Copyright (C) 2007-2016 GuinpinSoft inc <libmkv@makemkv.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#ifndef FFABI_INTERNAL_H
#define FFABI_INTERNAL_H

#include <libavutil/samplefmt.h>
#include <libavutil/mem.h>
#include <libavutil/log.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/version.h>

static const enum AVSampleFormat AV_SAMPLE_FMT_S24 = AV_SAMPLE_FMT_NB + 1;

typedef struct _ffabi_Logger {
    const AVClass*  av_class;
    void*           ctx2;
} ffabi_Logger;


FFM_AudioConvert *ff_ffm_audio_convert_alloc(
                                     enum AVSampleFormat out_fmt,
                                     enum AVSampleFormat in_fmt,
                                     int channels);

#define FFM_AVRESAMPLE_MAX_CHANNELS 32

int ff_avresample_build_matrix(uint64_t in_layout, uint64_t out_layout,
                            double center_mix_level, double surround_mix_level,
                            double lfe_mix_level, int normalize,
                            double *matrix_out, int stride,
                            FFM_MatrixEncoding matrix_encoding);


void ffabi_ff_mlp_init_crc(void);

void *ffabi_memalign(size_t align, size_t size);
void *ffabi_realloc(void *ptr, size_t size);
void ffabi_free(void *ptr);

void dca_log(void* ctx,const char* msg,unsigned int lineno);


#if (LIBAVCODEC_VERSION_MAJOR < 53)
#error "Can't use an ancient ffmpeg or libav"
#endif

#include "ffabicfg.h"

#ifndef FFABI_HAVE_AVCODECID
#define AVCodecID CodecID
#endif

#ifndef FFABI_HAVE_AV_CODEC_ID_NAMES
#define AV_CODEC_ID_NONE CODEC_ID_NONE
#define AV_CODEC_ID_MLP CODEC_ID_MLP
#define AV_CODEC_ID_TRUEHD CODEC_ID_TRUEHD
#endif

#ifndef FFABI_HAVE_REFCOUNTED_FRAMES
static inline void av_frame_unref(AVFrame *frame)
{
}
#endif

#ifndef FFABI_HAVE_AV_LOG_FORMAT_LINE
void av_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);
#endif

#ifdef FFABI_HAVE_AVUTIL_CHANNEL_LAYOUT_H
#include <libavutil/channel_layout.h>
#endif

#endif /* FFABI_INTERNAL_H */
