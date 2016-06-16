#include "common.h"
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
NTKERNELAPI CHAR* PsGetProcessImageFileName(PEPROCESS Process);

//ƫ�ƶ���(Win7 64λ��
#define PROCESS_FLAG_OFFSET					0x440

//Ŀ�걣������
PEPROCESS g_ProcectEProcess = NULL;
ULONG g_OpDat;

//��������
INT ProtectProcess(PEPROCESS Process, BOOLEAN bIsProtect, ULONG v)
{
	ULONG op;
	if (bIsProtect)
	{
		op = *(PULONG)((ULONG64)Process + PROCESS_FLAG_OFFSET);
		*(PULONG)((ULONG64)Process + PROCESS_FLAG_OFFSET) = 0;
		return op;
	}
	else
	{
		*(PULONG)((ULONG64)Process + PROCESS_FLAG_OFFSET) = v;
		return 0;
	}
}
//���EPROCESS
PEPROCESS GetProcessObjectByName(char *name)
{
	SIZE_T i;
	for (i = 100; i<20000; i += 4)
	{
		NTSTATUS st;
		PEPROCESS ep;
		st = PsLookupProcessByProcessId((HANDLE)i, &ep);
		if (NT_SUCCESS(st))
		{
			char *pn = PsGetProcessImageFileName(ep);
			KdPrint(("i:%d,pName:%s", i, pn));
			if (_stricmp(pn, name) == 0)
				return ep;
		}
	}
	return NULL;
}

INT ProcessProcectByName(char *szName)
{
	
	
	 g_ProcectEProcess = GetProcessObjectByName(szName);
	KdPrint(("����%s, %p\n", szName, g_ProcectEProcess));

	 if (g_ProcectEProcess)
	{
		g_OpDat = ProtectProcess(g_ProcectEProcess, 1, 0);
		ObDereferenceObject(g_ProcectEProcess);
		return 0;
	}
	 return 1;
}