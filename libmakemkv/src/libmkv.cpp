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
#include <libmkv/libmkv.h>
#include <lgpl/world.h>
#include <lgpl/apdefs.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS 1
#endif
#include <inttypes.h>

#include <lgpl/stl.h>
#include <libmkv/internal.h>
#include <libmkv/ebmlwrite.h>
#include <lgpl/cassert>
#include <lgpl/sstring.h>

#define TIMECODE_SCALE              1000000
#define MAX_TIMECODE_SIZE_BYTES     6
#define MAX_TIMECODE                ((((uint64_t)1)<<(8*MAX_TIMECODE_SIZE_BYTES))-1)
#define AUTO_DURATION_TIMECODE      4500000000ll
#define BAD_TIMECODE                ((1ll<<62)+1)

#define CNZ(x) if (!(x)) { ThrowMkvExceptionUnbuffered( "Error in " #x ); };

template <class Tv,class Te>
static inline Tv& GetChild(EbmlMaster *node)
{
    return * (static_cast<Tv *>(&GetChild<Te>( *node )));
}

template <class Tv,class Te>
static inline Tv& GetChild(EbmlMaster &node)
{
    return GetChild<Tv,Te>(&node);
}

static void ThrowMkvExceptionUnbuffered(const char* Msg)
{
    throw mkv_error_exception_unbuffered(Msg);
};

static inline int64_t ScaleTimecode(int64_t UnscaledTimecode)
{
    return UnscaledTimecode / TIMECODE_SCALE;
}

static inline uint64_t TimecodeFromClock(uint64_t MkvClockTimecode)
{
    return ((MkvClockTimecode*25)/27);
}

static KaxSeek* CreateSeekEntry(KaxSeekHead& Seek,const EbmlId& aId)
{
    KaxSeek & aNewPoint = AddNewChild<KaxSeek>(Seek);

    // add the informations to this element
    KaxSeekPosition & aNewPos = GetChild<KaxSeekPosition>(aNewPoint);
    EbmlUInteger * pNewPos = static_cast<EbmlUInteger *>(&aNewPos);
    pNewPos->SetDefaultSize(8);
    *pNewPos = 0;

    KaxSeekID & aNewID = GetChild<KaxSeekID>(aNewPoint);
    binary ID[4];
    aId.Fill(ID);
    aNewID.CopyBuffer(ID, EBML_ID_LENGTH(aId));
    return &aNewPoint;
}

template <class Te>
static inline KaxSeek* AddSeekEntry(KaxSeekHead& Seek)
{
    return CreateSeekEntry(Seek,EBML_ID(Te));
}

static inline void UpdateSeekEntry(KaxSeek* Seek,IOCallback &File,const EbmlElement & aElt, const KaxSegment & ParentSegment)
{
    GetChild<EbmlUInteger,KaxSeekPosition>(Seek) = ParentSegment.GetRelativePosition(aElt);
    GetChild<KaxSeekPosition>(*Seek).OverwriteData(File,true);
}

static inline void VoidElement(EbmlMaster* Element,IOCallback &File)
{
    EbmlVoid v;
    CNZ(v.Overwrite(*Element,File));

    uint64 CurrentPosition = File.getFilePointer();
    File.setFilePointer(v.GetElementPosition()+v.HeadSize());
    CNZ(v.RenderData(File,true));
    File.setFilePointer(CurrentPosition);
}

static void OverwriteIntegerData(IOCallback & output, uint64_t Value, uint64_t Position, unsigned int Size)
{
    binary FinalData[8]; // we don't handle more than 64 bits integers

    uint64 TempValue = Value;
    for (unsigned int i=0; i<Size;i++) {
        FinalData[Size-i-1] = TempValue & 0xFF;
        TempValue >>= 8;
    }

    uint64 CurrentPosition = output.getFilePointer();
    output.setFilePointer(Position);
    output.writeFully(FinalData,Size);
    output.setFilePointer(CurrentPosition);
}

static void FinishCluster(KaxCluster *cluster,IOCallback *File,KaxSegment *Segment);

static int64_t GetClusterTimecode(IMkvTrack *Input)
{
    int64_t rtn = -1;
    for (unsigned int i=0;i<Input->MkvGetStreamCount();i++)
    {
        IMkvFrameSource *p;
        unsigned int frameCount;

        p = Input->MkvGetStream(i);

        if (i==0)
        {
            CNZ(p->FetchFrames(2,true));
        } else {
            CNZ(p->FetchFrames(1,false));
        }

        frameCount = p->GetAvailableFramesCount();

        if (i==0)
        {
            if (frameCount>16) frameCount=16;
        } else {
            if (frameCount>2) frameCount=2;
        }

        for (unsigned int j=0;j<frameCount;j++)
        {
            int64_t ft;

            ft = p->PeekFrame(j)->timecode;

            MKV_ASSERT(ft!=-1);

            if (rtn==-1) rtn = ft;

            if (ft<rtn) rtn=ft;
        }
    }

    MKV_ASSERT(rtn!=-1);

    return rtn;
}

static UTFstring UTF8string(const char *str)
{
    UTFstring rtn;

    rtn.SetUTF8(str);

    return rtn;
}

static uint64_t Rand64()
{
    uint64_t value;
    for (unsigned int k=0;k<sizeof(value);k++)
    {
        ((uint8_t*)&value)[k]=lgpl_get_random_byte();
    }
    return value;
}

