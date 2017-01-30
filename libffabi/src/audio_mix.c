/*
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
#include <libavutil/samplefmt.h>
#include <libavutil/log.h>
#include "internal.h"

struct _FFM_AudioMix {
    uint64_t in_layout;
    uint64_t out_layout;
    int in_channels;
    int out_channels;

    int in_matrix_channels;
    int out_matrix_channels;
    int output_zero[FFM_AVRESAMPLE_MAX_CHANNELS];
    int input_skip[FFM_AVRESAMPLE_MAX_CHANNELS];
    int output_skip[FFM_AVRESAMPLE_MAX_CHANNELS];
    int32_t *matrix_q30[FFM_AVRESAMPLE_MAX_CHANNELS];
    int32_t   **matrix;
};

static int ff_audio_mix_set_matrix(FFM_AudioMix *am, const double *matrix, int stride,void* logctx);

static void mix_any_s32_q30_c(int32_t **samples, int32_t **matrix, int len, int out_ch, int in_ch)
{
    int i, in, out;
    int32_t temp[FFM_AVRESAMPLE_MAX_CHANNELS];
    for (i = 0; i < len; i++) {
        for (out = 0; out < out_ch; out++) {
            int64_t sum; sum = 0;
            for (in = 0; in < in_ch; in++)
                sum += (samples[in][i] * ((int64_t)(matrix[out][in])));
            temp[out] = av_clipl_int32(sum >> 30);
        }
        for (out = 0; out < out_ch; out++)
            samples[out][i] = temp[out];
    }
}

FFM_AudioMix* __cdecl ffm_audio_mix_alloc(void* logctx,uint64_t in_channel_layout, uint64_t out_channel_layout, const double* mix_levels, FFM_MatrixEncoding matrix_encoding)
{
    FFM_AudioMix *am;
    int ret;

    am = av_mallocz(sizeof(*am));
    if (!am)
        return NULL;

    am->in_layout    = in_channel_layout;
    am->out_layout   = out_channel_layout;
    am->in_channels  = av_get_channel_layout_nb_channels(in_channel_layout);
    am->out_channels = av_get_channel_layout_nb_channels(out_channel_layout);

    {
        double *matrix_dbl = av_mallocz(am->out_channels * am->in_channels *
                                        sizeof(*matrix_dbl));
        if (!matrix_dbl)
            goto error;

        ret = ff_avresample_build_matrix(in_channel_layout,
                                      out_channel_layout,
                                      mix_levels[0],//avr->center_mix_level,
                                      mix_levels[1],//avr->surround_mix_level,
                                      mix_levels[2],//avr->lfe_mix_level,
                                      1,//avr->normalize_mix_level,
                                      matrix_dbl,
                                      am->in_channels,
                                      matrix_encoding);
        if (ret < 0) {
            av_free(matrix_dbl);
            goto error;
        }

        ret = ff_audio_mix_set_matrix(am, matrix_dbl, am->in_channels, logctx);
        if (ret < 0) {
            av_free(matrix_dbl);
            goto error;
        }

        av_free(matrix_dbl);
    }

    return am;

error:
    av_free(am);
    return NULL;
}

void __cdecl ffm_audio_mix_free(FFM_AudioMix **am_p)
{
    FFM_AudioMix *am;

    if (!*am_p)
        return;
    am = *am_p;

    if (am->matrix) {
        av_free(am->matrix[0]);
        am->matrix = NULL;
    }
    memset(am->matrix_q30, 0, sizeof(am->matrix_q30));

    av_freep(am_p);
}

int __cdecl ffm_audio_mix(FFM_AudioMix *am, int32_t *src_data[], int nb_samples)
{
    int len = nb_samples;
    int i, j;

    if (am->in_matrix_channels && am->out_matrix_channels) {
        int32_t **data;
        int32_t *data0[FFM_AVRESAMPLE_MAX_CHANNELS] = { NULL };

        if (am->out_matrix_channels < am->out_channels ||
             am->in_matrix_channels <  am->in_channels) {
            for (i = 0, j = 0; i < FFMAX(am->in_channels, am->out_channels); i++) {
                if (am->input_skip[i] || am->output_skip[i] || am->output_zero[i])
                    continue;
                data0[j++] = src_data[i];
            }
            data = data0;
        } else {
            data = src_data;
        }

        mix_any_s32_q30_c(data, am->matrix, len, am->out_matrix_channels,
                        am->in_matrix_channels);
    }

    if (am->out_matrix_channels < am->out_channels) {
        for (i = 0; i < am->out_channels; i++)
            if (am->output_zero[i])
                memset(src_data[i],0,sizeof(int32_t)*len);
    }

    return 0;
}

static void reduce_matrix(FFM_AudioMix *am, const double *matrix, int stride)
{
    int i, o;

    memset(am->output_zero, 0, sizeof(am->output_zero));
    memset(am->input_skip,  0, sizeof(am->input_skip));
    memset(am->output_skip, 0, sizeof(am->output_skip));

    /* exclude output channels if they can be zeroed instead of mixed */
    for (o = 0; o < am->out_channels; o++) {
        int zero = 1;

        /* check if the output is always silent */
        for (i = 0; i < am->in_channels; i++) {
            if (matrix[o * stride + i] != 0.0) {
                zero = 0;
                break;
            }
        }
        /* check if the corresponding input channel makes a contribution to
           any output channel */
        if (o < am->in_channels) {
            for (i = 0; i < am->out_channels; i++) {
                if (matrix[i * stride + o] != 0.0) {
                    zero = 0;
                    break;
                }
            }
        }
        if (zero) {
            am->output_zero[o] = 1;
            am->out_matrix_channels--;
            if (o < am->in_channels)
                am->in_matrix_channels--;
        }
    }
    if (am->out_matrix_channels == 0 || am->in_matrix_channels == 0) {
        am->out_matrix_channels = 0;
        am->in_matrix_channels = 0;
        return;
    }

    /* skip input channels that contribute fully only to the corresponding
       output channel */
    for (i = 0; i < FFMIN(am->in_channels, am->out_channels); i++) {
        int skip = 1;

        for (o = 0; o < am->out_channels; o++) {
            int i0;
            if ((o != i && matrix[o * stride + i] != 0.0) ||
                (o == i && matrix[o * stride + i] != 1.0)) {
                skip = 0;
                break;
            }
            /* if the input contributes fully to the output, also check that no
               other inputs contribute to this output */
            if (o == i) {
                for (i0 = 0; i0 < am->in_channels; i0++) {
                    if (i0 != i && matrix[o * stride + i0] != 0.0) {
                        skip = 0;
                        break;
                    }
                }
            }
        }
        if (skip) {
            am->input_skip[i] = 1;
            am->in_matrix_channels--;
        }
    }
    /* skip input channels that do not contribute to any output channel */
    for (; i < am->in_channels; i++) {
        int contrib = 0;

        for (o = 0; o < am->out_channels; o++) {
            if (matrix[o * stride + i] != 0.0) {
                contrib = 1;
                break;
            }
        }
        if (!contrib) {
            am->input_skip[i] = 1;
            am->in_matrix_channels--;
        }
    }
    if (am->in_matrix_channels == 0) {
        am->out_matrix_channels = 0;
        return;
    }

    /* skip output channels that only get full contribution from the
       corresponding input channel */
    for (o = 0; o < FFMIN(am->in_channels, am->out_channels); o++) {
        int skip = 1;
        int o0;

        for (i = 0; i < am->in_channels; i++) {
            if ((o != i && matrix[o * stride + i] != 0.0) ||
                (o == i && matrix[o * stride + i] != 1.0)) {
                skip = 0;
                break;
            }
        }
        /* check if the corresponding input channel makes a contribution to
           any other output channel */
        i = o;
        for (o0 = 0; o0 < am->out_channels; o0++) {
            if (o0 != i && matrix[o0 * stride + i] != 0.0) {
                skip = 0;
                break;
            }
        }
        if (skip) {
            am->output_skip[o] = 1;
            am->out_matrix_channels--;
        }
    }
    if (am->out_matrix_channels == 0) {
        am->in_matrix_channels = 0;
        return;
    }
}

