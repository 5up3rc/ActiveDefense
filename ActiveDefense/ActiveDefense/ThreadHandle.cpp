#include "stdafx.h"
bool g_isProcessOver = 0;//ָʾ�����Ƿ�Ҫ�˳�
int g_ThreadNum = 0;//��¼�̸߳���
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"
 
unsigned int __stdcall ThreadHandle(VOID *pParam)
{
	WarningDlg myDlg;
	int nDlgRet;
	CString CS_Str;
	HANDLE device = NULL;
	char *msg = { "Hello driver, this is a message from app.\r\n" };
	char str[512+8];
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

		if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), NULL, 0, str, sizeof(str), &ret_len, 0))//��ȡ
		{
			ShowInfoInDlg(L"�߳���Ϣ����������ȡ��Ϣʧ��");
			continue;
		}
		/*
		CString temp;
		temp.Format(L"ret_len:%d", ret_len);
		ShowInfoInDlg(temp);
		*/
		

		int nType, nLength;
		int *pNum;
		pNum = (int *)&str;
		PWCHAR pWchr=PWCHAR(str+8);
		nType = *pNum;
		nLength = *(pNum + 1);
		CS_Str.Format(L"T:%d,L:%d,re:%d", nType, nLength, ret_len);

		ShowInfoInDlg(CS_Str);

		CS_Str = L"";
	
		//memcpy(CS_Str.GetBuffer(), &str[8], nLength);
		unsigned short *pShort=(unsigned short  *)&str[8];
		pShort += 4;//����ǰ4���ֽ�
		for (unsigned int i = 0; i<nLength/2-4; i++)
		{
			if (*pShort!=0)//û�е�������
				CS_Str.AppendChar(*pShort);
			pShort++;
		}
		ShowInfoInDlg(CS_Str);

		if (ret_len == 0) continue;
		nDlgRet = myDlg.DoModal();  //�������洰��
		CS_Str.Format(L"nDlgRet:%d", nDlgRet);
		ShowInfoInDlg(CS_Str);
		if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), &nDlgRet, sizeof(nDlgRet), NULL, 0, &ret_len, 0))//���ʹ�����
		//if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), msg, strlen(msg), NULL, 0, &ret_len, 0))//���ʹ�����
		{
			ShowInfoInDlg(L"�߳���Ϣ��������������Ϣʧ��");
			continue;
		}

	}
	
	CloseHandle(device);
	g_ThreadNum--;//�߳��ܸ���-1
	//MessageBox(NULL, L"�߳��˳�", L"", NULL);
	ShowInfoInDlg(L"�߳���Ϣ������߳��˳�");
	return 0;
}