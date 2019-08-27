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
#include <lgpl/aproxy.h>
#include <strings.h>

void CApClient::SignalExit()
{
    ExecCmd(apCallSignalExit);
    m_mem->exit = 1;
}

void CApClient::CancelAllJobs()
{
    ExecCmd(apCallCancelAllJobs);
}

void CApClient::OnIdle()
{
    ExecCmd(apCallOnIdle);
}

bool CApClient::UpdateAvailableDrives()
{
    ExecCmd(apCallUpdateAvailableDrives);
    return (m_mem->args[0]!=0);
}

bool CApClient::CloseDisk(unsigned int EjectId)
{
    m_mem->args[0]=EjectId;
    ExecCmd(apCallCloseDisk);
    return (m_mem->args[0]!=0);
}

int CApClient::GetSettingInt(ApSettingId Id)
{
    m_mem->args[0]=Id;
    ExecCmd(apCallGetSettingInt);
    return (int)(m_mem->args[0]);
}

const utf16_t* CApClient::GetSettingString(ApSettingId Id)
{
    m_mem->args[0]=Id;
    ExecCmd(apCallGetSettingString);
    if (0==m_mem->args[0]) return NULL;
    return (utf16_t*)(m_mem->strbuf);
}

void CApClient::SetSettingInt(ApSettingId Id,int Value)
{
    m_mem->args[0]=Id;
    m_mem->args[1]=Value;
    ExecCmd(apCallSetSettingInt);
}

void CApClient::SetSettingString(ApSettingId Id,const utf16_t* Value)
{
    m_mem->args[0]=Id;
    if (NULL==Value)
    {
        m_mem->args[1]=0;
    } else {
        m_mem->args[1]=1;
        memcpy((void*)m_mem->strbuf,Value,(utf16len(Value)+1)*sizeof(utf16_t));
    }
    ExecCmd(apCallSetSettingString);
}

bool CApClient::SaveSettings()
{
    ExecCmd(apCallSaveSettings);
    return (m_mem->args[0]!=0);
}

const utf16_t* CApClient::GetAppString(unsigned int Id)
{
    static const utf16_t zero = 0;
    if (m_shutdown) return &zero;
    m_mem->args[0] = Id;
    ExecCmd(apCallAppGetString);
    if (m_mem->args[0]==0)
    {
        return NULL;
    } else {
        return (utf16_t*)m_mem->strbuf;
    }
}

