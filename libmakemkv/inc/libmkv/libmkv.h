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
#ifndef LIBMKV_H_INCLUDED
#define LIBMKV_H_INCLUDED

#include <stdint.h>
#include <string.h>

typedef char mkv_utf8_t;

static const unsigned int MKV_CHUNK_KEYFRAME=1;
static const unsigned int MKV_CHUNK_CLUSTER_START=2;
static const unsigned int MKV_CHUNK_CHAPTER_MARK=4;
static const unsigned int MKV_CHUNK_DISCARDABLE=8;
static const unsigned int MKV_CHUNK_OLD_BLOCK=16;
static const unsigned int MKV_CHUNK_AUTO_DURATION=32;

static const unsigned int MKV_TRACK_COMPRESSION_ZLIB=0;
static const unsigned int MKV_TRACK_COMPRESSION_BZ2=1;
static const unsigned int MKV_TRACK_COMPRESSION_LZO=2;
static const unsigned int MKV_TRACK_COMPRESSION_HEADERS=3;
static const unsigned int MKV_TRACK_COMPRESSION_NONE=4;

static const unsigned int MKV_TRACK_FLAG_DEFAULT=1;
static const unsigned int MKV_TRACK_FLAG_FORCED=2;
static const unsigned int MKV_TRACK_FLAG_LACING=128;

class IMkvChunk
{
public:
    virtual const uint8_t*  get_data()=0;
    virtual unsigned int    get_size()=0;
    virtual void*           get_ref()=0;
    virtual bool            compress_start(unsigned int compressionType,unsigned int compressionLevel)=0;
    virtual bool            compress_wait()=0;
    virtual unsigned int    compress_srcsize()=0;
public:
    int64_t     timecode;
    int64_t     duration;
    uint32_t    flags;
public:
    inline bool keyframe()      const { return (0!=(flags&MKV_CHUNK_KEYFRAME)); }
    inline bool cluster_start() const { return (0!=(flags&MKV_CHUNK_CLUSTER_START)); }
    inline bool chapter_mark()  const { return (0!=(flags&MKV_CHUNK_CHAPTER_MARK)); }
    inline bool discardable()   const { return (0!=(flags&MKV_CHUNK_DISCARDABLE)); }
    inline bool old_block()     const { return (0!=(flags&MKV_CHUNK_OLD_BLOCK)); }
    inline bool auto_duration() const { return (0!=(flags&MKV_CHUNK_AUTO_DURATION)); }
};

typedef enum _MkvTrackType
{
    mttUnknown,
    mttVideo,
    mttAudio,
    mttSubtitle
} MkvTrackType;

typedef struct _MkvProfileTrackInfo
{
    unsigned int        compressionType;
    unsigned int        compressionLevel;
} MkvProfileTrackInfo;

typedef struct _MkvTrackInfo
{
    MkvTrackType        type;
    const char*         codec_id;
    const char*         codec_subid;
    const char*         lang;
    const mkv_utf8_t*   metadata_lang;
    const mkv_utf8_t*   name;

    const unsigned char *codec_private;
    unsigned int        codec_private_size;

    const unsigned char *header_comp_data;
    unsigned int        header_comp_size;

    unsigned int        mkv_flags;

    int64_t             default_duration;
    int64_t             dts_adjust;

    unsigned int        bitrate;

    uint32_t            stream_flags;
    int                 stream_subtype;

    unsigned int        min_cache;

    const MkvProfileTrackInfo*  profile_track_info;

    union _u
    {
        struct _video
        {
            int     pixel_h;
            int     pixel_v;

            int     display_h;
            int     display_v;

            int     fps_n;
            int     fps_d;

            uint64_t priv[2];

            uint8_t stereo_mode;
        } video;

        struct _audio
        {
            int     sample_rate;
            int     channels_count;
            int     bits_per_sample;
            uint64_t channel_layout;
        } audio;

        struct _subtitles
        {
            uint8_t offset_sequence_id_ref;
        } subtitles;
    } u;

    uint8_t source_id[8];

} MkvTrackInfo;

class IMkvFrameSource
{
public:
    virtual unsigned int    GetAvailableFramesCount()=0;
    virtual void            PopFrame()=0;
    virtual bool            SourceFinished()=0;
    virtual bool            FetchFrames(unsigned int Count,bool Force=false)=0;
    virtual IMkvChunk*      PeekFrame(unsigned int Index)=0;
    virtual bool            UpdateTrackInfo(MkvTrackInfo* Info)=0;
};

class IMkvWriteTarget
{
public:
    virtual bool            Write(const void *Data,unsigned int Size)=0;
    virtual bool            Overwrite(uint64_t Offset,const void *Data,unsigned int Size)=0;
};

class IMkvTrack
{
public:
    virtual unsigned int MkvGetStreamCount()=0;
    virtual IMkvFrameSource* MkvGetStream(unsigned int Index)=0;
};

typedef struct _MkvChapterInfo
{
    unsigned int                name_count;
    const char* const *         name_lang;
    const mkv_utf8_t* const *   name_text;
} MkvChapterInfo;

typedef struct _MkvAttachmentInfo
{
    const mkv_utf8_t*   name;
    const char*         mime_type;
    const void*         data;
    unsigned int        size;
} MkvAttachmentInfo;

typedef struct _MkvTitleInfo
{
    const mkv_utf8_t*   metadata_lang;
    const mkv_utf8_t*   name;
} MkvTitleInfo;

class IMkvTitleInfo
{
public:
    virtual unsigned int GetChapterCount()=0;
    virtual void GetChapterInfo(MkvChapterInfo *info,unsigned int chapter_id)=0;
    virtual void GetMkvTitleInfo(MkvTitleInfo *info)=0;
    virtual unsigned int GetAttachmentCount()=0;
    virtual void GetAttachmentInfo(MkvAttachmentInfo* info,unsigned int attachment_id)=0;
};

typedef struct _MkvDebugInfo
{
    int                 evoid[8];
    unsigned int        compatFlags;
} MkvDebugInfo;

typedef struct _MkvProfileInfo
{
    int     version;
    bool    useISO639Type2T;
} MkvProfileInfo;

typedef struct _MkvFormatInfo
{
    MkvProfileInfo      profile;
    MkvDebugInfo        debug;
} MkvFormatInfo;

#ifndef _MSC_VER
#define __cdecl
#endif

extern "C"
bool __cdecl MkvCreateFile(IMkvWriteTarget* Output,IMkvTrack *Input,const mkv_utf8_t *WritingApp,IMkvTitleInfo* TitleInfo,MkvFormatInfo* FormatInfo) throw();

#endif // LIBMKV_H_INCLUDED
