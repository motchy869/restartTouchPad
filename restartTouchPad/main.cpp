#include <Windows.h>
#include <Powrprof.h>
#include <Tchar.h>

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <io.h>

#define APPNAME	_T("restartTouchPad")

using namespace std;

string devcon_path, devID;	//devcon.exe �̃p�X, �^�b�`�p�b�h�̃f�o�C�XID

DEVICE_NOTIFY_CALLBACK_ROUTINE DeviceNotifyCallbackRoutine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	//��d�N���h�~
	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("mutex_restartTouchPad"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) { ReleaseMutex(hMutex); CloseHandle(hMutex); return EXIT_SUCCESS; }

	//��������
	if (__argc != 3) {
		MessageBox(NULL, _T("�������s���ł��B\n�E��1������ devcon.exe �̃p�X\n�E��2�������^�b�`�p�b�h�̃f�o�C�XID\n�ɂ��Ă��������B\n�v���O�������I�����܂��B"), APPNAME, MB_OK|MB_ICONWARNING);
		exit(EXIT_FAILURE);
	}
	if (_access_s(__argv[1], 0) != 0) {
		MessageBox(NULL, _T("�w�肳�ꂽ�p�X�� devcon.exe �����݂��܂���B\n�v���O�������I�����܂��B"), APPNAME, MB_OK | MB_ICONWARNING);
		exit(EXIT_FAILURE);
	}
	devcon_path = __argv[1];
	devID = __argv[2];

	//�X���[�v�˓�/���A���Ƀ��b�Z�[�W���󂯎��悤�ɐݒ�
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

//�X���[�v�˓�/���A���ɌĂ΂��R�[���o�b�N�֐�
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