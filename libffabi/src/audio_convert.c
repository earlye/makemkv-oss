/*
 * Copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
 * Copyright (c) 2012 Justin Ruggles <justin.ruggles@gmail.com>
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
#include <lgpl/ffabi.h>

#include <libavutil/common.h>
#include "internal.h"


enum ConvFuncType {
    CONV_FUNC_TYPE_FLAT,
    CONV_FUNC_TYPE_INTERLEAVE,
    CONV_FUNC_TYPE_DEINTERLEAVE,
};

typedef void (conv_func_flat)(uint8_t *out, const uint8_t *in, int len);

typedef void (conv_func_interleave)(uint8_t *out, const uint8_t *const *in,
                                    int len, int channels);

typedef void (conv_func_deinterleave)(uint8_t **out, const uint8_t *in, int len,
                                      int channels);

struct _FFM_AudioConvert {
    enum AVSampleFormat in_fmt;
    enum AVSampleFormat out_fmt;
    int channels;
    int planes;
    int in_planar;
    enum ConvFuncType func_type;
    conv_func_flat         *conv_flat;
    conv_func_interleave   *conv_interleave;
    conv_func_deinterleave *conv_deinterleave;
};

static inline enum AVSampleFormat ac_get_packed_sample_fmt(enum AVSampleFormat fmt)
{
    if (fmt == AV_SAMPLE_FMT_S24) return AV_SAMPLE_FMT_S24;

#if (LIBAVCODEC_VERSION_MAJOR == 53)
    if ( (fmt>=AV_SAMPLE_FMT_U8P) && (fmt<=AV_SAMPLE_FMT_DBLP) ) {
        return (fmt-5);
    }
    return fmt;
#else
    return av_get_packed_sample_fmt(fmt);
#endif
}

static void ff_audio_convert_set_func(FFM_AudioConvert *ac, enum AVSampleFormat out_fmt,
                               enum AVSampleFormat in_fmt, int channels,
                               int ptr_align, int samples_align,
                               const char *descr, void *conv)
{
    switch (ac->func_type) {
    case CONV_FUNC_TYPE_FLAT:
        if (ac_get_packed_sample_fmt(ac->in_fmt)  == in_fmt &&
            ac_get_packed_sample_fmt(ac->out_fmt) == out_fmt) {
            ac->conv_flat     = conv;
        }
        break;
    case CONV_FUNC_TYPE_INTERLEAVE:
        if (ac->in_fmt == in_fmt && ac->out_fmt == out_fmt &&
            (!channels || ac->channels == channels)) {
            ac->conv_interleave = conv;
        }
        break;
    case CONV_FUNC_TYPE_DEINTERLEAVE:
        if (ac->in_fmt == in_fmt && ac->out_fmt == out_fmt &&
            (!channels || ac->channels == channels)) {
            ac->conv_deinterleave = conv;
        }
        break;
    }
}

#define ac_sizeof(type) AC_SIZE_OF_##type

#define AC_SIZE_OF_uint8_t              1
#define AC_SIZE_OF_int16_t              2
#define AC_SIZE_OF_act24_t              3
#define AC_SIZE_OF_int32_t              4
#define AC_SIZE_OF_float                sizeof(float)
#define AC_SIZE_OF_double               sizeof(double)

static av_always_inline void ac_put_uint8_t(uint8_t* dst,uint8_t value)
{
    *dst = value;
}

static av_always_inline void ac_put_int16_t(uint8_t* dst,int16_t value)
{
    *(int16_t*)dst = value;
}

static av_always_inline void ac_put_int32_t(uint8_t* dst,int32_t value)
{
    *(int32_t*)dst = value;
}

static av_always_inline void ac_put_float(uint8_t* dst,float value)
{
    *(float*)dst = value;
}

static av_always_inline void ac_put_double(uint8_t* dst,double value)
{
    *(double*)dst = value;
}

static av_always_inline void ac_put_act24_t(uint8_t* dst,int32_t value)
{
    dst[0] = (uint32_t)(value>>8);
    dst[1] = (uint32_t)(value>>16);
    dst[2] = (uint32_t)(value>>24);
}

static av_always_inline int32_t ac_get_act24(const uint8_t* src)
{
    int32_t value;

    value  = ((int32_t)(src[0])) << 8;
    value |= ((int32_t)(src[1])) << 16;
    value |= ((int32_t)(src[2])) << 24;

    return value;
}

#define CONV_FUNC_NAME(dst_fmt, src_fmt) conv_ ## src_fmt ## _to_ ## dst_fmt

#define CONV_LOOP(otype, expr)                                              \
    do {                                                                    \
        ac_put_##otype (po,(expr));                                         \
        pi += is;                                                           \
        po += os;                                                           \
    } while (po < end);                                                     \

#define CONV_FUNC_FLAT(ofmt, otype, ifmt, itype, expr)                      \
static void CONV_FUNC_NAME(ofmt, ifmt)(uint8_t *out, const uint8_t *in,     \
                                       int len)                             \
{                                                                           \
    int is       = ac_sizeof(itype);                                        \
    int os       = ac_sizeof(otype);                                        \
    const uint8_t *pi = in;                                                 \
    uint8_t       *po = out;                                                \
    uint8_t *end = out + os * len;                                          \
    CONV_LOOP(otype, expr)                                                  \
}

#define CONV_FUNC_INTERLEAVE(ofmt, otype, ifmt, itype, expr)                \
static void CONV_FUNC_NAME(ofmt, ifmt)(uint8_t *out, const uint8_t **in,    \
                                       int len, int channels)               \
{                                                                           \
    int ch;                                                                 \
    int out_bps = ac_sizeof(otype);                                         \
    int is      = ac_sizeof(itype);                                         \
    int os      = channels * out_bps;                                       \
    for (ch = 0; ch < channels; ch++) {                                     \
        const uint8_t *pi = in[ch];                                         \
        uint8_t       *po = out + ch * out_bps;                             \
        uint8_t      *end = po + os * len;                                  \
        CONV_LOOP(otype, expr)                                              \
    }                                                                       \
}

#define CONV_FUNC_DEINTERLEAVE(ofmt, otype, ifmt, itype, expr)              \
static void CONV_FUNC_NAME(ofmt, ifmt)(uint8_t **out, const uint8_t *in,    \
                                       int len, int channels)               \
{                                                                           \
    int ch;                                                                 \
    int in_bps = ac_sizeof(itype);                                          \
    int is     = channels * in_bps;                                         \
    int os     = ac_sizeof(otype);                                          \
    for (ch = 0; ch < channels; ch++) {                                     \
        const uint8_t *pi = in  + ch * in_bps;                              \
        uint8_t       *po = out[ch];                                        \
        uint8_t      *end = po + os * len;                                  \
        CONV_LOOP(otype, expr)                                              \
    }                                                                       \
}

#define CONV_FUNC_GROUP_ALL(ofmt, otype, ifmt, itype, expr) \
CONV_FUNC_FLAT(        ofmt,      otype, ifmt,      itype, expr) \
CONV_FUNC_INTERLEAVE(  ofmt,      otype, ifmt ## P, itype, expr) \
CONV_FUNC_DEINTERLEAVE(ofmt ## P, otype, ifmt,      itype, expr)

#define CONV_FUNC_GROUP_ID(ofmt, otype, ifmt, itype, expr) \
CONV_FUNC_INTERLEAVE(  ofmt,      otype, ifmt ## P, itype, expr) \
CONV_FUNC_DEINTERLEAVE(ofmt ## P, otype, ifmt,      itype, expr)

#define CONV_FUNC_GROUP_FI(ofmt, otype, ifmt, itype, expr) \
CONV_FUNC_FLAT(        ofmt,      otype, ifmt,      itype, expr) \
CONV_FUNC_INTERLEAVE(  ofmt,      otype, ifmt ## P, itype, expr) \

#define CONV_FUNC_GROUP_FD(ofmt, otype, ifmt, itype, expr) \
CONV_FUNC_FLAT(        ofmt,      otype, ifmt,      itype, expr) \
CONV_FUNC_DEINTERLEAVE(ofmt ## P, otype, ifmt,      itype, expr)

CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_U8,  uint8_t, AV_SAMPLE_FMT_U8,  uint8_t,  *(const uint8_t *)pi)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_U8,  uint8_t, (*(const uint8_t *)pi - 0x80) <<  8)
CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, act24_t, AV_SAMPLE_FMT_U8,  uint8_t, (*(const uint8_t *)pi - 0x80) << 24)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_U8,  uint8_t, (*(const uint8_t *)pi - 0x80) << 24)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, float,   AV_SAMPLE_FMT_U8,  uint8_t, (*(const uint8_t *)pi - 0x80) * (1.0f / (1 << 7)))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, double,  AV_SAMPLE_FMT_U8,  uint8_t, (*(const uint8_t *)pi - 0x80) * (1.0  / (1 << 7)))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  uint8_t, AV_SAMPLE_FMT_S16, int16_t, (*(const int16_t *)pi >> 8) + 0x80)
CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_S16, int16_t,  *(const int16_t *)pi)
CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, act24_t, AV_SAMPLE_FMT_S16, int16_t,  *(const int16_t *)pi << 16)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_S16, int16_t,  *(const int16_t *)pi << 16)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, float,   AV_SAMPLE_FMT_S16, int16_t,  *(const int16_t *)pi * (1.0f / (1 << 15)))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, double,  AV_SAMPLE_FMT_S16, int16_t,  *(const int16_t *)pi * (1.0  / (1 << 15)))
CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_U8,  uint8_t, AV_SAMPLE_FMT_S24, act24_t, (ac_get_act24(pi) >> 24) + 0x80)
CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_S24, act24_t,  ac_get_act24(pi) >> 16)
CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_S24, act24_t,  ac_get_act24(pi))
CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_FLT, float,   AV_SAMPLE_FMT_S24, act24_t,  ac_get_act24(pi) * (1.0f / (1U << 31)))
CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_DBL, double,  AV_SAMPLE_FMT_S24, act24_t,  ac_get_act24(pi) * (1.0  / (1U << 31)))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  uint8_t, AV_SAMPLE_FMT_S32, int32_t, (*(const int32_t *)pi >> 24) + 0x80)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_S32, int32_t,  *(const int32_t *)pi >> 16)
CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, act24_t, AV_SAMPLE_FMT_S32, int32_t,  *(const int32_t *)pi)
CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_S32, int32_t,  *(const int32_t *)pi)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, float,   AV_SAMPLE_FMT_S32, int32_t,  *(const int32_t *)pi * (1.0f / (1U << 31)))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, double,  AV_SAMPLE_FMT_S32, int32_t,  *(const int32_t *)pi * (1.0  / (1U << 31)))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  uint8_t, AV_SAMPLE_FMT_FLT, float,   av_clip_uint8(  lrintf(*(const float *)pi * (1  <<  7)) + 0x80))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_FLT, float,   av_clip_int16(  lrintf(*(const float *)pi * (1  << 15))))
CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, act24_t, AV_SAMPLE_FMT_FLT, float,   av_clipl_int32(llrintf(*(const float *)pi * (1U << 31))))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_FLT, float,   av_clipl_int32(llrintf(*(const float *)pi * (1U << 31))))
CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_FLT, float,   AV_SAMPLE_FMT_FLT, float,   *(const float *)pi)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, double,  AV_SAMPLE_FMT_FLT, float,   *(const float *)pi)
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  uint8_t, AV_SAMPLE_FMT_DBL, double,  av_clip_uint8(  lrint(*(const double *)pi * (1  <<  7)) + 0x80))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_DBL, double,  av_clip_int16(  lrint(*(const double *)pi * (1  << 15))))
CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, act24_t, AV_SAMPLE_FMT_DBL, double,  av_clipl_int32(llrint(*(const double *)pi * (1U << 31))))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_DBL, double,  av_clipl_int32(llrint(*(const double *)pi * (1U << 31))))
CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, float,   AV_SAMPLE_FMT_DBL, double,  *(const double *)pi)
CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_DBL, double,  AV_SAMPLE_FMT_DBL, double,  *(const double *)pi)

#define SET_CONV_FUNC_GROUP_ALL(ofmt, ifmt)                                                         \
ff_audio_convert_set_func(ac, ofmt,      ifmt,      0, 1, 1, "C", CONV_FUNC_NAME(ofmt,      ifmt)); \
ff_audio_convert_set_func(ac, ofmt ## P, ifmt,      0, 1, 1, "C", CONV_FUNC_NAME(ofmt ## P, ifmt)); \
ff_audio_convert_set_func(ac, ofmt,      ifmt ## P, 0, 1, 1, "C", CONV_FUNC_NAME(ofmt,      ifmt ## P));

#define SET_CONV_FUNC_GROUP_ID(ofmt, ifmt)                                                          \
ff_audio_convert_set_func(ac, ofmt ## P, ifmt,      0, 1, 1, "C", CONV_FUNC_NAME(ofmt ## P, ifmt)); \
ff_audio_convert_set_func(ac, ofmt,      ifmt ## P, 0, 1, 1, "C", CONV_FUNC_NAME(ofmt,      ifmt ## P));

#define SET_CONV_FUNC_GROUP_FI(ofmt, ifmt)                                                         \
ff_audio_convert_set_func(ac, ofmt,      ifmt,      0, 1, 1, "C", CONV_FUNC_NAME(ofmt,      ifmt)); \
ff_audio_convert_set_func(ac, ofmt,      ifmt ## P, 0, 1, 1, "C", CONV_FUNC_NAME(ofmt,      ifmt ## P));

#define SET_CONV_FUNC_GROUP_FD(ofmt, ifmt)                                                         \
ff_audio_convert_set_func(ac, ofmt,      ifmt,      0, 1, 1, "C", CONV_FUNC_NAME(ofmt,      ifmt)); \
ff_audio_convert_set_func(ac, ofmt ## P, ifmt,      0, 1, 1, "C", CONV_FUNC_NAME(ofmt ## P, ifmt));

static void set_generic_function(FFM_AudioConvert *ac)
{
    SET_CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_U8,  AV_SAMPLE_FMT_U8)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_U8)
    SET_CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, AV_SAMPLE_FMT_U8)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_U8)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_U8)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_U8)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  AV_SAMPLE_FMT_S16)
    SET_CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16)
    SET_CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, AV_SAMPLE_FMT_S16)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S16)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_S16)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_S16)
    SET_CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_U8,  AV_SAMPLE_FMT_S24)
    SET_CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S24)
    SET_CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S24)
    SET_CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_S24)
    SET_CONV_FUNC_GROUP_FD (AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_S24)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  AV_SAMPLE_FMT_S32)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S32)
    SET_CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, AV_SAMPLE_FMT_S32)
    SET_CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_S32)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_S32)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  AV_SAMPLE_FMT_FLT)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_FLT)
    SET_CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, AV_SAMPLE_FMT_FLT)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_FLT)
    SET_CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLT)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_FLT)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_U8,  AV_SAMPLE_FMT_DBL)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_DBL)
    SET_CONV_FUNC_GROUP_FI (AV_SAMPLE_FMT_S24, AV_SAMPLE_FMT_DBL)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_DBL)
    SET_CONV_FUNC_GROUP_ALL(AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_DBL)
    SET_CONV_FUNC_GROUP_ID (AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBL)
}

void __cdecl ffm_audio_convert_free(FFM_AudioConvert **ac)
{
    if (!*ac)
        return;
    av_freep(ac);
}

FFM_AudioConvert *ff_ffm_audio_convert_alloc(
                                     enum AVSampleFormat out_fmt,
                                     enum AVSampleFormat in_fmt,
                                     int channels)
{
    FFM_AudioConvert *ac;
    int in_planar, out_planar;

    ac = av_mallocz(sizeof(*ac));
    if (!ac)
        return NULL;

    ac->out_fmt  = out_fmt;
    ac->in_fmt   = in_fmt;
    ac->channels = channels;

    in_planar  = av_sample_fmt_is_planar(in_fmt);
    ac->in_planar = in_planar;
    out_planar = av_sample_fmt_is_planar(out_fmt);

    if (in_planar == out_planar) {
        ac->func_type = CONV_FUNC_TYPE_FLAT;
        ac->planes    = in_planar ? ac->channels : 1;
    } else if (in_planar)
        ac->func_type = CONV_FUNC_TYPE_INTERLEAVE;
    else
        ac->func_type = CONV_FUNC_TYPE_DEINTERLEAVE;

    set_generic_function(ac);

    return ac;
}

void __cdecl ffm_audio_convert(FFM_AudioConvert *ac, uint8_t *out_data[], const uint8_t* in_data[], int nb_samples)
{
    int len         = nb_samples;
    int p;

    switch (ac->func_type) {
    case CONV_FUNC_TYPE_FLAT: {
        if (!ac->in_planar)
            len *= ac->channels;
        for (p = 0; p < ac->planes; p++)
            ac->conv_flat(out_data[p], in_data[p], len);
        break;
    }
    case CONV_FUNC_TYPE_INTERLEAVE:
        ac->conv_interleave(out_data[0], in_data, len, ac->channels);
        break;
    case CONV_FUNC_TYPE_DEINTERLEAVE:
        ac->conv_deinterleave(out_data, in_data[0], len, ac->channels);
        break;
    }
}
