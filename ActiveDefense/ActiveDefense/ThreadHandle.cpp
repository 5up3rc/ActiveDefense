#include "stdafx.h"
bool g_isProcessOver = 0;//ָʾ�����Ƿ�Ҫ�˳�
int g_ThreadNum = 0;//��¼�̸߳���
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"
#define TYPE_PROCESS_CREAT 1
#define TYPE_PROCESS_EXIT	2
struct EventStrInfo g_EventStrInfo;
INT AnalysisType(PCHAR pStr)
{
	int nType;//�¼����ͣ��ַ�������
	int *pNum = (int *)pStr;//ָ�򻺳���ͷ��
	nType = *pNum;
	return nType;
}
VOID AnalysisStr(PCHAR pStr,INT *p_nType)
{
	int nType, nLength;//�¼����ͣ��ַ�������
	CString szTemp;

	int *pNum=(int *)pStr;//ָ�򻺳���ͷ��
	nType = *pNum;
	nLength = *(pNum + 1);
	*p_nType = nType;//���ؼ�¼������

	//����������·��
	USHORT *pShort = (USHORT  *)(pStr);//ָ��UCHAR�ַ�����ַ
	pShort += 8;//����ǰ4���ֽ�
	g_EventStrInfo.szPath = L"";
	for (int i = 0; i<nLength / 2 - 4; i++)
	{
		//if (*pShort != 0)//û�е�������
		g_EventStrInfo.szPath.AppendChar(*pShort);
		pShort++;
	}
	//szTemp = L"For Test";
	
	//------------------------------------------------------------------------
	switch (nType)//�����¼�����
	{
	case TYPE_PROCESS_CREAT:
		g_EventStrInfo.szType = L"�¼����ͣ����̴���";
		break;
	case TYPE_PROCESS_EXIT:
		g_EventStrInfo.szType = L"�¼����ͣ������˳�";
		break;

	default:
		g_EventStrInfo.szType = L"�¼����ͣ�δ�����¼�����";
		break;
	}
	
	if (nType == TYPE_PROCESS_CREAT)
	{
		g_EventStrInfo.szName = g_EventStrInfo.szPath;
		int nPos = szTemp.Find('\\');
		while (nPos)//������������
		{
			g_EventStrInfo.szName = g_EventStrInfo.szName.Mid(nPos + 1, g_EventStrInfo.szName.GetLength() - nPos);  //ȡ'\'�ұ��ַ���
			nPos = g_EventStrInfo.szName.Find('\\');   //������'\'������ֵ����-1 

			if (nPos == -1)
			{
				nPos = 0;
			}
		}
		g_EventStrInfo.szCaption = L"�����¼�";
		g_EventStrInfo.szDescribe = "����·��:";
		g_EventStrInfo.szName = L"������:" + g_EventStrInfo.szName;
	}
	

	ShowInfoInDlg(g_EventStrInfo.szType);
	if (nType == TYPE_PROCESS_EXIT)
	{
		ShowInfoInDlg(L"������:"+g_EventStrInfo.szPath);
		ShowInfoInDlg(L"----------------------------------------------");
	}
	else
	{
		ShowInfoInDlg(g_EventStrInfo.szName);
		ShowInfoInDlg(L"·��:"+g_EventStrInfo.szPath);
		
	}
	
	
	
}
VOID ShowHandleInfo(int nDlgRet)
{
	CString szTemp;
	switch (nDlgRet)
	{
	case 0:
		szTemp.Format(L"�û�ѡ������ý������С�\r\n----------------------------------------------");
		break;
	case 1:
		szTemp.Format(L"�û�ѡ���ֹ�ý������С�\r\n----------------------------------------------");
		break;

	default:
		szTemp.Format(L"�û�����ֵ�������⡣\r\n----------------------------------------------");
		break;
	}
	ShowInfoInDlg(szTemp);
}
unsigned int __stdcall ThreadHandle(VOID *pParam)
{
	
/*	g_EventStrInfo.szCaption = L"�����¼�";
	g_EventStrInfo.szType = L"�¼�����:���̴���";
	g_EventStrInfo.szName =L"������:360Safe.exe";
	g_EventStrInfo.szDescribe = "����·��:";
	g_EventStrInfo.szPath= L"C:\\Program Files (x86)\\360\\360safe\\Program Files (x86)\\360\\360safe\\360Safe.exe";
	myDlg.DoModal();
*/
	WarningDlg myDlg;

	int nDlgRet;
	CString CS_Str;
	HANDLE device = NULL;
	//char *msg = { "Hello driver, this is a message from app.\r\n" };
	char Buffer[512+8];
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
	ShowInfoInDlg(L"�߳���Ϣ������߳̿���\r\n----------------------------------------------");
	while (!g_isProcessOver)
	{

		if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), NULL, 0, Buffer, sizeof(Buffer), &ret_len, 0))//��ȡ
		{
			ShowInfoInDlg(L"�߳���Ϣ����������ȡ��Ϣʧ��");
			continue;
		}
		

		if (ret_len == 0)
		{
			ShowInfoInDlg(L"��ȡ���ķ�����ϢΪ0,�̼߳���\r\n----------------------------------------------");
			continue;
		}
		int nType;
		AnalysisStr(Buffer, &nType);//�����ܵ�������
		if (nType != TYPE_PROCESS_CREAT)
			continue;

		nDlgRet = myDlg.DoModal();  //�������洰��
		ShowHandleInfo(nDlgRet);
		if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), &nDlgRet, sizeof(nDlgRet), NULL, 0, &ret_len, 0))//���ʹ�����
		{
			ShowInfoInDlg(L"�߳���Ϣ��������������Ϣʧ��");
			continue;
		}

	}
	
	CloseHandle(device);
	g_ThreadNum--;//�߳��ܸ���-1
	//MessageBox(NULL, L"�߳��˳�", L"", NULL);
	ShowInfoInDlg(L"�߳���Ϣ������߳��˳�\r\n----------------------------------------------");
	return 0;
}