/*
    libMakeMKV - MKV multiplexer library

    Copyright (C) 2007-2019 GuinpinSoft inc <libmkv@makemkv.com>

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
#ifndef LGPL_FFABI_H_INCLUDED
#define LGPL_FFABI_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#if !defined(_MSC_VER) && !defined(__cdecl)
#define __cdecl
#endif

#define FFABI_VERSION_FLAG_PATCHLEVEL_MASK  0x0f
#define FFABI_VERSION_FLAG_PATCHLEVEL_VALUE 1

#define FFM_INPUT_BUFFER_PADDING_SIZE 16
#define FFM_PROFILE_UNKNOWN 0xffffffff
#define FFM_CODEC_FLAG_GLOBAL_HEADER 1

typedef enum _FFM_AudioFormat {
    FFM_AUDIO_FMT_UNKNOWN=0,
    FFM_AUDIO_FMT_PCM_U8,
    FFM_AUDIO_FMT_PCM_S16,
    FFM_AUDIO_FMT_PCM_S32,
    FFM_AUDIO_FMT_PCM_FLT,
    FFM_AUDIO_FMT_PCM_DBL,
    FFM_AUDIO_FMT_PCM_U8P,
    FFM_AUDIO_FMT_PCM_S16P,
    FFM_AUDIO_FMT_PCM_S32P,
    FFM_AUDIO_FMT_PCM_FLTP,
    FFM_AUDIO_FMT_PCM_DBLP,
    FFM_AUDIO_FMT_PCM_S24,
    FFM_AUDIO_FMT_MAX_VALUE
} FFM_AudioFormat;

typedef enum _FFM_MatrixEncoding {
    FFM_MATRIX_ENCODING_NONE=0,
    FFM_MATRIX_ENCODING_DOLBY,
    FFM_MATRIX_ENCODING_DPLII,
    FFM_MATRIX_ENCODING_NB
} FFM_MatrixEncoding;

#ifdef _MSC_VER
#define ALIGN_PACKED
#pragma pack(1)
#endif

#ifdef __GNUC__
#define ALIGN_PACKED __attribute((packed))
#endif

typedef struct _FFM_AudioInfo {
    uint32_t    sample_rate;
    uint32_t    channels;
    uint16_t    sample_fmt;
    uint16_t    bits_per_sample;
    uint32_t    frame_size;
    uint64_t    channel_layout;
    uint32_t    profile;
} ALIGN_PACKED FFM_AudioInfo;

typedef struct _FFM_CodecInfo {
    const char*     name;
    const char*     long_name;
    const int*      sample_rates;
    const uint64_t* channel_layouts;
    uint32_t        id;
    uint32_t        capabilities;
    uint32_t        profiles_count;
    uint32_t        sample_formats_count;
    uint32_t        profiles_values[32];
    const char*     profiles_names[32];
    uint8_t         sample_formats[16];
} ALIGN_PACKED FFM_CodecInfo;

typedef struct _FFM_AudioEncodeInfo {
    const uint8_t*  extradata;
    uint32_t        extradata_size;
    uint32_t        delay;
    uint32_t        flags;
} ALIGN_PACKED FFM_AudioEncodeInfo;

#ifdef _MSC_VER
#pragma pack()
#endif

struct _FFM_AudioDecodeContext;
typedef struct _FFM_AudioDecodeContext FFM_AudioDecodeContext;

struct _FFM_AudioEncodeContext;
typedef struct _FFM_AudioEncodeContext FFM_AudioEncodeContext;

struct _FFM_AudioConvert;
typedef struct _FFM_AudioConvert FFM_AudioConvert;

struct _FFM_AudioMix;
typedef struct _FFM_AudioMix FFM_AudioMix;

struct _FFM_DcaDec;
typedef struct _FFM_DcaDec FFM_DcaDec;

typedef void  (__cdecl *ffm_log_callback_t)(void* ctx,void* ctx2,int level,char* text);
typedef void* (__cdecl *ffm_memalign_t)(uintptr_t align, uintptr_t size);
typedef void* (__cdecl *ffm_realloc_t)(void *ptr, uintptr_t size);
typedef void  (__cdecl *ffm_free_t)(void *ptr);

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl ffm_init(ffm_log_callback_t log_proc,void* log_context,ffm_memalign_t memalign_proc,ffm_realloc_t realloc_proc,ffm_free_t free_proc);
uint64_t __cdecl ffm_avcodec_version3(void);

FFM_AudioDecodeContext* __cdecl ffm_audio_decode_init(void* logctx,const char* name,FFM_AudioFormat fmt,const char* argp[],const uint8_t* CodecData,unsigned int CodecDataSize,unsigned int time_base,unsigned int CodecFlags);
int __cdecl ffm_audio_decode_close(FFM_AudioDecodeContext* ctx);
int __cdecl ffm_audio_decode_put_data(FFM_AudioDecodeContext* ctx,const uint8_t* data,unsigned int size,int64_t pts);
int __cdecl ffm_audio_decode_get_frame(FFM_AudioDecodeContext* ctx,int64_t* pts,const uint8_t* data[]);
int __cdecl ffm_audio_decode_get_info(FFM_AudioDecodeContext* ctx,FFM_AudioInfo* info);

FFM_AudioEncodeContext* __cdecl ffm_audio_encode_init(void* logctx,const char* name,FFM_AudioFormat fmt,FFM_AudioInfo* info,const char* argp[],unsigned int time_base,unsigned int CodecFlags);
int __cdecl ffm_audio_encode_close(FFM_AudioEncodeContext* ctx);
int __cdecl ffm_audio_encode_put_frame(FFM_AudioEncodeContext* ctx,const uint8_t* frame_data[],unsigned int frame_size,unsigned int nb_samples,uint64_t pts);
int __cdecl ffm_audio_encode_get_data(FFM_AudioEncodeContext* ctx,unsigned int *size,int64_t *pts,const uint8_t** data);
int __cdecl ffm_audio_encode_get_info(FFM_AudioEncodeContext* ctx,FFM_AudioEncodeInfo* info);

int __cdecl ffm_audio_get_codec_information(FFM_CodecInfo* info,const char* name,int encode);

FFM_AudioConvert* __cdecl ffm_audio_convert_alloc(FFM_AudioFormat out_fmt,FFM_AudioFormat in_fmt,int channels);
void __cdecl ffm_audio_convert_free(FFM_AudioConvert **ac);
void __cdecl ffm_audio_convert(FFM_AudioConvert *ac, uint8_t *data_out[], const uint8_t* data_in[], int nb_samples);

FFM_AudioMix* __cdecl ffm_audio_mix_alloc(void* logctx,uint64_t in_channel_layout, uint64_t out_channel_layout, const double* mix_levels, FFM_MatrixEncoding matrix_encoding);
void __cdecl ffm_audio_mix_free(FFM_AudioMix **am_p);
int __cdecl ffm_audio_mix(FFM_AudioMix *am, int32_t *src_data[], int nb_samples);

int __cdecl ffm_mlp_read_syncframe(const uint8_t* data,unsigned int size,FFM_AudioInfo* info,uint32_t* bitrate);
uint16_t __cdecl ffm_mlp_checksum16(const uint8_t *buf, unsigned int buf_size);

int __cdecl ffm_mpa_decode_header(uint32_t hdr,FFM_AudioInfo* info,uint32_t* layer,uint32_t* frame_size,uint32_t* bitrate);

int __cdecl ffm_get_channel_layout_string(char *buf,int buf_size,uint64_t channel_layout);

#ifdef __cplusplus
};
#endif

#endif // LGPL_FFABI_H_INCLUDED

