#include "stdafx.h"
bool g_isProcessOver = 0;//ָʾ�����Ƿ�Ҫ�˳�
int g_ThreadNum = 0;//��¼�̸߳���
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"

unsigned int __stdcall ThreadHandle(VOID *pParam)
{
	
	HANDLE device = NULL;
	char *msg = { "Hello driver, this is a message from app.\r\n" };
	char str[100];
	ULONG ret_len;
	g_ThreadNum++;//�߳��ܸ���+1
	//���豸
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		//MessageBox(NULL, L"�̣߳��豸�򿪴���", L"", NULL);
		ShowInfoInDlg(L"�߳���Ϣ���豸�򿪴����߳��˳�");
		g_ThreadNum--;//�߳��ܸ���-1
		return -1;
	}
	ShowInfoInDlg(L"�߳���Ϣ������߳̿���");
	while (!g_isProcessOver)
	{
		Sleep(2000);
		if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), NULL, 0, str, sizeof(str), &ret_len, 0))//��ȡ
		{
			ShowInfoInDlg(L"�߳���Ϣ����������ȡ��Ϣʧ��");
			break;
		}

		if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), msg, strlen(msg) + 1, NULL, 0, &ret_len, 0))//����
		{
			ShowInfoInDlg(L"�߳���Ϣ��������������Ϣʧ��");
			break;
		}

	}
	
	CloseHandle(device);
	g_ThreadNum--;//�߳��ܸ���-1
	//MessageBox(NULL, L"�߳��˳�", L"", NULL);
	ShowInfoInDlg(L"�߳���Ϣ������߳��˳�");
	return 0;
}