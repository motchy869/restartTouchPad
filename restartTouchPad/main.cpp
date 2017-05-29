#include <Windows.h>
#include <Powrprof.h>
#include <Tchar.h>

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <io.h>

#define APPNAME	_T("restartTouchPad")

using namespace std;

string devcon_path, devID;	//devcon.exe のパス, タッチパッドのデバイスID

DEVICE_NOTIFY_CALLBACK_ROUTINE DeviceNotifyCallbackRoutine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	//二重起動防止
	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("mutex_restartTouchPad"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) { ReleaseMutex(hMutex); CloseHandle(hMutex); return EXIT_SUCCESS; }

	//引数処理
	if (__argc != 3) {
		MessageBox(NULL, _T("引数が不正です。\n・第1引数を devcon.exe のパス\n・第2引数をタッチパッドのデバイスID\nにしてください。\nプログラムを終了します。"), APPNAME, MB_OK|MB_ICONWARNING);
		exit(EXIT_FAILURE);
	}
	if (_access_s(__argv[1], 0) != 0) {
		MessageBox(NULL, _T("指定されたパスに devcon.exe が存在しません。\nプログラムを終了します。"), APPNAME, MB_OK | MB_ICONWARNING);
		exit(EXIT_FAILURE);
	}
	devcon_path = __argv[1];
	devID = __argv[2];

	//スリープ突入/復帰時にメッセージを受け取るように設定
	DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS dnsp = { DeviceNotifyCallbackRoutine };
	HPOWERNOTIFY hPwNotif = RegisterSuspendResumeNotification(&dnsp, DEVICE_NOTIFY_CALLBACK);
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	return msg.wParam;
}

//スリープ突入/復帰時に呼ばれるコールバック関数
_Use_decl_annotations_
ULONG CALLBACK DeviceNotifyCallbackRoutine(PVOID Context, ULONG Type, PVOID Setting) {
	switch (Type) {
		case PBT_APMRESUMESUSPEND:
			string cmd(devcon_path); cmd += " restart \"" + devID + "\"";
			system(cmd.c_str());
			break;
	}
	return 0UL;
}