#ifndef _EVENT_LIST_H_
#define _EVENT_LIST_H_
#include "common.h"

#define MAX_PATH 512
typedef struct
{
	LIST_ENTRY list_enty;
	INT32	nType; 
	wchar_t wStr[MAX_PATH];
}MY_EVENT, *PMY_EVENT;


void EventListInit();

NTSTATUS AddEventToList(PMY_EVENT pEvent);
PMY_EVENT RemoveEventFromList();//������ͷ��ɾ��,������ɾ���Ľṹ��
void add();
void remove();

void ShowList();


#endif