class CChapters
{
private:
    KaxChapters         m_Chapters;
    std::vector<KaxChapterAtom*> m_Atoms;
    unsigned int        m_RealCount;
    bool                m_DoRender;
    IMkvTitleInfo*      m_TitleInfo;
    MkvFormatInfo*      m_FormatInfo;
public:
    CChapters(IMkvTitleInfo* TitleInfo,MkvFormatInfo* FormatInfo)
        : m_RealCount(0) , m_TitleInfo(TitleInfo) , m_FormatInfo(FormatInfo)
    {
        MkvChapterInfo  ti;
        unsigned int chap_count = m_TitleInfo->GetChapterCount();

        m_DoRender=false;

        if (chap_count==0) return;

        KaxEditionEntry &edit = CreateEdition();

        for (unsigned int i=0;i<chap_count;i++)
        {
            KaxChapterAtom* atom;

            memset(&ti,0,sizeof(ti));
            m_TitleInfo->GetChapterInfo(&ti,i);

            atom = & AddNewChild<KaxChapterAtom>(edit);
            GetChild<EbmlUInteger,KaxChapterUID>( *atom ) = Rand64();

            GetChild<EbmlUInteger,KaxChapterTimeStart>( *atom ) = 0;
            GetChild<EbmlUInteger,KaxChapterTimeStart>( *atom ).SetDefaultSize(MAX_TIMECODE_SIZE_BYTES);
            GetChild<EbmlUInteger,KaxChapterTimeEnd>( *atom ) = 0;
            GetChild<EbmlUInteger,KaxChapterTimeEnd>( *atom ).SetDefaultSize(MAX_TIMECODE_SIZE_BYTES);

            for (unsigned int j=0;j<ti.name_count;j++)
            {
                SetChapterName(atom,ti.name_lang[j],ti.name_text[j]);
            }

            m_Atoms.push_back(atom);
        }
        m_DoRender = (chap_count>0);
    }
    void Render(IOCallback &File,KaxSeek* Seek,const KaxSegment & FileSegment)
    {
        if (m_DoRender)
        {
            CNZ(m_Chapters.Render(File,true));
            UpdateSeekEntry(Seek,File,m_Chapters,FileSegment);
        } else {
            VoidElement(Seek,File);
        }
    }
    void AddChapterMark(uint64_t Timecode,IOCallback &File)
    {
        if (!m_DoRender) return;

        MKV_ASSERT(Timecode < MAX_TIMECODE);

        if (m_RealCount>0)
        {
            GetChild<EbmlUInteger,KaxChapterTimeEnd>( *(m_Atoms[m_RealCount-1]) ) = TimecodeFromClock(Timecode);
            CNZ(m_Atoms[m_RealCount-1]->OverwriteData(File,true));
        }
        GetChild<EbmlUInteger,KaxChapterTimeStart>( *(m_Atoms[m_RealCount]) ) = TimecodeFromClock(Timecode);
        CNZ(m_Atoms[m_RealCount]->OverwriteData(File,true));
        m_RealCount++;
    }
    void Finalize(IOCallback &File,KaxSeek* Seek,uint64_t Duration)
    {
        if (!m_DoRender) return;

        if (m_RealCount<2)
        {
            VoidElement(&m_Chapters,File);
            VoidElement(Seek,File);
            return;
        }

        GetChild<EbmlUInteger,KaxChapterTimeEnd>( *m_Atoms[m_RealCount-1] ) = TimecodeFromClock(Duration);
        CNZ(m_Atoms[m_RealCount-1]->OverwriteData(File,true));

        for (unsigned int i=m_RealCount;i<m_Atoms.size();i++)
        {
            VoidElement(m_Atoms[i],File);
        }
    }
private:
    void SetChapterName(KaxChapterAtom* atom,const char *lang,const mkv_utf8_t* name)
    {
        char langBuf[4];
        if (m_FormatInfo->profile.useISO639Type2T)
        {
            strcpy(langBuf,lang);
            lgpl_iso6392T(langBuf);
            lang=langBuf;
        }
        KaxChapterDisplay& disp = AddNewChild<KaxChapterDisplay>(*atom);
        GetChild<EbmlString,KaxChapterLanguage>(disp) = lang;
        GetChild<EbmlUnicodeString,KaxChapterString>(disp) = UTF8string(name);
    }
    KaxEditionEntry& CreateEdition()
    {
        KaxEditionEntry &edit = GetChild<KaxEditionEntry>(m_Chapters);
        GetChild<EbmlUInteger,KaxEditionFlagDefault>(edit) = 1;
        GetChild<EbmlUInteger,KaxEditionUID>(edit) = Rand64();

        return edit;
    }
};

static bool CompareTimecode(int64_t Timecode1,int64_t Timecode2)
{
    int64_t Diff;
    if (Timecode1>Timecode2)
    {
        Diff = Timecode1 - Timecode2;
    } else {
        Diff = Timecode2 - Timecode1;
    }
    return (Diff<3);
}

