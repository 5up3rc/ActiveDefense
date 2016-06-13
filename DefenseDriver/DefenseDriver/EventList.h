#ifndef _EVENT_LIST_H_
#define _EVENT_LIST_H_
#include "common.h"

#define MAX_PATH 512
typedef struct
{
	LIST_ENTRY list_enty;
	INT32	nType; 
	INT32  nLength;//�ַ�������
	wchar_t wStr[MAX_PATH];
	PKEVENT pProcessEvent;//���ڻ��ѽ��̻ص��������¼�
}MY_EVENT, *PMY_EVENT;


void EventListInit();

NTSTATUS AddEventToList(PMY_EVENT pEvent);
PMY_EVENT RemoveEventFromList();//������ͷ��ɾ��,������ɾ���Ľṹ��
PMY_EVENT GetEvent(); 

void ShowList();


#endif