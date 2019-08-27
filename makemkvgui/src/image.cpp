/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2019 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#include "qtapp.h"
#include "qtgui.h"
#include "image_defs.h"
#include "image_data.h"

const uint32_t* get_image_data();
static const uint32_t* image_data = NULL;
static const QIcon* anim_icons[(AP_IMG_ANIMATION0102H-AP_IMG_ANIMATION0102)*2];

QIcon* createIconPixmaps(unsigned int firstId,unsigned int count)
{
    QIcon* icon = new QIcon();
    for (unsigned int i=0;i<count;i++)
    {
        QImage* image;

        image = getBuiltinImage(firstId+i);

        if (!image) continue;

        icon->addPixmap(QPixmap::fromImage(*image));
    }
    return icon;
}

QImage* getBuiltinImage(unsigned int id)
{
    if (id>=IMAGE_TABLE_SIZE) return NULL;

    if (!image_data)
    {
        image_data = get_image_data();
    }

    const uint32_t* data = image_data + image_table[id];

    return new QImage((const uchar*)(data+4),data[0],data[1],IMAGE_TABLE_FORMAT);
}

void createAnimIcons()
{
    for (unsigned int i=AP_IMG_ANIMATION0102;i<AP_IMG_ANIMATION0102H;i++)
    {
        if (image_table[i])
        {
            QImage* image = getBuiltinImage(i);
            QPixmap pixmap = QPixmap::fromImage(*image);
#ifdef IMAGE_TABLE_BIG
            QImage* imageb = getBuiltinImage(i+(AP_IMG_ANIMATION0102H-AP_IMG_ANIMATION0102));
            QPixmap pixmapb = QPixmap::fromImage(*imageb);
#endif
            QIcon* icon1 = new QIcon();
            icon1->addPixmap(pixmap,QIcon::Normal);
            icon1->addPixmap(pixmap,QIcon::Disabled);
#ifdef IMAGE_TABLE_BIG
            icon1->addPixmap(pixmapb,QIcon::Normal);
            icon1->addPixmap(pixmapb,QIcon::Disabled);
#endif

            QIcon* icon2 = new QIcon();
            icon2->addPixmap(pixmap,QIcon::Normal);
#ifdef IMAGE_TABLE_BIG
            icon2->addPixmap(pixmapb,QIcon::Normal);
#endif
            anim_icons[(i-AP_IMG_ANIMATION0102)*2+0] = icon1;
            anim_icons[(i-AP_IMG_ANIMATION0102)*2+1] = icon2;
        } else {
            anim_icons[(i-AP_IMG_ANIMATION0102)*2+0] = NULL;
            anim_icons[(i-AP_IMG_ANIMATION0102)*2+1] = NULL;
        }
    }
}

const QIcon* getAnimIcon(unsigned int id,unsigned int id2)
{
    if ( (id<AP_IMG_ANIMATION0102) || (id>=AP_IMG_ANIMATION0102H) ) return NULL;
    return anim_icons[(id-AP_IMG_ANIMATION0102)*2+id2];
}

QSize adjustIconSize(QSize size,unsigned int min)
{
    unsigned int s = size.height();

    if ( (s>=14) && (s<29)   ) s = 16;
    if ( (s>=29) && (s<58)   ) s = 32;
    if ( (s>=58) && (s<115)  ) s = 64;
    if ( (s>=115) && (s<230) ) s = 128;
    if ( (s>=230) && (s<460) ) s = 256;
    if (s>=460) s = 512;

    if (s<min) s=min;

    return QSize(s,s);
}