class MyMkvTrackInfo
{
public:
    MkvTrackInfo    info;
    uint64_t        duration_pos;
    uint64_t        duration_time;
    uint64_t        refs[2];
    unsigned int    duration_size;
    unsigned int    compression_type;
    unsigned int    compression_level;
    EbmlBinary*     codec_private;
    uint64_t        stat_frames;
    uint64_t        stat_bytes;
    uint64_t        stat_bytes_out;
    int64_t         stat_time_start;
    int64_t         stat_time_end;
public:
    void UpdateStat(IMkvChunk*  frame)
    {
        int64_t time_end = frame->timecode + frame->duration;
        if (stat_frames)
        {
            if (frame->timecode<stat_time_start) {
                stat_time_start = frame->timecode;
            }
            if (time_end>stat_time_end) {
                stat_time_end = time_end;
            }
        } else {
            stat_time_start = frame->timecode;
            stat_time_end = time_end;
        }
        stat_frames++;
        CNZ(frame->compress_wait());
        stat_bytes += frame->compress_srcsize();
        stat_bytes_out += frame->get_size();
    }
    void RenderStat(KaxTags &tags,unsigned int trackUID,uint64_t mtime,const char* WritingApp)
    {
        KaxTag &tag = AddNewChild<KaxTag>(tags);
        KaxTagTargets &targets = GetChild<KaxTagTargets>(tag);
        GetChild<EbmlUInteger,KaxTagTrackUID>(targets) = trackUID;
        GetChild<EbmlUInteger,KaxTagTargetTypeValue>(targets) = 50;
        GetChild<EbmlString,KaxTagTargetType>(targets) = "MOVIE";

        static const char* names[6]={
            "BPS","DURATION","NUMBER_OF_FRAMES",
            "NUMBER_OF_BYTES","NUMBER_OF_BYTES_UNCOMPRESSED","SOURCE_ID"};

        char buffer[256];
        uint64_t values[4];
        uint8_t have = 0;

        values[1] = TimecodeFromClock(stat_time_end-stat_time_start);
        values[0] = (values[1]!=0)?(stat_bytes*8000/(values[1]/1000000)):0;
        values[2] = stat_frames;
        values[3] = stat_bytes_out;

        for (unsigned int i=0;i<4;i++)
        {
            if (i==1)
            {
                unsigned int dec,sec,min,hr;
                uint64_t t = values[1];
                dec = (unsigned int)(t % 1000000000); t /= 1000000000;
                sec = (unsigned int)(t % 60); t /= 60;
                min = (unsigned int)(t % 60); t /= 60;
                hr = (unsigned int) t;
                sprintf_s(buffer,sizeof(buffer),"%02u:%02u:%02u.%09u",hr,min,sec,dec);
            } else {
                sprintf_s(buffer,sizeof(buffer),"%" PRIu64,values[i]);
            }
            AddSimple(tag,names[i],buffer);
        }

        if (stat_bytes_out!=stat_bytes)
        {
            have |= 1;
            sprintf_s(buffer,sizeof(buffer),"%" PRIu64,stat_bytes);
            AddSimple(tag,names[4],buffer);
        }

        if (info.source_id[0]!=0)
        {
            have |= 2;
            for (unsigned int i=0;i<info.source_id[0];i++)
            {
                sprintf_s(buffer+i*2,(sizeof(buffer)-(i*2)),"%02X",info.source_id[i+1]);
            }
            AddSimple(tag,names[5],buffer);
        }

        AddSimple(tag,"_STATISTICS_WRITING_APP",WritingApp);

        struct tm t;
        libmkv_gmtime_r(mtime,&t);
        sprintf_s(buffer,sizeof(buffer),"%04u-%02u-%02u %02u:%02u:%02u",
            t.tm_year+1900,t.tm_mon+1,t.tm_mday,
            t.tm_hour,t.tm_min,t.tm_sec);
        AddSimple(tag,"_STATISTICS_WRITING_DATE_UTC",buffer);

        sprintf_s(buffer,sizeof(buffer),"%s %s %s %s",
            names[0],names[1],names[2],names[3]);
        for (unsigned int i=0;i<2;i++)
        {
            if ((have&(1<<i))!=0)
            {
                strcat(buffer," ");
                strcat(buffer,names[i+4]);
            }
        }
        AddSimple(tag,"_STATISTICS_TAGS",buffer);

        if ((info.stream_flags&AP_AVStreamFlag_OffsetSequenceIdPresent)!=0)
        {
            sprintf_s(buffer,sizeof(buffer),"%u",(unsigned int)info.u.subtitles.offset_sequence_id_ref);
            AddSimple(tag,"3d-plane",buffer);
        }
    }
    static void AddSimple(KaxTag &tag,const char* Name,const char* Value)
    {
        KaxTagSimple& simple = AddNewChild<KaxTagSimple>(tag);
        GetChild<EbmlUnicodeString,KaxTagName>(simple) = UTF8string(Name);
        GetChild<EbmlString,KaxTagLangue>(simple) = "eng";
        GetChild<EbmlUInteger,KaxTagDefault>(simple) = 1;
        GetChild<EbmlUnicodeString,KaxTagString>(simple) = UTF8string(Value);
    }
};

class MyDataBuffer : public DataBuffer
{
private:
    void*   ref;
public:
    MyDataBuffer(IMkvChunk* aChunk,unsigned int aOffset=0);
private:
    bool FreeBuffer() const
    {
        lgpl_free_chunk_ref(ref);
        return true;
    }
    static bool MyFreeBufferStatic(const DataBuffer & aBuffer)
    {
        const DataBuffer* p = &aBuffer;
        const MyDataBuffer* pm = static_cast<const MyDataBuffer*>(p);
        return pm->FreeBuffer();
    }
};

MyDataBuffer::MyDataBuffer(IMkvChunk* aChunk,unsigned int aOffset)
 : DataBuffer((binary*)aChunk->get_data()+aOffset,aChunk->get_size()-aOffset,MyFreeBufferStatic)
{
    ref = aChunk->get_ref();
}

static void RenderVoid(IOCallback &File,unsigned int Size)
{
    if (Size==0) return;

    if ( (Size>4) && (Size<30000) )
    {
        EbmlVoid v;
        v.SetSize(Size-3);
        v.SetSizeLength(2);
        CNZ(v.Render(File,true));
        return;
    }

    throw mkv_error_exception("RenderVoid failed");
}

static DataBuffer* GetDataBuffer(IMkvChunk* frame,MyMkvTrackInfo* track)
{
    DataBuffer* mkv_buffer;

    if (track->compression_type==MKV_TRACK_COMPRESSION_HEADERS)
    {
        MKV_ASSERT(frame->get_size()>track->info.header_comp_size);
        if (memcmp(frame->get_data(),track->info.header_comp_data,track->info.header_comp_size))
        {
            throw mkv_error_exception("header_comp_data missing");
        }
        mkv_buffer = new MyDataBuffer(frame,track->info.header_comp_size);
    } else {
        CNZ(frame->compress_start(track->compression_type,track->compression_level));
        CNZ(frame->compress_wait());
        mkv_buffer = new MyDataBuffer(frame);
    }
    return mkv_buffer;
}

