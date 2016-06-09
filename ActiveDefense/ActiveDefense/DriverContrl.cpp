#include "stdafx.h"
#include <Winsvc.h>

#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_ANY_ACCESS                 0
#define METHOD_BUFFERED                 0

DWORD CTL_CODE_GEN(DWORD lngFunction)
{
	return (FILE_DEVICE_UNKNOWN * 65536) | (FILE_ANY_ACCESS * 16384) | (lngFunction * 4) | METHOD_BUFFERED;
}
//szFullPath����·����szName��������iType��������(��װ�����С�ֹͣ���Ƴ�)
BOOL operaType(TCHAR *szFullPath, TCHAR *szName, int iType)
{
	SC_HANDLE shOSCM = NULL, shCS = NULL;
	SERVICE_STATUS ss;
	DWORD dwErrorCode = 0;
	BOOL bSuccess = FALSE;

	shOSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!shOSCM)
	{
		//SetWindowText(hwndStatus, TEXT("OpenSCManagerʱ����"));
		MessageBox(NULL, L"OpenSCManagerʱ����", NULL, NULL);
		return FALSE;
	}

	if (iType) //���������Ͳ���"��װ����"
	{
		shCS = OpenService(shOSCM, szName, SERVICE_ALL_ACCESS);
		if (!shCS)
		{
			dwErrorCode = GetLastError(); //��ȡ������Ϣ
			if (ERROR_INVALID_NAME == dwErrorCode)
				//SetWindowText(hwndStatus, TEXT("��������Ч��"));
				MessageBox(NULL, L"��������Ч��", NULL, NULL);
			else if (ERROR_SERVICE_DOES_NOT_EXIST == dwErrorCode)
				//SetWindowText(hwndStatus, TEXT("�����ڴ˷���"));
				MessageBox(NULL, L"�����ڴ˷���", NULL, NULL);
			else
				//SetWindowText(hwndStatus, TEXT("OpenServiceʱ����"));
				MessageBox(NULL, L"OpenServiceʱ����", NULL, NULL);

			CloseServiceHandle(shOSCM); //�رշ�����
			shOSCM = NULL;
			return FALSE;
		}
	}

	switch (iType)
	{
	case 0: //��װ����
		shCS = CreateService(shOSCM, szName, szName,
			SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
			szFullPath, NULL, NULL, NULL, NULL, NULL);

		if (!shCS)
		{
			if (ERROR_SERVICE_EXISTS == GetLastError()) //��������Ѿ�����
				MessageBox(NULL, L"ָ���ķ����Ѿ�����", NULL, NULL);
			else
				MessageBox(NULL, L"CreateService", NULL, NULL);

			bSuccess = FALSE;
			break;
		}
		bSuccess = TRUE;

		MessageBox(NULL, L"��װ����ɹ�", NULL, NULL);
		break;

	case 1: //���з���
		if (StartService(shCS, 0, NULL))
			//SetWindowText(hwndStatus, TEXT("����ָ������ɹ���"));
			MessageBox(NULL, L"����ָ������ɹ�", NULL, NULL);
		else
		{
			dwErrorCode = GetLastError();
			if (ERROR_SERVICE_ALREADY_RUNNING == dwErrorCode)
				//SetWindowText(hwndStatus, TEXT("ָ���ķ����Ѿ����У�"));
				MessageBox(NULL, L"ָ���ķ����Ѿ�����", NULL, NULL);
			else
				//SetWindowText(hwndStatus, TEXT("����ָ���������"));
				MessageBox(NULL, L"����ָ���������", NULL, NULL);
			bSuccess = FALSE;
			break;
		}
		bSuccess = TRUE;
		break;

	case 2: //ֹͣ����
		if (!ControlService(shCS, SERVICE_CONTROL_STOP, &ss))
		{
			dwErrorCode = GetLastError();
			if (ERROR_SERVICE_NOT_ACTIVE == dwErrorCode)
				//SetWindowText(hwndStatus, TEXT("ָ���ķ���δ������"));
				MessageBox(NULL, L"ָ���ķ���δ����", NULL, NULL);
			else
				//SetWindowText(hwndStatus, TEXT("����ֹͣ����"));
				MessageBox(NULL, L"����ֹͣ����", NULL, NULL);
			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("�ɹ�ֹͣ����"));
		MessageBox(NULL, L"�ɹ�ֹͣ����", NULL, NULL);
		bSuccess = TRUE;
		break;

	case 3: //�Ƴ�����
		if (!DeleteService(shCS))
		{
			//SetWindowText(hwndStatus, TEXT("�����Ƴ�����"));
			MessageBox(NULL, L"�����Ƴ�����", NULL, NULL);
			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("�ɹ��Ƴ�����"));
		MessageBox(NULL, L"�ɹ��Ƴ�����", NULL, NULL);
		bSuccess = TRUE;
		break;

	default:
		break;
	}

	if (shCS)
	{
		CloseServiceHandle(shCS);
		shCS = NULL;
	}
	if (shOSCM)
	{
		CloseServiceHandle(shOSCM);
		shOSCM = NULL;
	}

	return bSuccess;
}