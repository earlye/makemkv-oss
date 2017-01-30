/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2016 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#include <lgpl/aproxy.h>
#include <lgpl/sstring.h>

void CApClient::SetUiNotifier(CApClient::INotifier* Notifier)
{
    m_Ui = Notifier;
}

CApClient::CApClient()
{
    m_Ui = NULL;
    m_debug = 0;
    m_mem = NULL;
}

void CApClient::AppExiting()
{
    if (NULL!=m_mem)
    {
        m_mem->exit = 2;
    }
}

static bool CheckVersionString(const char* Version,const char* AvailableVersions)
{
    const char *p,*pe;
    size_t vl = strlen(Version);

    p=AvailableVersions;
    while(*p!=0)
    {
        if (*p=='/') p++;
        pe = strchr(p,'/');
        if (pe==NULL)
        {
            pe=p+strlen(p);
        }

        if (vl==(pe-p))
        {
            if (0==memcmp(p,Version,vl))
            {
                return true;
            }
        }
        p=pe;
    }
    return false;
}

bool CApClient::Init(char Type,const char* AppName,unsigned int* Code)
{
    char *p;
    const char *args[2];
    const char *verstr= NULL;
    int error;
    char response[512];
    uint64_t pipeh[2];

    *Code = 0;

    switch(Type)
    {
    case 'C': verstr=AP_CRE_VER; break;
    case 'G': verstr=AP_GUI_VER; break;
    default:  verstr=NULL; break;
    }
    if (NULL==verstr)
    {
        DebugFail(Type,NULL);
        return false;
    }

    if (0!=(error=ApCreatePipe(pipeh)))
    {
        DebugFail(error,NULL);
        return false;
    }

    error = ApSpawnApp(verstr,AppName,response,512);
    if (error!=0)
    {
        DebugFail(error,NULL);
        *Code = 1;
        return false;
    }

    p=response;
    args[0]=p; p=strchr(p,':'); *p=0; p++;
    args[1]=p;

    if (false==CheckVersionString(verstr,args[0]))
    {
        DebugFail(0,verstr);
        DebugFail(0,args[0]);
        *Code = 2;
        return false;
    }

    m_mem = (AP_SHMEM*) ApOpenShmem(args[1]);
    if (NULL==m_mem)
    {
        DebugFail(0,NULL);
        return false;
    }

    if (false==ApOpenSgrp(&m_sem,(uint64_t*)m_mem->args))
    {
        DebugFail(0,NULL);
        return false;
    }

    m_mem->args[24]=pipeh[0];
    m_mem->args[25]=pipeh[1];
    m_mem->start=1;
    m_shutdown = false;

    DebugFail(0,"ok");
    return true;
}

AP_CMD CApClient::FatalCommError()
{
    DebugFail(m_mem->abort_nomem,NULL);
    m_shutdown = true;
    if (m_mem->abort_nomem) return apBackOutOfMem;
    return apBackFatalCommError;
}

AP_CMD CApClient::Transact(AP_CMD cmd)
{
    if (m_shutdown)
    {
        DebugFail(cmd,NULL);
        return apReturn;
    }

    DebugFail(cmd,"in");

    m_mem->cmd = cmd;

    if (m_mem->abort_nomem) return FatalCommError();

    if (false==ApSemInc(&m_sem,m_sem.a_id))
    {
        DebugFail(cmd,NULL);
        return FatalCommError();
    }

    if (m_mem->abort_nomem) return FatalCommError();

    if (false==ApSemDec(&m_sem,m_sem.b_id))
    {
        DebugFail(cmd,NULL);
        return FatalCommError();
    }

    if (m_mem->abort_nomem)
    {
        DebugFail(cmd,NULL);
        return FatalCommError();
    }

    DebugFail((uint32_t)m_mem->cmd,"out");

    return (AP_CMD)(uint32_t)m_mem->cmd;
}