static void MkvCreateFileInternal(IOCallback &File,IMkvTrack *Input,IMkvTitleInfo* TitleInfo,MkvFormatInfo* FormatInfo,const char *WritingApp)
{
    EbmlHead FileHead;

    EDocType & MyDocType = GetChild<EDocType>(FileHead);
    *static_cast<EbmlString *>(&MyDocType) = "matroska";

    EDocTypeVersion & MyDocTypeVer = GetChild<EDocTypeVersion>(FileHead);
    *(static_cast<EbmlUInteger *>(&MyDocTypeVer)) = MATROSKA_VERSION;

    EDocTypeReadVersion & MyDocTypeReadVer = GetChild<EDocTypeReadVersion>(FileHead);
    *(static_cast<EbmlUInteger *>(&MyDocTypeReadVer)) = 2;

    CNZ(FileHead.Render(File,true));

    // objects
    KaxSegment FileSegment;

    KaxCues AllCues;
    AllCues.SetGlobalTimecodeScale(TIMECODE_SCALE);

    CChapters Chapters(TitleInfo,FormatInfo);

    // start render
    FileSegment.WriteHead(File, 8);

    KaxSeekHead & MetaSeek = AddNewChild<KaxSeekHead>(FileSegment);

    KaxSeek* seek_infos = AddSeekEntry<KaxInfo>(MetaSeek);
    KaxSeek* seek_tracks = AddSeekEntry<KaxTracks>(MetaSeek);
    KaxSeek* seek_cues = AddSeekEntry<KaxCues>(MetaSeek);
    KaxSeek* seek_chap = AddSeekEntry<KaxChapters>(MetaSeek);
    KaxSeek* seek_att = AddSeekEntry<KaxAttachments>(MetaSeek);
    KaxSeek* seek_tags = AddSeekEntry<KaxTags>(MetaSeek);

    CNZ(MetaSeek.Render(File));

    MkvTitleInfo    titleInfo;

    memset(&titleInfo,0,sizeof(titleInfo));
    TitleInfo->GetMkvTitleInfo(&titleInfo);

    // fill the mandatory Info section
    KaxInfo & MyInfos = GetChild<KaxInfo>(FileSegment);

    GetChild<EbmlUInteger,KaxTimecodeScale>(MyInfos) = TIMECODE_SCALE;
    GetChild<EbmlFloat,KaxDuration>(MyInfos) = (double)0;

    GetChild<EbmlUnicodeString,KaxMuxingApp>(MyInfos) = GetLibraryVersionString();
    GetChild<EbmlUnicodeString,KaxWritingApp>(MyInfos) = UTF8string(WritingApp);

    if (titleInfo.name)
    {
        GetChild<EbmlUnicodeString,KaxTitle>(MyInfos) = UTF8string(titleInfo.name);
    }

    uint8_t SegmentUid[16];
    for (unsigned int i=0;i<16;i++)
    {
        SegmentUid[i] = lgpl_get_random_byte();
    }
    uint64_t mtime = libmkv_time();
    GetChild<EbmlDate,KaxDateUTC>(MyInfos).SetEpochDate(mtime);
    GetChild<EbmlBinary,KaxSegmentUID>(MyInfos).CopyBuffer(SegmentUid, 16);

    RenderVoid(File,FormatInfo->debug.evoid[0]);
    CNZ(MyInfos.Render(File,true));


    // tracks
    KaxTracks & MyTracks = GetChild<KaxTracks>(FileSegment);

    std::vector<MyMkvTrackInfo> track_info;
    std::vector<KaxTrackEntry*> tracks;

    track_info.resize(Input->MkvGetStreamCount());
    tracks.resize(Input->MkvGetStreamCount());

    for (unsigned int i=0;i<Input->MkvGetStreamCount();i++)
    {
        MkvTrackInfo *ti;
        KaxTrackEntry *cur_track;

        ti = & track_info[i].info;
        track_info[i].duration_pos=0;
        track_info[i].refs[0]=BAD_TIMECODE;
        track_info[i].refs[1]=BAD_TIMECODE;
        track_info[i].stat_frames=0;
        track_info[i].stat_bytes=0;
        track_info[i].stat_bytes_out=0;

        memset(ti,0,sizeof(*ti));
        if (false==Input->MkvGetStream(i)->UpdateTrackInfo(ti))
        {
            throw mkv_error_exception("UpdateTrackInfo failed");
        }

        cur_track = & AddNewChild<KaxTrackEntry>(MyTracks);
        cur_track->SetGlobalTimecodeScale(TIMECODE_SCALE);

        tracks[i] = cur_track;

        GetChild<EbmlUInteger,KaxTrackNumber>(cur_track) = (i+1);
        GetChild<EbmlUInteger,KaxTrackUID>(cur_track) = (i+1);
        if (FormatInfo->debug.compatFlags&1)
        {
            GetChild<EbmlUInteger,KaxTrackUID>(cur_track).SetSizeLength(4);
        }

        track_type ttype;
        switch(ti->type)
        {
        case mttVideo : ttype = track_video; break;
        case mttAudio : ttype = track_audio; break;
        case mttSubtitle : ttype = track_subtitle; break;
        default: throw mkv_error_exception("bad track type");
        }

        GetChild<EbmlUInteger,KaxTrackType>(cur_track) = ttype;

        GetChild<EbmlUInteger,KaxTrackFlagDefault>(cur_track) = (0!=(ti->mkv_flags&MKV_TRACK_FLAG_DEFAULT))?1:0;
        GetChild<EbmlUInteger,KaxTrackFlagDefault>(cur_track).ForceNoDefault();

        if (0!=(ti->mkv_flags&MKV_TRACK_FLAG_FORCED))
        {
            GetChild<EbmlUInteger,KaxTrackFlagForced>(cur_track) = 1;
        }

        if (NULL!=ti->lang)
        {
            char langBuf[4];
            strcpy(langBuf,ti->lang);
            if (FormatInfo->profile.useISO639Type2T)
            {
                lgpl_iso6392T(langBuf);
            }
            GetChild<EbmlString,KaxTrackLanguage>(cur_track) = langBuf;
            GetChild<EbmlString,KaxTrackLanguage>(cur_track).ForceNoDefault();
        } else {
            if (FormatInfo->debug.compatFlags&1)
            {
                GetChild<EbmlString,KaxTrackLanguage>(cur_track) = "und";
            }
        }

        GetChild<EbmlString,KaxCodecID>(cur_track) = ti->codec_id;

        if (0!=ti->codec_private_size)
        {
            track_info[i].codec_private = & GetChild<EbmlBinary,KaxCodecPrivate>(cur_track);
            track_info[i].codec_private->CopyBuffer(ti->codec_private,ti->codec_private_size);
        } else {
            track_info[i].codec_private = NULL;
        }

        if (0!=ti->default_duration)
        {
            GetChild<EbmlUInteger,KaxTrackDefaultDuration>(cur_track) = TimecodeFromClock(ti->default_duration);
        }

        GetChild<EbmlUInteger,KaxTrackMinCache>(cur_track) = ti->min_cache;

        if ( (NULL!=ti->name) && (0==(FormatInfo->debug.compatFlags&2)) )
        {
            UTFstring tmp;
            tmp = UTF8string(ti->name);
            GetChild<EbmlUnicodeString,KaxTrackName>(cur_track) = tmp;
        }

        GetChild<EbmlUInteger,KaxTrackFlagLacing>(cur_track) = (0!=(ti->mkv_flags&MKV_TRACK_FLAG_LACING))?1:0;
        GetChild<EbmlUInteger,KaxTrackFlagLacing>(cur_track).ForceNoDefault();

        if (mttVideo==ti->type)
        {
            KaxTrackVideo*  vid_track = & GetChild<KaxTrackVideo>(*cur_track);

            GetChild<EbmlUInteger,KaxVideoPixelWidth>(vid_track) = ti->u.video.pixel_h;
            GetChild<EbmlUInteger,KaxVideoPixelHeight>(vid_track) = ti->u.video.pixel_v;

            GetChild<EbmlUInteger,KaxVideoDisplayWidth>(vid_track) = ti->u.video.display_h;
            GetChild<EbmlUInteger,KaxVideoDisplayHeight>(vid_track) = ti->u.video.display_v;
            if (FormatInfo->debug.compatFlags&1)
            {
                GetChild<KaxVideoDisplayWidth>(*vid_track).SetSizeLength(3);
                GetChild<KaxVideoDisplayHeight>(*vid_track).SetSizeLength(3);
            }

            GetChild<EbmlUInteger,KaxVideoDisplayUnit>(vid_track) = 0;

            if (ti->u.video.stereo_mode!=0)
            {
                GetChild<EbmlUInteger,KaxVideoStereoMode>(vid_track) = ti->u.video.stereo_mode;
            }

            if (ti->u.video.fps_n!=0)
            {
                char fps_string[128];

                fps_string[sizeof(fps_string)-1]=0;

                if (ti->u.video.fps_d==1)
                {
                    sprintf_s(fps_string,sizeof(fps_string),"%d",ti->u.video.fps_n);
                }
                else
                {
                    int fps_1=ti->u.video.fps_n/ti->u.video.fps_d;
                    int64_t fps_2=( ( ((int64_t)ti->u.video.fps_n) * 10000) / ti->u.video.fps_d) - (((int64_t)fps_1)*10000);


                    while( ((fps_2%10)==0) && (fps_2!=0) )
                    {
                        fps_2/=10;
                    }

                    sprintf_s(fps_string,sizeof(fps_string),"%d.%d (%d/%d)",fps_1,((int)fps_2),ti->u.video.fps_n,ti->u.video.fps_d);
                }
            }
        }

        if (mttAudio==ti->type)
        {
            KaxTrackAudio*  aud_track = & GetChild<KaxTrackAudio>(*cur_track);

            GetChild<EbmlFloat,KaxAudioSamplingFreq>(aud_track) = ti->u.audio.sample_rate;
            GetChild<EbmlUInteger,KaxAudioChannels>(aud_track) = ti->u.audio.channels_count;
            if (0!=ti->u.audio.bits_per_sample)
            {
                if ( (!(FormatInfo->debug.compatFlags&1)) || strcmp(ti->codec_id,"A_DTS") )
                {
                    GetChild<EbmlUInteger,KaxAudioBitDepth>(aud_track) = ti->u.audio.bits_per_sample;
                }
            }
        }

        if (track_info[i].info.profile_track_info)
        {
            track_info[i].compression_type=track_info[i].info.profile_track_info->compressionType;
            track_info[i].compression_level=track_info[i].info.profile_track_info->compressionLevel;
        } else {
            track_info[i].compression_type=MKV_TRACK_COMPRESSION_NONE;
        }

        if ( (!ti->header_comp_size) && (track_info[i].compression_type==MKV_TRACK_COMPRESSION_HEADERS) )
        {
            track_info[i].compression_type=MKV_TRACK_COMPRESSION_NONE;
        }

        if (track_info[i].compression_type!=MKV_TRACK_COMPRESSION_NONE)
        {
            KaxContentEncodings &cs = GetChild<KaxContentEncodings>(*cur_track);
            KaxContentEncoding &ce = GetChild<KaxContentEncoding>(cs);
            KaxContentCompression &cc = GetChild<KaxContentCompression>(ce);

            GetChild<EbmlUInteger,KaxContentCompAlgo>(cc) = track_info[i].compression_type;
            GetChild<EbmlUInteger,KaxContentCompAlgo>(cc).ForceNoDefault();

            if (ti->header_comp_size)
            {
                GetChild<EbmlBinary,KaxContentCompSettings>(cc).CopyBuffer(ti->header_comp_data,ti->header_comp_size);
            }
        }
    }

    CNZ(MyTracks.Render(File));

    Chapters.Render(File,seek_chap,FileSegment);
    RenderVoid(File,FormatInfo->debug.evoid[1]);

    MkvAttachmentInfo   ainfo;
    if (TitleInfo->GetAttachmentCount())
    {
        KaxAttachments & MyAttachments = GetChild<KaxAttachments>(FileSegment);
        for (unsigned int i=0;i<TitleInfo->GetAttachmentCount();i++)
        {
            KaxAttached *pa;

            TitleInfo->GetAttachmentInfo(&ainfo,i);

            pa = & AddNewChild<KaxAttached>(MyAttachments);
            GetChild<EbmlUnicodeString,KaxFileName>(pa) = UTF8string(ainfo.name);
            GetChild<EbmlString,KaxMimeType>(pa) = ainfo.mime_type;
            GetChild<EbmlUInteger,KaxFileUID>(pa) = 1+i;
            GetChild<EbmlBinary,KaxFileData>(pa).CopyBuffer((const binary *)ainfo.data,ainfo.size);
        }
        CNZ(MyAttachments.Render(File));
        UpdateSeekEntry(seek_att,File,MyAttachments,FileSegment);
        MyAttachments.RemoveAll();
    } else {
        VoidElement(seek_att,File);
    }

    // finish all meta info
    UpdateSeekEntry(seek_infos,File,MyInfos,FileSegment);
    UpdateSeekEntry(seek_tracks,File,MyTracks,FileSegment);
    MyTracks.RemoveAll();

    KaxCluster *curr_cluster=NULL,*prev_cluster=NULL;
    int64_t cluster_timecode=0;
    int64_t max_duration=0;
    uint64_t prg_val=0;
    int64_t frame_end;
    unsigned int clusterlen = (track_info.size() > 64) ? 5 : 4;

    while(true)
    {
        int64_t min_timecode1,min_timecode2;
        unsigned int stream_id1,stream_id2,stream_id;
        IMkvFrameSource *stream;
        int clusterFlags;
        bool new_cluster;

        min_timecode1=min_timecode2=BAD_TIMECODE;
        clusterFlags = 0;

        stream_id = stream_id1 = stream_id2 = 0;
        for (unsigned int i=0;i<tracks.size();i++)
        {
            bool force_fetch,type2;
            unsigned int frames_scan;

            force_fetch = ( (track_info[i].info.type==mttVideo) || (track_info[i].info.type==mttAudio) );
            type2 = true;

            if (false==Input->MkvGetStream(i)->FetchFrames(1,force_fetch))
            {
                throw mkv_error_exception("Error while reading input");
            }

            frames_scan = (track_info[i].info.type==mttVideo)?16:1;
            if (Input->MkvGetStream(i)->GetAvailableFramesCount()<frames_scan)
            {
                frames_scan = Input->MkvGetStream(i)->GetAvailableFramesCount();
            }

            if ( (track_info[i].info.type!=mttSubtitle) && frames_scan)
            {
                if (Input->MkvGetStream(i)->PeekFrame(0)->cluster_start())
                {
                    type2 = false;
                    clusterFlags |= 1;
                } else {
                    clusterFlags |= 2;
                }
            }

            for (unsigned int j=0;j<frames_scan;j++)
            {
                int64_t dts;

                IMkvChunk* tf = Input->MkvGetStream(i)->PeekFrame(j);

                if (tf->timecode==-1) throw mkv_error_exception("Frame not timestamped");
                dts = ScaleTimecode(TimecodeFromClock(tf->timecode)+(TIMECODE_SCALE/2));

                if (dts<min_timecode1)
                {
                    min_timecode1 = dts;
                    stream_id1 = i;
                }
                if ( (dts<min_timecode2) && type2 )
                {
                    min_timecode2 = dts;
                    stream_id2 = i;
                }
            }
            if (track_info[i].compression_type==MKV_TRACK_COMPRESSION_ZLIB)
            {
                frames_scan = Input->MkvGetStream(i)->GetAvailableFramesCount();
                for (unsigned int k=0;k<frames_scan;k++)
                {
                    CNZ(Input->MkvGetStream(i)->PeekFrame(k)->compress_start(
                        track_info[i].compression_type,
                        track_info[i].compression_level));
                }
            }
        }
        switch(clusterFlags)
        {
        case 0:
            stream_id = 0;
            new_cluster = false;
            break;
        case 1:
            if (Input->MkvGetStream(0)->GetAvailableFramesCount())
            {
                stream_id = 0;
            } else {
                stream_id = stream_id1;
            }
            new_cluster = true;
            break;
        case 2:
            stream_id = stream_id1;
            new_cluster = false;
            break;
        case 3:
            stream_id = stream_id2;
            new_cluster = false;
            break;
        default:
            throw mkv_error_exception("clusterFlags");
            break;
        }

        if (new_cluster)
        {
            for (unsigned int i=0;i<tracks.size();i++)
            {
                if (Input->MkvGetStream(i)->GetAvailableFramesCount()>0)
                {
                    Input->MkvGetStream(i)->PeekFrame(0)->flags &= (~MKV_CHUNK_CLUSTER_START);
                }
            }
        };

        if (NULL==curr_cluster)
        {
            new_cluster=true;
            stream_id=0;
        }

        // update UI
        if (0==stream_id)
        {
            lgpl_update_current_progress(Input,&prg_val);
        }

        IMkvChunk*  frame,*prev_frame;

        stream = Input->MkvGetStream(stream_id);
        if (stream->GetAvailableFramesCount()==0)
        {
            // no more frames
            break;
        }
        frame = stream->PeekFrame(0);

        if (frame->chapter_mark())
        {
            Chapters.AddChapterMark(frame->timecode,File);
        }

        if (new_cluster)
        {
            if (NULL!=curr_cluster)
            {
                FinishCluster(curr_cluster,&File,&FileSegment);
            }
            prev_cluster = curr_cluster;
            curr_cluster = & AddNewChild<KaxCluster>(FileSegment);
            curr_cluster->SetSizeInfinite();
            curr_cluster->SetParent(FileSegment);

            if (FormatInfo->debug.compatFlags&1)
            {
                CNZ(curr_cluster->WriteHead(File, 3));
            } else {
                CNZ(curr_cluster->WriteHead(File, clusterlen));
            }

            cluster_timecode = GetClusterTimecode(Input);

            curr_cluster->InitTimecode(ScaleTimecode(TimecodeFromClock(cluster_timecode)),TIMECODE_SCALE);

            GetChild<EbmlUInteger,KaxClusterTimecode>(curr_cluster) = ScaleTimecode(TimecodeFromClock(cluster_timecode));
            GetChild<KaxClusterTimecode>(*curr_cluster).Render(File);

            if (NULL!=prev_cluster)
            {
                GetChild<EbmlUInteger,KaxClusterPrevSize>(curr_cluster) = curr_cluster->GetElementPosition() - prev_cluster->GetElementPosition();
                GetChild<KaxClusterPrevSize>(*curr_cluster).Render(File);
            }
        }

        if (frame->get_size()==0)
        {
            CNZ(frame->compress_wait());
            stream->PopFrame();
            continue;
        }

        // have a frame and a cluster

        // update entire file duration
        frame_end=frame->timecode+frame->duration;
        if (frame_end>max_duration) max_duration = frame_end;

        unsigned int lacing_frames,fetch_frames,frames_count,same_frame_size;
        bool frame_keyframe,all_same;

        if (0!=(track_info[stream_id].info.mkv_flags&MKV_TRACK_FLAG_LACING))
        {
            if (FormatInfo->debug.compatFlags&1)
            {
                lacing_frames = 8;
            } else {
                lacing_frames = (unsigned int)(108000000 / track_info[stream_id].info.default_duration);
            }
            if (lacing_frames>180)
            {
                lacing_frames = 180;
            }
            if (lacing_frames==0)
            {
                lacing_frames = 1;
                fetch_frames = 1;
            } else {
                fetch_frames=lacing_frames+1;
            }
        } else {
            lacing_frames = 1;
            fetch_frames = 1;
        }

        frames_count = fetch_frames;
        if (frames_count>lacing_frames) frames_count=lacing_frames;

        MKV_ASSERT(stream->GetAvailableFramesCount()>0);

        if (frames_count>1)
        {
            CNZ(frame->compress_start(track_info[stream_id].compression_type,track_info[stream_id].compression_level));
            CNZ(frame->compress_wait());
        }

        frame_keyframe = frame->keyframe();
        same_frame_size = frame->get_size();
        all_same = true;
        for (unsigned int i=1;i<frames_count;i++)
        {
            prev_frame = frame;

            if (stream->GetAvailableFramesCount()<(i+1))
            {
                if (false==stream->FetchFrames(i+1,true))
                {
                    throw mkv_error_exception("Error while reading input");
                }
                if (stream->GetAvailableFramesCount()<(i+1))
                {
                    frames_count=i;
                    break;
                }
            }

            frame=stream->PeekFrame(i);
            if ( (frame->keyframe()!=frame_keyframe) || frame->cluster_start() )
            {
                frames_count=i;
                break;
            }
            if (frame->duration!=track_info[stream_id].info.default_duration)
            {
                frames_count=i;
                break;
            }
            if ( !CompareTimecode( prev_frame->timecode+prev_frame->duration , frame->timecode ))
            {
                frames_count=i;
                break;
            }

            CNZ(frame->compress_start(track_info[stream_id].compression_type,track_info[stream_id].compression_level));
            CNZ(frame->compress_wait());

            if (frame->get_size()!=same_frame_size)
            {
                all_same=false;
            }
        }

        frame = stream->PeekFrame(0);

        KaxBlockGroup       *blkg;
        KaxSimpleBlock      *blks;
        KaxInternalBlock    *blki;
        uint64_t            refs[2];

        blkg=NULL;
        blks=NULL;
        if (frame->old_block())
        {
            blkg = new KaxBlockGroup();
            blki = &(KaxInternalBlock&)(*blkg);
            blkg->SetParent(*curr_cluster);

            frames_count=1;

            DataBuffer* mkv_buffer = GetDataBuffer(frame,&track_info[stream_id]);
            CNZ(blkg->AddFrame( * (tracks[stream_id]) , TimecodeFromClock(frame->timecode)+(TIMECODE_SCALE/2) , *mkv_buffer));
            track_info[stream_id].UpdateStat(frame);

            if (frame->auto_duration())
            {
                blkg->SetBlockDuration( AUTO_DURATION_TIMECODE );
            } else {
                if ( (frame->duration != track_info[stream_id].info.default_duration) || (track_info[stream_id].info.type==mttVideo) )
                {
                    blkg->SetBlockDuration( TimecodeFromClock(frame->duration)+(TIMECODE_SCALE/2) );
                }
            }

            if (track_info[stream_id].info.type==mttVideo)
            {
                if (frame->discardable())
                {
                    refs[0]=track_info[stream_id].refs[0];
                    refs[1]=track_info[stream_id].refs[1];
                } else {
                    if (!frame->keyframe())
                    {
                        refs[0]=BAD_TIMECODE;
                        refs[1]=track_info[stream_id].refs[1];
                    } else {
                        refs[0]=BAD_TIMECODE;
                        refs[1]=BAD_TIMECODE;
                    }
                }
                if (frame->keyframe())
                {
                    track_info[stream_id].refs[0]=BAD_TIMECODE;
                    track_info[stream_id].refs[1]=frame->timecode;
                } else {
                    if (!frame->discardable())
                    {
                        track_info[stream_id].refs[0]=track_info[stream_id].refs[1];
                        track_info[stream_id].refs[1]=frame->timecode;
                    }
                }
                for (int nr=0;nr<2;nr++)
                {
                    if (refs[nr]!=BAD_TIMECODE)
                    {
                        KaxReferenceBlock & theRef = AddNewChild<KaxReferenceBlock>(*blkg);
                        int64_t refTimecode = ScaleTimecode(TimecodeFromClock(refs[nr])+(TIMECODE_SCALE/2));
                        refTimecode -= ScaleTimecode(TimecodeFromClock(frame->timecode)+(TIMECODE_SCALE/2));
                        theRef.SetReferencedTimecode(refTimecode);
                        theRef.SetParentBlock(*blkg);
                    }
                }
            }

            CNZ(blkg->Render(File));

            if (track_info[stream_id].duration_pos)
            {
                uint64_t duration = TimecodeFromClock(frame->timecode) - track_info[stream_id].duration_time;
                if (duration < AUTO_DURATION_TIMECODE)
                {
                    OverwriteIntegerData(File,ScaleTimecode(duration),track_info[stream_id].duration_pos,track_info[stream_id].duration_size);
                }
                track_info[stream_id].duration_pos = 0;
            }

            if (frame->auto_duration())
            {
                KaxBlockDuration & myDuration = GetChild<KaxBlockDuration>(*blkg);
                track_info[stream_id].duration_pos  = myDuration.GetElementPosition() + myDuration.HeadSize();
                track_info[stream_id].duration_size = (unsigned int) myDuration.GetSize();
                track_info[stream_id].duration_time = TimecodeFromClock(frame->timecode);
            }

        } else {
            blks = new KaxSimpleBlock();
            blki = &(KaxInternalBlock&)(*blks);
            blks->SetParent(*curr_cluster);
            blks->SetKeyframe(frame->keyframe());
            blks->SetDiscardable(frame->discardable());

            for (unsigned int i=0;i<frames_count;i++)
            {
                frame=stream->PeekFrame(i);

                DataBuffer* mkv_buffer = GetDataBuffer(frame,&track_info[stream_id]);
                CNZ(blks->AddFrame( * (tracks[stream_id]) , TimecodeFromClock(frame->timecode)+(TIMECODE_SCALE/2) , *mkv_buffer , all_same ? LACING_FIXED : LACING_EBML ));

                track_info[stream_id].UpdateStat(frame);
            }
            CNZ(blks->Render(File));
        }

        if (new_cluster)
        {
            KaxCuePoint *cp = & AddNewChild<KaxCuePoint>(AllCues);
            cp->PositionSet(*blki,blkg,TIMECODE_SCALE);
        }

        delete blks;
        delete blkg;

        for (unsigned int i=0;i<frames_count;i++)
        {
            stream->PopFrame();
        }
    }
    FinishCluster(curr_cluster,&File,&FileSegment);

    // update total duration
    GetChild<EbmlFloat,KaxDuration>(MyInfos) = (double) ScaleTimecode(TimecodeFromClock(max_duration));
    GetChild<KaxDuration>(MyInfos).OverwriteData(File,true);

    CNZ(AllCues.Render(File));
    UpdateSeekEntry(seek_cues,File,AllCues,FileSegment);

    Chapters.Finalize(File,seek_chap,max_duration);

    for (unsigned int i=1;i<Input->MkvGetStreamCount();i++)
    {

        if (track_info[i].stat_frames==0)
        {
            VoidElement(tracks[i],File);
            my_world()->uc_emptytrack(Input,i,&track_info[i].info);
            continue;
        }

        if (track_info[i].codec_private)
        {
            MkvTrackInfo ti;


            memset(&ti,0,sizeof(ti));
            if (false==Input->MkvGetStream(i)->UpdateTrackInfo(&ti))
            {
                throw mkv_error_exception("UpdateTrackInfo failed");
            }

            if (track_info[i].codec_private->GetSize()==ti.codec_private_size)
            {
                if (0!=memcmp(track_info[i].codec_private->GetBuffer(),
                    ti.codec_private,ti.codec_private_size))
                {

                    memcpy(track_info[i].codec_private->GetBuffer(),
                        ti.codec_private,ti.codec_private_size);

                    track_info[i].codec_private->OverwriteData(File,true);
                }
            }
        }
    }

    // tags
    KaxTags & MyTags = GetChild<KaxTags>(FileSegment);
    for (unsigned int i=0;i<Input->MkvGetStreamCount();i++)
    {
        if (track_info[i].stat_frames==0) continue;

        track_info[i].RenderStat(MyTags,i+1,mtime,WritingApp);
    }
    CNZ(MyTags.Render(File));
    UpdateSeekEntry(seek_tags,File,MyTags,FileSegment);

    // end

    // Set the correct size for the segment.
    CNZ(FileSegment.ForceSize(File.getFilePointer() - ( FileSegment.GetElementPosition() + FileSegment.HeadSize() ) ));
    CNZ(FileSegment.OverwriteHead(File));
}

