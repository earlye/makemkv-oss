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
#ifndef LIBMKV_INTERNAL_H_INCLUDED
#define LIBMKV_INTERNAL_H_INCLUDED

#include "ebml/EbmlHead.h"
#include "ebml/EbmlSubHead.h"
#include "ebml/EbmlVoid.h"
#include <ebml/EbmlString.h>
#include <ebml/EbmlVersion.h>

#include "matroska/FileKax.h"
#include "matroska/KaxSegment.h"
#include "matroska/KaxTracks.h"
#include "matroska/KaxTrackEntryData.h"
#include "matroska/KaxTrackAudio.h"
#include "matroska/KaxTrackVideo.h"
#include "matroska/KaxCluster.h"
#include "matroska/KaxClusterData.h"
#include "matroska/KaxSeekHead.h"
#include "matroska/KaxCues.h"
#include "matroska/KaxCuesData.h"
#include "matroska/KaxInfo.h"
#include "matroska/KaxInfoData.h"
#include "matroska/KaxTags.h"
#include "matroska/KaxTag.h"
#include "matroska/KaxChapters.h"
#include "matroska/KaxContentEncoding.h"
#include "matroska/KaxVersion.h"
#include "matroska/KaxBlock.h"
#include "matroska/KaxBlockData.h"
#include "matroska/KaxAttachments.h"
#include "matroska/KaxAttached.h"

#include <time.h>

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

UTFstring GetLibraryVersionString();

#ifdef _MSC_VER

static inline uint64_t libmkv_time()
{
    return _time64(NULL);
};

static inline struct tm *libmkv_gmtime_r(uint64_t atime, struct tm *result)
{
    __time64_t t = atime;
    struct tm *p = _gmtime64(&t);
    if (!p) return NULL;
    memcpy(result,p,sizeof(struct tm));
    return result;
}

#else

static inline uint64_t libmkv_time()
{
    return time(NULL);
};

static inline struct tm *libmkv_gmtime_r(uint64_t atime, struct tm *result)
{
    time_t t = atime;
    return gmtime_r(&t,result);
}

#endif

#endif // LIBMKV_INTERNAL_H_INCLUDED

