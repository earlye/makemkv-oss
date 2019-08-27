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
#include <lgpl/ffabi.h>
#include <libavcodec/avcodec.h>
#include "mpegaudiodecheader.h"
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <string.h>
#include "internal.h"

static int                  log_print_prefix = 1;
static void*                log_context = NULL;
static ffm_log_callback_t   log_callback = NULL;
static ffm_memalign_t       proc_memalign = NULL;
static ffm_realloc_t        proc_realloc = NULL;
static ffm_free_t           proc_free = NULL;

void *ffabi_memalign(size_t align, size_t size)
{
    return (*proc_memalign)(align,size);
}

void *ffabi_realloc(void *ptr, size_t size)
{
    return (*proc_realloc)(ptr,size);
}

void ffabi_free(void *ptr)
{
    (*proc_free)(ptr);
}

static void static_log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    char line[1024];
    void* ctx2 = NULL;

    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &log_print_prefix);

    line[sizeof(line)-1]=0;

    if (ptr) {
        struct AVClass* pclass = *((struct AVClass**)ptr);
        if (pclass) {
            if (!strcmp(pclass->class_name,"AVCodecContext")) {
                ctx2 = ((AVCodecContext*)ptr)->opaque;
            }
            if (!strcmp(pclass->class_name,"FFM_AudioMix")) {
                ctx2 = ((ffabi_Logger*)ptr)->ctx2;
            }
        }
    }

    if (log_callback) {
        (*log_callback)(log_context,ctx2,level,line);
    }
}

#if !defined(FFABI_HAVE_ALL_CODECS) && defined(FFABI_HAVE_DECODER_AC3) && (LIBAVCODEC_VERSION_MAJOR < 58)
#define FFABI_REGISTER_ALL
extern AVCodec ff_ac3_decoder,ff_eac3_decoder,ff_aac_latm_decoder,ff_aac_fixed_decoder;
extern AVCodec ff_dca_decoder,ff_mlp_decoder,ff_truehd_decoder;
extern AVCodec ff_mp1_decoder,ff_mp2_decoder,ff_mp3_decoder;
extern AVCodec ff_flac_decoder;
extern AVCodec ff_ac3_encoder,ff_ac3_fixed_encoder,ff_aac_encoder,ff_libfdk_aac_encoder,ff_flac_encoder;
extern AVCodecParser ff_mlp_parser;

