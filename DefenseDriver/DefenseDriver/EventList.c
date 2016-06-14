#include "common.h"

#define MEM_TAG 110
int g_number = 0;//�����ȫ�ֱ��
extern INT g_isRefuse;//ָʾ�����Ƿ񱻷���

LIST_ENTRY		my_list_head;
KSPIN_LOCK	my_list_lock;


void EventListInit()//�����ʼ��
{
	InitializeListHead(&my_list_head);//��ʼ������ͷ
	KeInitializeSpinLock(&my_list_lock);
}

NTSTATUS AddEventToList(PMY_EVENT pEvent)
{

	ExInterlockedInsertTailList(&my_list_head, pEvent, &my_list_lock);//������β������
	return STATUS_SUCCESS;
}
PMY_EVENT RemoveEventFromList()//������ͷ��ɾ��,������ɾ���Ľṹ��
{
	PLIST_ENTRY pEntry;
	PMY_EVENT pEvent;
	pEntry=ExInterlockedRemoveHeadList(&my_list_head, &my_list_lock);//��ȡɾ���������ַ
	pEvent = CONTAINING_RECORD(pEntry,MY_EVENT, list_enty);//�������ַ�����MY_EVENT�ṹ���ַ

	return pEvent;
}

PMY_EVENT GetEvent()
{
	PLIST_ENTRY pEntry = my_list_head.Flink;
	PMY_EVENT pEvent;

	pEvent = CONTAINING_RECORD(pEntry, MY_EVENT, list_enty);//�������ַ�����MY_EVENT�ṹ���ַ
	KdPrint(("GetEvent:%d", pEvent->nType));
	return pEvent;
}
VOID DeleteAllList()//�ڹرյ�ʱ���ͷ�������������
{
	KdPrint(("DeleteAllList"));
	g_isRefuse = 0;//����Ĭ����������
	while (!IsListEmpty(&my_list_head))//ֻҪ����Ϊ��
	{
		PMY_EVENT pEvent = RemoveEventFromList();//������ɾ��һ���¼�
		KeSetEvent(pEvent->pProcessEvent, IO_NO_INCREMENT, FALSE);//����ص�����
		KdPrint(("pEvent:%d", pEvent));
		ExFreePool(pEvent);//�ͷ��ڴ�
	}
}
void ShowList()
{
	int i = 0;
	PLIST_ENTRY p;
	for (p = my_list_head.Flink; p != &my_list_head.Flink; p = p->Flink)
	{
		PMY_EVENT pEvent = CONTAINING_RECORD(p, MY_EVENT, list_enty);
		KdPrint(("i:%d,pEvent->type=%d", i++, pEvent->nType));
	}
}


