#include "common.h"
NTKERNELAPI PCHAR PsGetProcessImageFileName(PEPROCESS Process);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);

#define MEM_TAG 111

BOOLEAN g_isProcessRoutine=0;//��¼�Ƿ�װ���̻ص�
INT g_isRefuse = 0;//ָʾ�����Ƿ񱻷���
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
	KEVENT ProcessEvent;
	//��ʼ���¼�,ͬ���¼�
	KeInitializeEvent(&ProcessEvent, SynchronizationEvent, FALSE);
	PMY_EVENT pEvent = (PMY_EVENT)ExAllocatePoolWithTag(PagedPool, sizeof(MY_EVENT), MEM_TAG);//�����ڴ�
	if (pEvent == NULL)
	{
		return;
	}
	if (CreateInfo != NULL)	//���̴����¼�
	{
		pEvent->nType = 1;
		pEvent->nLength = CreateInfo->ImageFileName->Length;
		pEvent->pProcessEvent = &ProcessEvent;

		RtlCopyMemory(pEvent->wStr, CreateInfo->ImageFileName->Buffer, pEvent->nLength);//�����ַ���
		KdPrint(("ProcessRoutine:nTtype:%d,nLen:%d", pEvent->nType, pEvent->nLength));
		
		AddEventToList(pEvent);//��������

		DbgPrint("[���̴���:][PID:%d]·��: %wZ --",
			ProcessId,
			CreateInfo->ImageFileName);

		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//�����¼�,MyDeviceControl������������

		KeWaitForSingleObject(&ProcessEvent, Executive, KernelMode, FALSE, 0);//�ȴ��¼��Ĵ����ź� 
		KdPrint(("g_isRefuse:%d", g_isRefuse));
		if (g_isRefuse)
		{
			DbgPrint("��ֹ�������̣�");
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