static void ffabi_register_all(void)
{
    avcodec_register(&ff_ac3_decoder);
#ifdef FFABI_HAVE_DECODER_EAC3
    avcodec_register(&ff_eac3_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_AAC_LATM
    avcodec_register(&ff_aac_latm_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_AAC_FIXED
    avcodec_register(&ff_aac_fixed_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_DCA
    avcodec_register(&ff_dca_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_MLP
    avcodec_register(&ff_mlp_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_TRUEHD
    avcodec_register(&ff_truehd_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_MP1
    avcodec_register(&ff_mp1_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_MP2
    avcodec_register(&ff_mp2_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_MP3
    avcodec_register(&ff_mp3_decoder);
#endif
#ifdef FFABI_HAVE_DECODER_FLAC
    avcodec_register(&ff_flac_decoder);
#endif

#ifdef FFABI_HAVE_ENCODER_AC3
    avcodec_register(&ff_ac3_encoder);
#endif
#ifdef FFABI_HAVE_ENCODER_AC3_FIXED
    avcodec_register(&ff_ac3_fixed_encoder);
#endif
#ifdef FFABI_HAVE_ENCODER_AAC
    avcodec_register(&ff_aac_encoder);
#endif
#ifdef FFABI_HAVE_ENCODER_LIBFDK_AAC
    avcodec_register(&ff_libfdk_aac_encoder);
#endif
#ifdef FFABI_HAVE_ENCODER_FLAC
    avcodec_register(&ff_flac_encoder);
#endif

    av_register_codec_parser(&ff_mlp_parser);
}
#endif

int __cdecl ffm_init(ffm_log_callback_t log_proc,void* log_ctx,ffm_memalign_t memalign_proc,ffm_realloc_t realloc_proc,ffm_free_t free_proc)
{
    log_context = log_ctx;
    log_callback = log_proc;
    proc_memalign = memalign_proc;
    proc_realloc = realloc_proc;
    proc_free = free_proc;

    av_log_set_callback(static_log_callback);

#ifdef FFABI_REGISTER_ALL
    ffabi_register_all();
#else
    avcodec_register_all();
#endif

    ffabi_ff_mlp_init_crc();

    return 0;
}

uint64_t __cdecl ffm_avcodec_version3(void)
{
    const uint32_t flags = FFABI_VERSION_FLAG_PATCHLEVEL_VALUE;
    uint32_t version = avcodec_version()&0x7fffffff;
    return (((uint64_t)flags)<<32) | version | 0x80000000;
}

#ifdef FFABI_AVCODEC_OLD_API

static inline AVFrame *ffm_frame_alloc(void)
{
#ifdef FFABI_HAVE_AV_FRAME_FREE
    return av_frame_alloc();
#else
    return avcodec_alloc_frame();
#endif
}

static inline void ffm_frame_free(AVFrame **frame)
{
#if defined(FFABI_HAVE_AV_FRAME_FREE)
    av_frame_free(frame);
#elif defined(FFABI_HAVE_AVCODEC_FREE_FRAME)
    avcodec_free_frame(frame);
#else
    av_freep(frame);
#endif
}

#endif

static enum AVSampleFormat translate_sample_fmt(FFM_AudioFormat fmt)
{
    enum AVSampleFormat r;

    switch(fmt) {
    case FFM_AUDIO_FMT_PCM_U8 : r = AV_SAMPLE_FMT_U8; break;
    case FFM_AUDIO_FMT_PCM_S16: r = AV_SAMPLE_FMT_S16; break;
    case FFM_AUDIO_FMT_PCM_S24: r = AV_SAMPLE_FMT_S24; break;
    case FFM_AUDIO_FMT_PCM_S32: r = AV_SAMPLE_FMT_S32; break;
    case FFM_AUDIO_FMT_PCM_FLT: r = AV_SAMPLE_FMT_FLT; break;
    case FFM_AUDIO_FMT_PCM_DBL: r = AV_SAMPLE_FMT_DBL; break;
    case FFM_AUDIO_FMT_PCM_U8P : r = AV_SAMPLE_FMT_U8P; break;
    case FFM_AUDIO_FMT_PCM_S16P: r = AV_SAMPLE_FMT_S16P; break;
    case FFM_AUDIO_FMT_PCM_S32P: r = AV_SAMPLE_FMT_S32P; break;
    case FFM_AUDIO_FMT_PCM_FLTP: r = AV_SAMPLE_FMT_FLTP; break;
    case FFM_AUDIO_FMT_PCM_DBLP: r = AV_SAMPLE_FMT_DBLP; break;
    default : r = AV_SAMPLE_FMT_NONE; break;
    }
    return r;
}

static FFM_AudioFormat back_translate_sample_fmt(enum AVSampleFormat fmt)
{
    int i;

    for (i=0;i<FFM_AUDIO_FMT_MAX_VALUE;i++) {
        if (fmt==translate_sample_fmt((FFM_AudioFormat)i)) {
            return (FFM_AudioFormat)i;
        }
    }
    return FFM_AUDIO_FMT_UNKNOWN;
}

struct _FFM_AudioDecodeContext
{
    AVCodec*                codec;
    AVCodecContext*         avctx;
    AVFrame*                frame;
    unsigned int            nb_samples;
#ifdef FFABI_AVCODEC_OLD_API
    int                     have_frame;
#else
    int                     eos;
#endif
};

FFM_AudioDecodeContext* __cdecl ffm_audio_decode_init(void* logctx,const char* name,FFM_AudioFormat fmt,const char* argp[],const uint8_t* CodecData,unsigned int CodecDataSize,unsigned int time_base,unsigned int CodecFlags)
{
    AVDictionary *opts = NULL;
    FFM_AudioDecodeContext* ctx;
    int i;

    ctx = av_mallocz(sizeof(FFM_AudioDecodeContext));
    if (!ctx) {
        return NULL;
    }

    ctx->codec = avcodec_find_decoder_by_name(name);
    if (!ctx->codec) {
        ffm_audio_decode_close(ctx);
        return NULL;
    }

    ctx->avctx = avcodec_alloc_context3(ctx->codec);
    if (!ctx->avctx) {
        ffm_audio_decode_close(ctx);
        return NULL;
    }
    ctx->avctx->opaque = logctx;
    ctx->avctx->time_base.num = 1;
    ctx->avctx->time_base.den = time_base;
#if defined(FFABI_HAVE_REFCOUNTED_FRAMES) && defined(FFABI_AVCODEC_OLD_API)
    ctx->avctx->refcounted_frames = 1;
#endif

    if (CodecData) {
        ctx->avctx->extradata = (uint8_t*) CodecData;
        ctx->avctx->extradata_size = CodecDataSize;
    }

    ctx->avctx->request_sample_fmt = translate_sample_fmt(fmt);

    if (argp) {
        for (i=0;argp[i];i+=2) {
            av_dict_set(&opts, argp[i], argp[i+1], 0);
        }
    }

    if (avcodec_open2(ctx->avctx,ctx->codec,&opts)) {
        av_dict_free(&opts);
        ffm_audio_decode_close(ctx);
        return NULL;
    }
    av_dict_free(&opts);

#ifdef FFABI_AVCODEC_OLD_API
    ctx->frame = ffm_frame_alloc();
#else
    ctx->frame = av_frame_alloc();
#endif

    if (!ctx->frame) {
        ffm_audio_decode_close(ctx);
        return NULL;
    }

    return ctx;
}

int __cdecl ffm_audio_decode_close(FFM_AudioDecodeContext* ctx)
{
    if (ctx->frame) {
#ifdef FFABI_AVCODEC_OLD_API
        av_frame_unref(ctx->frame);
        ffm_frame_free(&ctx->frame);
#else
        av_frame_free(&ctx->frame);
#endif
    }

    if (ctx->avctx) {
        avcodec_close(ctx->avctx);
        av_free(ctx->avctx);
    }

    av_free(ctx);

    return 0;
}

static int av_cold ffmerr(uint8_t type,unsigned int value)
{
    uint32_t v;

    v = 0x80000000 | (((uint32_t)type)<<28) | ((uint32_t)(value&0x0fffffff));

    return (int)(int32_t)v;
}


#ifdef FFABI_AVCODEC_OLD_API

int __cdecl ffm_audio_decode_put_data(FFM_AudioDecodeContext* ctx,const uint8_t* data,unsigned int size,int64_t pts)
{
    int                 r=-1;
    struct AVPacket     avpkt;

    av_init_packet(&avpkt);
    avpkt.pts = pts;
    avpkt.dts = pts;
    avpkt.data = (uint8_t*)data;
    avpkt.size = size;

    av_frame_unref(ctx->frame);
    ctx->have_frame = 0;

    r = avcodec_decode_audio4(ctx->avctx,ctx->frame,&ctx->have_frame,&avpkt);
    if (r != size) { return ffmerr(0,r); }

    if (ctx->have_frame) {
#ifdef FFABI_HAVE_AV_FRAME_CHANNELS
        if (av_frame_get_channels(ctx->frame) != ctx->avctx->channels) { return ffmerr(1,av_frame_get_channels(ctx->frame)); }
#endif
#ifdef FFABI_HAVE_AV_FRAME_SAMPLE_RATE
        if (av_frame_get_sample_rate(ctx->frame) != ctx->avctx->sample_rate) { return ffmerr(2,av_frame_get_sample_rate(ctx->frame)); }
#endif
        if (ctx->frame->format != ctx->avctx->sample_fmt) { return ffmerr(3,ctx->frame->format); }
        if (ctx->nb_samples) {
            if (ctx->nb_samples < ctx->frame->nb_samples) { return ffmerr(5,ctx->frame->nb_samples); }
        } else {
            ctx->nb_samples = ctx->frame->nb_samples;
        }
    }

    return 0;
}

int __cdecl ffm_audio_decode_get_frame(FFM_AudioDecodeContext* ctx,int64_t* pts,const uint8_t* data[])
{
    int i;

    if (!ctx->have_frame) { return 0; }

    ctx->have_frame = 0;

    *pts = ctx->frame->pts;

    data[0]=ctx->frame->data[0];

    if (av_sample_fmt_is_planar(ctx->avctx->sample_fmt)) {
        for (i=1;i<ctx->avctx->channels;i++) {
            data[i] = ctx->frame->data[i];
        }
    }

    return ctx->frame->nb_samples;
}

#else // FFABI_AVCODEC_OLD_API

int __cdecl ffm_audio_decode_put_data(FFM_AudioDecodeContext* ctx,const uint8_t* data,unsigned int size,int64_t pts)
{
    int r;

    if (NULL!=data) {
        struct AVPacket     avpkt;

        av_init_packet(&avpkt);
        avpkt.pts = pts;
        avpkt.dts = pts;
        avpkt.data = (uint8_t*)data;
        avpkt.size = size;

        r = avcodec_send_packet(ctx->avctx,&avpkt);
    } else {
        if (!ctx->eos) {
            r = avcodec_send_packet(ctx->avctx,NULL);
            ctx->eos = 1;
        } else {
            r = 0;
        }
    }
    return r;
}

int __cdecl ffm_audio_decode_get_frame(FFM_AudioDecodeContext* ctx,int64_t* pts,const uint8_t* data[])
{
    int r,i;

    av_frame_unref(ctx->frame);

    r = avcodec_receive_frame(ctx->avctx,ctx->frame);

    if ( (r==AVERROR_EOF) || (r==AVERROR(EAGAIN)) ) return 0;
    if (r!=0) return r;

#ifdef FFABI_HAVE_AV_FRAME_CHANNELS
    if (ctx->frame->channels != ctx->avctx->channels) { return ffmerr(1,ctx->frame->channels); }
#endif
    if (ctx->frame->sample_rate != ctx->avctx->sample_rate) { return ffmerr(2,ctx->frame->sample_rate); }
    if (ctx->frame->format != ctx->avctx->sample_fmt) { return ffmerr(3,ctx->frame->format); }
    if (ctx->nb_samples) {
        if (ctx->nb_samples < ctx->frame->nb_samples) { return ffmerr(5,ctx->frame->nb_samples); }
    } else {
        ctx->nb_samples = ctx->frame->nb_samples;
    }

    *pts = ctx->frame->pts;

    data[0]=ctx->frame->data[0];

    if (av_sample_fmt_is_planar(ctx->avctx->sample_fmt)) {
        for (i=1;i<ctx->avctx->channels;i++) {
            data[i] = ctx->frame->extended_data[i];
        }
    }

    return ctx->frame->nb_samples;
}

#endif

int __cdecl ffm_audio_decode_get_info(FFM_AudioDecodeContext* ctx,FFM_AudioInfo* info)
{
    info->sample_rate = ctx->avctx->sample_rate;
    info->channels = ctx->avctx->channels;
    info->channel_layout = ctx->avctx->channel_layout;
    info->frame_size = ctx->nb_samples;
    info->sample_fmt = back_translate_sample_fmt(ctx->avctx->sample_fmt);
    info->bits_per_sample = ctx->avctx->bits_per_raw_sample;
    info->profile = ctx->avctx->profile;
    return 0;
}

struct _FFM_AudioEncodeContext
{
    AVCodec*                codec;
    AVCodecContext*         avctx;
    AVFrame*                frame;
    AVPacket                pck;
#ifdef FFABI_AVCODEC_OLD_API
    int                     have_packet;
#else
    uint8_t**               frame_extended_data;
    int                     eos;
#endif
};


FFM_AudioEncodeContext* __cdecl ffm_audio_encode_init(void* logctx,const char* name,FFM_AudioFormat fmt,FFM_AudioInfo* info,const char* argp[],unsigned int time_base,unsigned int CodecFlags)
{
    AVDictionary *opts = NULL;
    FFM_AudioEncodeContext* ctx;
    int i;

    ctx = av_mallocz(sizeof(FFM_AudioEncodeContext));
    if (!ctx) {
        return NULL;
    }

    av_init_packet(&ctx->pck);
    ctx->pck.data=NULL;
    ctx->pck.size=0;

    ctx->codec = avcodec_find_encoder_by_name(name);
    if (!ctx->codec) {
        ffm_audio_encode_close(ctx);
        return NULL;
    }

    ctx->avctx = avcodec_alloc_context3(ctx->codec);
    if (!ctx->avctx) {
        ffm_audio_encode_close(ctx);
        return NULL;
    }
#if defined(FFABI_AVCODEC_OLD_API) && defined(FFABI_HAVE_REFCOUNTED_FRAMES)
    ctx->avctx->refcounted_frames = 1;
#endif
    ctx->avctx->opaque = logctx;
    ctx->avctx->time_base.num = 1;
    ctx->avctx->time_base.den = time_base;

    ctx->avctx->sample_fmt = translate_sample_fmt(fmt);
    ctx->avctx->sample_rate = info->sample_rate;
    ctx->avctx->channels = info->channels;
    ctx->avctx->bits_per_raw_sample = info->bits_per_sample;
    ctx->avctx->channel_layout = info->channel_layout;
    ctx->avctx->profile = (info->profile != FFM_PROFILE_UNKNOWN) ?
        info->profile : FF_PROFILE_UNKNOWN;

    if ((CodecFlags&FFM_CODEC_FLAG_GLOBAL_HEADER)!=0)
        ctx->avctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (argp) {
        for (i=0;argp[i];i+=2) {
            av_dict_set(&opts, argp[i], argp[i+1], 0);
        }
    }

    if (avcodec_open2(ctx->avctx,ctx->codec,&opts)) {
        av_dict_free(&opts);
        ffm_audio_encode_close(ctx);
        return NULL;
    }
    av_dict_free(&opts);

    info->frame_size = ctx->avctx->frame_size;
    info->profile = ctx->avctx->profile;

#ifdef FFABI_AVCODEC_OLD_API
    ctx->frame = ffm_frame_alloc();
#else
    ctx->frame = av_frame_alloc();
    if (av_sample_fmt_is_planar(ctx->avctx->sample_fmt)) {
        ctx->frame_extended_data = av_mallocz_array(ctx->avctx->channels,
            sizeof(*ctx->frame_extended_data));
        if (!ctx->frame_extended_data) {
            ffm_audio_encode_close(ctx);
            return NULL;
        }
    }
#endif

    if (!ctx->frame) {
        ffm_audio_encode_close(ctx);
        return NULL;
    }

    return ctx;
}

int __cdecl ffm_audio_encode_close(FFM_AudioEncodeContext* ctx)
{
#ifdef FFABI_AVCODEC_OLD_API
    av_free_packet(&ctx->pck);

    if (ctx->frame) {
        av_frame_unref(ctx->frame);
        ffm_frame_free(&ctx->frame);
    }
#else
    av_packet_unref(&ctx->pck);
    av_frame_free(&ctx->frame);
    av_freep(&ctx->frame_extended_data);
#endif

    if (ctx->avctx) {
        avcodec_close(ctx->avctx);
        av_free(ctx->avctx);
    }

    av_free(ctx);

    return 0;
}

#ifdef FFABI_AVCODEC_OLD_API

int __cdecl ffm_audio_encode_put_frame(FFM_AudioEncodeContext* ctx,const uint8_t* frame_data[],unsigned int frame_size,unsigned int nb_samples,uint64_t pts)
{
    AVFrame* frame;

    av_free_packet(&ctx->pck);
    av_init_packet(&ctx->pck);
    ctx->pck.data=NULL;
    ctx->pck.size=0;
    ctx->have_packet = 0;

    av_frame_unref(ctx->frame);

    if (frame_data) {
        frame = ctx->frame;
        frame->format = ctx->avctx->sample_fmt;
#ifdef FFABI_HAVE_AV_FRAME_CHANNELS
        av_frame_set_channels(frame,ctx->avctx->channels);
#endif
#ifdef FFABI_HAVE_AV_FRAME_CHANNEL_LAYOUT
        av_frame_set_channel_layout(frame,ctx->avctx->channel_layout);
#endif
        frame->data[0] = (uint8_t*)frame_data[0];
        frame->linesize[0] = frame_size;

        if (av_sample_fmt_is_planar(ctx->avctx->sample_fmt)) {
            int i;
            for (i=1;i<ctx->avctx->channels;i++) {
                frame->data[i] = (uint8_t*)frame_data[i];
            }
        }

        frame->nb_samples = nb_samples;
        frame->pts = pts;
    } else {
        frame = NULL;
        if ((ctx->avctx->codec->capabilities&CODEC_CAP_DELAY)==0) {
            return 0;
        }
    }

    return avcodec_encode_audio2(ctx->avctx,&ctx->pck,frame,&ctx->have_packet);
}

int __cdecl ffm_audio_encode_get_data(FFM_AudioEncodeContext* ctx,unsigned int *size,int64_t *pts,const uint8_t** data)
{
    if (ctx->have_packet) {
        ctx->have_packet = 0;
        *size = ctx->pck.size;
        *pts = ctx->pck.pts;
        *data = ctx->pck.data;
    } else {
        *data = NULL;
    }
    return 0;
}

#else

static void ffm_frame_clean(AVFrame* frame)
{
    frame->extended_data = NULL;
    frame->data[0] = (uint8_t*)(((uintptr_t)0)-3);
    frame->linesize[0] = 0x7ffffff1;
    frame->nb_samples = 0x7ffffff2;
}

int __cdecl ffm_audio_encode_put_frame(FFM_AudioEncodeContext* ctx,const uint8_t* frame_data[],unsigned int frame_size,unsigned int nb_samples,uint64_t pts)
{
    int r;

    if (frame_data) {

        ctx->frame->format = ctx->avctx->sample_fmt;
#ifdef FFABI_HAVE_AV_FRAME_CHANNELS
        ctx->frame->channels = ctx->avctx->channels;
#endif
        ctx->frame->channel_layout = ctx->avctx->channel_layout;
        ctx->frame->nb_samples = nb_samples;
        ctx->frame->pts = pts;
        ctx->frame->linesize[0] = frame_size;

        ctx->frame->data[0] = (uint8_t*)frame_data[0];

        if (av_sample_fmt_is_planar(ctx->avctx->sample_fmt)) {
            int i;

            ctx->frame->extended_data = ctx->frame_extended_data;

            ctx->frame->extended_data[0] = ctx->frame->data[0];
            for (i=1;i<ctx->avctx->channels;i++) {
                ctx->frame->extended_data[i] = (uint8_t*)frame_data[i];
                if (i<AV_NUM_DATA_POINTERS) {
                    ctx->frame->data[i] = ctx->frame->extended_data[i];
                }
            }
        } else {
            ctx->frame->extended_data = ctx->frame->data;
        }

        r = avcodec_send_frame(ctx->avctx,ctx->frame);

        ffm_frame_clean(ctx->frame);

    } else {
        if (!ctx->eos)
        {
            r = avcodec_send_frame(ctx->avctx,NULL);
            ctx->eos = 1;
        } else {
            r = 0;
        }
    }

    return r;
}

int __cdecl ffm_audio_encode_get_data(FFM_AudioEncodeContext* ctx,unsigned int *size,int64_t *pts,const uint8_t** data)
{
    int r;

    av_packet_unref(&ctx->pck);

    r = avcodec_receive_packet(ctx->avctx,&ctx->pck);

    if ( (r==AVERROR_EOF) || (r==AVERROR(EAGAIN)) ) {
        *data = NULL;
        return 0;
    }
    if (r!=0) return r;

    *size = ctx->pck.size;
    *pts = ctx->pck.pts;
    *data = ctx->pck.data;

    return 0;
}

#endif

int __cdecl ffm_audio_encode_get_info(FFM_AudioEncodeContext* ctx,FFM_AudioEncodeInfo* info)
{
    info->extradata = (const uint8_t*)ctx->avctx->extradata;
    info->extradata_size = ctx->avctx->extradata_size;
    info->delay = (int32_t)ctx->avctx->delay;
    info->flags = 0;

    if ((ctx->avctx->flags&AV_CODEC_FLAG_GLOBAL_HEADER)!=0)
        info->flags |= FFM_CODEC_FLAG_GLOBAL_HEADER;

    return 0;
}

int __cdecl ffm_audio_get_codec_information(FFM_CodecInfo* info,const char* name,int encode)
{
    AVCodec* codec;
    int i;

    memset(info,0,sizeof(*info));

    if (encode) {
        codec = avcodec_find_encoder_by_name(name);
    } else {
        codec = avcodec_find_decoder_by_name(name);
    }

    if (!codec) {
        return ffmerr(1,encode);
    }

    info->name = codec->name;
    info->long_name = codec->long_name;
    info->sample_rates = codec->supported_samplerates;
    info->channel_layouts = codec->channel_layouts;
    info->id = codec->id;
    info->capabilities = codec->capabilities;
    if (codec->profiles) {
        for (i=0;i<32;i++) {
            if (codec->profiles[i].profile==FF_PROFILE_UNKNOWN)
                break;
            info->profiles_names[i]=codec->profiles[i].name;
            info->profiles_values[i]=codec->profiles[i].profile;
            info->profiles_count = i+1;
        }
    }
    if (codec->sample_fmts) {
        for (i=0;i<16;i++) {
            if (codec->sample_fmts[i]==AV_SAMPLE_FMT_NONE)
                break;
            info->sample_formats[i]=(uint8_t)back_translate_sample_fmt(codec->sample_fmts[i]);
            info->sample_formats_count = i+1;
        }
    }

    return 0;
}

FFM_AudioConvert* __cdecl ffm_audio_convert_alloc(FFM_AudioFormat out_fmt,FFM_AudioFormat in_fmt,int channels)
{
    return ff_ffm_audio_convert_alloc(
        translate_sample_fmt(out_fmt),
        translate_sample_fmt(in_fmt),
        channels);
}

int __cdecl ffm_mlp_read_syncframe(const uint8_t* data,unsigned int size,FFM_AudioInfo* info,uint32_t* bitrate)
{
    AVCodec*                codec;
    AVCodecParserContext*   pctx=NULL;
    AVCodecContext*         avctx=NULL;
    int                     err,outbuf_size,rest;
    uint8_t                 *outbuf;

    codec = avcodec_find_decoder(AV_CODEC_ID_MLP);
    if (!codec) {
        err=ffmerr(1,0); goto ret;
    }

    avctx = avcodec_alloc_context3(codec);
    if (!avctx) {
        err=ffmerr(2,0); goto ret;
    }

    pctx = av_parser_init(AV_CODEC_ID_MLP);
    if (!pctx) {
        err=ffmerr(3,0); goto ret;
    }

    rest = size;
    while(rest) {
        err = av_parser_parse2(pctx,avctx,&outbuf,&outbuf_size,
            data,rest,0,0,0);
        if ( (err<0) || (err>rest) ) {
            err=ffmerr(4,(unsigned int)err); goto ret;
        }
        rest -= err;
        data += err;
    }

    if (outbuf_size!=size) {
        err=ffmerr(5,outbuf_size); goto ret;
    }

    info->sample_rate = avctx->sample_rate;
    info->channels = avctx->channels;
    info->bits_per_sample = avctx->bits_per_raw_sample;
    info->channel_layout = avctx->channel_layout;
    *bitrate = avctx->bit_rate;

#ifndef FFABI_HAVE_PARSER_DURATION
    info->frame_size = avctx->frame_size;
#else
    info->frame_size = pctx->duration;
#endif

    err = 0;

ret:
    if (pctx) {
        av_parser_close(pctx);
    }

    if (avctx) {
        avcodec_close(avctx);
        av_free(avctx);
    }
    return err;
}

int __cdecl ffm_mpa_decode_header(uint32_t hdr,FFM_AudioInfo* info,uint32_t* layer,uint32_t* frame_size,uint32_t* bitrate)
{
    MPADecodeHeader s;
    int err;

    if (ff_mpa_check_header(hdr)) {
        return ffmerr(0,0);
    }

    err = avpriv_mpegaudio_decode_header(&s, hdr);
    if ((err < 0) || (err > 1)) {
        return ffmerr(1,err);
    }
    if (err==0) {
        *bitrate = s.bit_rate;
        *frame_size = s.frame_size;
    } else {
        *bitrate = 0;
        *frame_size = 0;
    }

    info->bits_per_sample = 0;
    info->channel_layout = 0;

    info->sample_rate = s.sample_rate;
    info->channels = s.nb_channels;
    *layer = s.layer;

    switch(s.layer) {
    case 1:
        info->frame_size = 384;
        break;
    case 2:
        info->frame_size = 1152;
        break;
    default:
    case 3:
        if (s.lsf)
            info->frame_size = 576;
        else
            info->frame_size = 1152;
        break;
    }

    return 0;
}

int __cdecl ffm_get_channel_layout_string(char *buf,int buf_size,uint64_t channel_layout)
{
    av_get_channel_layout_string(buf,buf_size,-1,channel_layout);
    return 0;
}

