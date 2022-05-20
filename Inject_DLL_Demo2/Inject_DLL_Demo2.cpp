// Inject_DLL_Demo2.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Inject_DLL_Demo2.h"
#include <Windows.h>
#include "resource.h"
#include <TlHelp32.h>

#define WECHAT_PROCESS_NAME "WeChat.exe"

VOID InjectDll();
INT_PTR CALLBACK Dlgproc(HWND unnamedParam1, UINT unnamedParam2, WPARAM unnamedParam3, LPARAM unnamedParam4);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(MAIN), NULL, &Dlgproc);
    return 0;
}



INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg)
	{
	case WM_CLOSE:
		EndDialog(hwndDlg, NULL);
		break;
	case WM_INITDIALOG:
		MessageBox(NULL, "加载！", "提示", 0);
		break;
	case WM_COMMAND:
		if (wParam == INJECT_DLL) {
			InjectDll();
		}
		if (wParam == UN_DLL) {

		}
		break;
	}
	return FALSE;
}


//获取微信的句柄
//进程名->Pid->句柄

//获取PID
DWORD ProcessNameFindPid(LPCSTR ProcessName) {
	//获取进程快照
	HANDLE ProcessAll = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	//对比进程名称
	PROCESSENTRY32 processInfo = { 0 };
	processInfo.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		if (strcmp(ProcessName,processInfo.szExeFile) == 0)
		{
			return processInfo.th32ParentProcessID;
		}
	} while (true);
	Process32Next(ProcessAll, &processInfo);
	return 0;
}

VOID InjectDll() {

	CHAR pathStr[0x100] = { "G://Dll//WeChatTest.dll" };

	//获取微信的PID
	DWORD pid = ProcessNameFindPid(WECHAT_PROCESS_NAME);
	if (pid == 0)
	{
		MessageBox(NULL, "未找到微信程序，请检查是否启动！", "错误", 0);
		return;
	}
	HANDLE hProcess =  OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
	{
		MessageBox(NULL, "进程启动失败！", "错误", 0);
		return;
	}

	//申请内存
	LPVOID dllAdd = VirtualAllocEx(hProcess, NULL, sizeof(pathStr), MEM_COMMIT, PAGE_READWRITE);
	if (dllAdd = NULL)
	{
		MessageBox(NULL, "内存申请失败！", "错误", 0);
		return;
	}

	//写入dll
	if (WriteProcessMemory(hProcess, dllAdd, pathStr, sizeof(pathStr), NULL) == 0)
	{
		MessageBox(NULL, "dll路径写入失败！", "错误", 0);
		return;
	}
	
	//获取函数地址
	HMODULE k32 = GetModuleHandle("Kernel32.dll");
	LPVOID loadAdd = GetProcAddress(k32, "LoadLibraryA");
	HANDLE exec = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadAdd, dllAdd,0, NULL);
	if (NULL == exec)
	{
		MessageBox(NULL, "注入失败！", "错误", 0);
		return;
	}
}