void CApClient::ExecCmd(AP_CMD cmd)
{
    AP_CMD r;

    while(apReturn!=(r=Transact(cmd)))
    {
        // process command
        switch(r)
        {
        case apNop:
            break;
        case apBackEnterJobMode:
            m_Ui->EnterJobMode((unsigned int) m_mem->args[0]);
            break;
        case apBackLeaveJobMode:
            m_Ui->LeaveJobMode();
            break;
        case apBackExit:
            m_shutdown = true;
            m_Ui->ExitApp();
            return;
            break;
        case apBackFatalCommError:
            m_Ui->ReportUiMessage(APP_IFACE_FATAL_COMM,AP_UIMSG_BOXERROR,AP_UI_STRING(APP_IFACE_FATAL_COMM));
            m_Ui->ExitApp();
            return;
            break;
        case apBackOutOfMem:
            m_Ui->ReportUiMessage(APP_IFACE_FATAL_MEM,AP_UIMSG_BOXERROR,AP_UI_STRING(APP_IFACE_FATAL_MEM));
            m_Ui->ExitApp();
            return;
            break;
        case apBackUpdateDrive:
            const utf16_t* p_str,*drv_name,*dsk_name,*dev_name;

            p_str = (const utf16_t*) m_mem->strbuf;
            if (m_mem->args[1]==0)
            {
                drv_name = NULL;
            } else {
                drv_name = p_str;
                p_str += (utf16len(p_str)+1);
            }
            if ((m_mem->args[4]&1)==0)
            {
                dsk_name = NULL;
            } else {
                dsk_name = p_str;
                p_str += (utf16len(p_str)+1);
            }
            if ((m_mem->args[4]&2)==0)
            {
                dev_name = NULL;
            } else {
                dev_name = p_str;
                p_str += (utf16len(p_str)+1);
            }
            m_Ui->UpdateDrive(
                (unsigned int) m_mem->args[0],
                drv_name,
                (AP_DriveState)(m_mem->args[2]),
                dsk_name,
                dev_name,
                (AP_DiskFsFlags)(m_mem->args[5]),
                ((char*)m_mem)+m_mem->args[7],
                (unsigned int) m_mem->args[6]
                );
            break;
        case apBackSetTotalName:
            m_Ui->SetTotalName( (unsigned long) m_mem->args[0] );
            break;
        case apBackUpdateLayout:
            unsigned int n_size;
            unsigned long c_name;
            unsigned int name_index;
            unsigned int flags;
            unsigned long names[AP_Progress_MaxLayoutItems];
            c_name = (unsigned long) m_mem->args[0];
            name_index = (unsigned int) m_mem->args[1];
            flags = (unsigned int) m_mem->args[2];
            n_size = (unsigned int) m_mem->args[3];
            for (unsigned int i=0;i<n_size;i++)
            {
                names[i] = (unsigned long) m_mem->args[4+i];
            }
            m_Ui->UpdateLayout(c_name,name_index,flags,n_size,names);
            break;
        case apBackUpdateCurrentInfo:
            m_Ui->UpdateCurrentInfo( (unsigned int) m_mem->args[0] , (utf16_t*)m_mem->strbuf );
            break;
        case apBackReportUiMesaage:
            int rr;
            if (m_Ui)
            {
                rr = m_Ui->ReportUiMessage( (unsigned long) m_mem->args[0] , (unsigned long) m_mem->args[2] , (utf16_t*)m_mem->strbuf );
            } else {
                rr = 0;
            }
            m_mem->args[0] = rr;
            break;
        case apBackReportUiDialog:
            if (m_Ui)
            {
                const utf16_t* text[32];
                unsigned int len,count = (unsigned int) m_mem->args[2];
                if (count>32) count=32;
                const utf16_t* p = (utf16_t*)m_mem->strbuf;

                for (unsigned int i=0;i<count;i++)
                {
                    len = *p++;
                    if ((len&0x8000)==0)
                    {
                        text[i] = p;
                        p += len;
                    } else {
                        len &= 0x7fff;
                        len <<= 16;
                        len |= *p++;
                        text[i] = AppGetString(len);
                    }
                }
                m_mem->args[0] = m_Ui->ReportUiDialog((unsigned long) m_mem->args[0] , (unsigned long) m_mem->args[1] , count, text , (utf16_t*)m_mem->strbuf );
            } else {
                m_mem->args[0] = -1;
            }
            break;
        case apBackUpdateCurrentBar:
            m_Ui->UpdateCurrentBar( (unsigned int) m_mem->args[0] );
            break;
        case apBackUpdateTotalBar:
            m_Ui->UpdateTotalBar( (unsigned int) m_mem->args[0] );
            break;
        case apBackSetTitleCollInfo:
            SetTitleCollInfo(m_mem->args[0],(unsigned int) m_mem->args[1]);
            break;
        case apBackSetTitleInfo:
            SetTitleInfo((unsigned int) m_mem->args[0],m_mem->args[1],(unsigned int) m_mem->args[2],(unsigned int) m_mem->args[3],m_mem->args[4]);
            break;
        case apBackSetTrackInfo:
            SetTrackInfo((unsigned int) m_mem->args[0],(unsigned int) m_mem->args[1],m_mem->args[2]);
            break;
        case apBackSetChapterInfo:
            SetChapterInfo((unsigned int) m_mem->args[0],(unsigned int) m_mem->args[1],m_mem->args[2]);
            break;
        default:
            DebugFail(r,NULL);
            m_mem->args[0] = 0;
            break;
        }

        cmd = apClientDone;
    }
}

bool CApClient::EnableDebug(const char* LogName)
{
    m_debug = ApDebugOpen(LogName);
    return (m_debug!=0);
}

void CApClient::DebugFailRoutine(uintptr_t arg0,const char* arg1,int line)
{
    char    buffer[1024];

    if (m_debug==0) return;
    sprintf_s(buffer,sizeof(buffer),"DEBUG: %p %s %u\n",((void*)arg0),(arg1==NULL)?"(null)":arg1,line);

    ApDebugOut(m_debug,buffer);
}