static const AVClass av_audio_mix_log_context_class = {
    .class_name = "FFM_AudioMix",
    .item_name  = av_default_item_name,
    .version    = LIBAVUTIL_VERSION_INT,
};

static int ff_audio_mix_set_matrix(FFM_AudioMix *am, const double *matrix, int stride,void* logctx)
{
    int i, o, i0, o0;
    char in_layout_name[128];
    char out_layout_name[128];
    ffabi_Logger flog = { &av_audio_mix_log_context_class , logctx };

    if ( am->in_channels <= 0 ||  am->in_channels > FFM_AVRESAMPLE_MAX_CHANNELS ||
        am->out_channels <= 0 || am->out_channels > FFM_AVRESAMPLE_MAX_CHANNELS) {
        av_log(&flog, AV_LOG_ERROR, "Invalid channel counts\n");
        return AVERROR(EINVAL);
    }

    if (am->matrix) {
        av_free(am->matrix[0]);
        am->matrix = NULL;
    }

    am->in_matrix_channels  = am->in_channels;
    am->out_matrix_channels = am->out_channels;

    reduce_matrix(am, matrix, stride);

#define CONVERT_MATRIX(type, expr)                                          \
    am->matrix_## type[0] = (int32_t*)av_mallocz(am->out_matrix_channels *  \
                                       am->in_matrix_channels  *            \
                                       sizeof(*am->matrix_## type[0]));     \
    if (!am->matrix_## type[0])                                             \
        return AVERROR(ENOMEM);                                             \
    for (o = 0, o0 = 0; o < am->out_channels; o++) {                        \
        if (am->output_zero[o] || am->output_skip[o])                       \
            continue;                                                       \
        if (o0 > 0)                                                         \
            am->matrix_## type[o0] = am->matrix_## type[o0 - 1] +           \
                                     am->in_matrix_channels;                \
        for (i = 0, i0 = 0; i < am->in_channels; i++) {                     \
            double v;                                                       \
            if (am->input_skip[i] || am->output_zero[i])                    \
                continue;                                                   \
            v = matrix[o * stride + i];                                     \
            am->matrix_## type[o0][i0] = expr;                              \
            i0++;                                                           \
        }                                                                   \
        o0++;                                                               \
    }                                                                       \
    am->matrix = am->matrix_## type;

    if (am->in_matrix_channels && am->out_matrix_channels) {
        CONVERT_MATRIX(q30, av_clipl_int32(llrint(1073741824.0 * v)))
    }

    av_get_channel_layout_string(in_layout_name, sizeof(in_layout_name),
                                 am->in_channels, am->in_layout);
    av_get_channel_layout_string(out_layout_name, sizeof(out_layout_name),
                                 am->out_channels, am->out_layout);
    av_log(&flog, AV_LOG_DEBUG, "audio_mix: %s to %s\n",
           in_layout_name, out_layout_name);
    av_log(&flog, AV_LOG_DEBUG, "matrix size: %d x %d\n",
           am->in_matrix_channels, am->out_matrix_channels);
    for (o = 0; o < am->out_channels; o++) {
        for (i = 0; i < am->in_channels; i++) {
            if (am->output_zero[o])
                av_log(&flog, AV_LOG_DEBUG, "  (ZERO)");
            else if (am->input_skip[i] || am->output_zero[i] || am->output_skip[o])
                av_log(&flog, AV_LOG_DEBUG, "  (SKIP)");
            else
                av_log(&flog, AV_LOG_DEBUG, "  %0.3f ",
                       matrix[o * am->in_channels + i]);
        }
        av_log(&flog, AV_LOG_DEBUG, "\n");
    }

    return 0;
}