static void FinishCluster(KaxCluster *cluster,IOCallback *File,KaxSegment *Segment)
{
    // add position info
    GetChild<EbmlUInteger,KaxClusterPosition>(cluster) =
        cluster->GetElementPosition() - ( Segment->GetElementPosition() + Segment->HeadSize() );
    GetChild<KaxClusterPosition>(*cluster).Render(*File);

    // correct size
    CNZ(cluster->ForceSize(File->getFilePointer() - ( cluster->GetElementPosition() + cluster->HeadSize() ) ));
    CNZ(cluster->OverwriteHead(*File));
}

extern "C"
bool __cdecl MkvCreateFile(IMkvWriteTarget* Output,IMkvTrack *Input,const char *WritingApp,IMkvTitleInfo* TitleInfo,MkvFormatInfo* FormatInfo) throw()
{
    CEbmlWrite  wrt(Output);
    try
    {
        MkvCreateFileInternal(wrt,Input,TitleInfo,FormatInfo,WritingApp);
        return true;
    } catch(std::exception &Ex)
    {
        // no memory allocations here
        if (Ex.what()[0]!='$')
        {
            char tstr[512];
            strcpy(tstr,"Exception: ");
            strncat(tstr,Ex.what(),sizeof(tstr)-1);
            tstr[sizeof(tstr)-1]=0;
            lgpl_trace(tstr);
        }
    } catch(...)
    {
        lgpl_trace("Exception: unknown");
    }
    return false;
}

