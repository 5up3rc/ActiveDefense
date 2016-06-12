#include "common.h"
NTKERNELAPI PCHAR PsGetProcessImageFileName(PEPROCESS Process);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);

BOOLEAN g_isProcessRoutine=0;//��¼�Ƿ�װ���̻ص�
extern KEVENT g_kEvent;	//ȫ���¼�����



PCHAR GetProcessNameByProcessId(HANDLE ProcessId)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	PEPROCESS ProcessObj = NULL;
	PCHAR string = NULL;
	st = PsLookupProcessByProcessId(ProcessId, &ProcessObj);
	if (NT_SUCCESS(st))
	{
		string = PsGetProcessImageFileName(ProcessObj);
		ObfDereferenceObject(ProcessObj);
	}
	return string;
}

VOID MyCreateProcessNotifyEx//�����¼� �ص�������
(
__inout   PEPROCESS Process,
__in      HANDLE ProcessId,
__in_opt  PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	char xxx[32] = { 0 };
	if (CreateInfo != NULL)	//���̴����¼�
	{
		strcpy(xxx, GetProcessNameByProcessId(ProcessId));
		DbgPrint("[���̴���:][%s-PID:%d]·��: %wZ --",
			xxx,
			ProcessId,
			CreateInfo->ImageFileName);
		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//�����¼�

		if (!_stricmp(xxx, "calc.exe"))
		{
			DbgPrint("��ֹ�������������̣�");
			//CreateInfo->CreationStatus = STATUS_UNSUCCESSFUL;	//��ֹ��������
			CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
		}
	}
	else
	{
		DbgPrint("�����˳�: %s", PsGetProcessImageFileName(Process));
	}
	
}
VOID UnLoadProcessRoutine()
{
	if (g_isProcessRoutine)//����ص���������
	{
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)MyCreateProcessNotifyEx, TRUE);
		g_isProcessRoutine = 0;
	}
	KdPrint(("UnLoadProcessRoutine"));
}
NTSTATUS CreateProcessRoutine()
{
	NTSTATUS status = STATUS_SUCCESS;
	status = PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)MyCreateProcessNotifyEx, FALSE);
	if (status == STATUS_SUCCESS)
	{
		g_isProcessRoutine = 1;//��ʾ�����ص������ɹ�
	}
	KdPrint(("PsSetCreateProcessNotifyRoutineEx return: %x", status));
	return status;
}