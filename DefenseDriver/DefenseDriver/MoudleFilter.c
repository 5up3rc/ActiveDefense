#include "common.h"
BOOLEAN VxkCopyMemory(PVOID pDestination, PVOID pSourceAddress, SIZE_T SizeOfCopy)
{
	PMDL pMdl = NULL;
	PVOID pSafeAddress = NULL;
	pMdl = IoAllocateMdl(pSourceAddress, (ULONG)SizeOfCopy, FALSE, FALSE, NULL);//����MDL�ڴ� 
	if (!pMdl) 
		return FALSE;
	__try
	{
		MmProbeAndLockPages(pMdl, KernelMode, IoReadAccess);//�����ڴ�ҳ
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		IoFreeMdl(pMdl);//�ͷ�
		return FALSE;
	}
	pSafeAddress = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);//Bufferӳ�䵽�ں˵�ַ�ռ�
	if (!pSafeAddress) 
		return FALSE;
	RtlCopyMemory(pDestination, pSafeAddress, SizeOfCopy);//����
	MmUnlockPages(pMdl);//�����ڴ�ҳ
	IoFreeMdl(pMdl);//�ͷ�
	return TRUE;
}
PVOID GetDriverEntryByImageBase(PVOID ImageBase)//���ؿ�ִ��ӳ�� ��ִ����ڵ�ַ
{
	PIMAGE_DOS_HEADER pDOSHeader;
	PIMAGE_NT_HEADERS64 pNTHeader;
	PVOID pEntryPoint;
	pDOSHeader = (PIMAGE_DOS_HEADER)ImageBase;
	pNTHeader = (PIMAGE_NT_HEADERS64)((ULONG64)ImageBase + pDOSHeader->e_lfanew);
	pEntryPoint = (PVOID)((ULONG64)ImageBase + pNTHeader->OptionalHeader.AddressOfEntryPoint);
	return pEntryPoint;
}

void DenyLoadDriver(PVOID DriverEntry)//����ڵ�ַд���ִֹ�еĻ�����
{
	//Mov eax,c0000022h  B8 22 00 00 C0
	//Ret							 C3
	UCHAR code[] = "\xB8\x22\x00\x00\xC0\xC3";
	if (MmIsAddressValid(DriverEntry))
	{
		VxkCopyMemory(DriverEntry, code, sizeof(code));
	}
	
}

VOID LoadImageNotifyRoutine//ģ����� ���˺���
(
__in_opt PUNICODE_STRING  FullImageName,
__in HANDLE  ProcessId,
__in PIMAGE_INFO  ImageInfo
)
{
	PVOID pDrvEntry;
	char szFullImageName[260] = { 0 };
	if (FullImageName != NULL && MmIsAddressValid(FullImageName))
	{
		if (ProcessId == 0)//�����ö�����ϵͳ�����Լ��ص�������
		{
			DbgPrint("ImageName:%wZ\n", FullImageName);
			pDrvEntry = GetDriverEntryByImageBase(ImageInfo->ImageBase);
			DbgPrint("DriverEntry: %p\n", pDrvEntry);
/*		UnicodeToChar(FullImageName, szFullImageName);
		if (strstr(_strlwr(szFullImageName), "win64ast.sys"))
			{
				DbgPrint("Deny load [WIN64AST.SYS]");
				//��ֹ����win64ast.sys
				DenyLoadDriver(pDrvEntry);
			}*/
		}
	}
}

INT MoudleFilterLoad()
{
	PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)LoadImageNotifyRoutine);
	return 0;
}
INT MoudleFilterUnLoad()
{
	PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)LoadImageNotifyRoutine);
	return 0;
}