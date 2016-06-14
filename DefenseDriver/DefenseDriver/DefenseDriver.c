#include "common.h"
#define CDO_DEVICE_NAME    L"\\Device\\DefenseDevice"
#define CDO_SYB_NAME    L"\\??\\DefenseDevice"
// ��Ӧ�ò����������һ���ַ�����
#define  IOCTL_SEND \
	(ULONG)CTL_CODE( \
	FILE_DEVICE_UNKNOWN, \
	0x911,METHOD_BUFFERED, \
	FILE_ANY_ACCESS)

// ��������ȡһ���ַ���
#define  IOCTL_RECV\
	(ULONG)CTL_CODE( \
	FILE_DEVICE_UNKNOWN, \
	0x912,METHOD_BUFFERED, \
	FILE_ANY_ACCESS)

// Ӧ�ò�ر��ź�
#define  IOCTL_CLOSE\
	(ULONG)CTL_CODE( \
	FILE_DEVICE_UNKNOWN, \
	0x913,METHOD_BUFFERED, \
	FILE_ANY_ACCESS)

PDEVICE_OBJECT g_pDevObj;//���ɵ��豸����ָ��
KEVENT g_kEvent;	//ȫ���¼�����
INT g_Close_Flag = 0;//�����˳���־λ

extern LIST_ENTRY		my_list_head;
extern INT g_isRefuse;//ָʾ�����Ƿ񱻷���

NTSTATUS CreateDevice(PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS Status;
	UNICODE_STRING usDevName, usSymName;

	RtlInitUnicodeString(&usDevName, CDO_DEVICE_NAME);
	//����һ���豸
	Status = IoCreateDevice(pDriverObject, 0, &usDevName, 
		FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, 
		FALSE, &g_pDevObj);
	if (!NT_SUCCESS(Status))//��������
	{
		return Status;
	}
	g_pDevObj->Flags |= DO_BUFFERED_IO;

	RtlInitUnicodeString(&usSymName, CDO_SYB_NAME);
	//������������
	IoDeleteSymbolicLink(&usSymName);//��ɾ��
	Status = IoCreateSymbolicLink(&usSymName, &usDevName);
	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(g_pDevObj);
		return Status;
	}
	return STATUS_SUCCESS;
}
VOID UnloadDevice()//ɾ���豸���󼰷�������
{
	UNICODE_STRING cdo_syb = RTL_CONSTANT_STRING(CDO_SYB_NAME);
	IoDeleteSymbolicLink(&cdo_syb);//ɾ����������
	IoDeleteDevice(g_pDevObj);//ɾ������

}

NTSTATUS MyDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)//Control�ַ�����
{
	NTSTATUS Status = STATUS_SUCCESS;
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
	ULONG inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG code = irpsp->Parameters.DeviceIoControl.IoControlCode;
	//KdPrint(("����:%s", buffer));
	//KdPrint(("code:%x,IOCTL_SEND:%x", code, IOCTL_SEND));

	
	switch (code)
	{
	case IOCTL_SEND://Ӧ�÷����ź�
		KdPrint(("IOCTL_SEND:%s",buffer));
		KdPrint(("inlen:%d,outlen:%d", inlen, outlen));
		PMY_EVENT pEvent=RemoveEventFromList();//������ɾ��һ���¼�
		KeSetEvent(pEvent->pProcessEvent, IO_NO_INCREMENT, FALSE);//�����¼�,�ص����̼�������,��������Ƿ�����
		//int *p = buffer;

		g_isRefuse = *(int *)(buffer);
		//KdPrint(("g_isRefuse = *p:%d", *p));
		ExFreePool(pEvent);//�ͷ��ڴ�

		Irp->IoStatus.Information = 0;//д�볤��
		break;
	case IOCTL_RECV://Ӧ�ö�ȡ�ź�
		KdPrint(("IOCTL_RECV"));
		/*
		if (IsListEmpty(&my_list_head))
		{
			KdPrint(("����Ϊ��"));
		}
		else
		{
			KdPrint(("����buΪ��"));

		}*/
		while (IsListEmpty(&my_list_head) && g_Close_Flag == 0)//����Ϊ���ұ�־λΪ0
		{
			/*
			if (Close_Flag == 0)
			{
				KdPrint(("Close_Flag == 0"));
			}*/
			KdPrint(("ѭ���ȴ�֮ǰ,g_Close_Flag:%d", g_Close_Flag));
			KeWaitForSingleObject(&g_kEvent, Executive, KernelMode, FALSE, 0);//�ȴ��¼��ź�  
		}
		//KdPrint(("Close_flag:%d", Close_Flag));

		if (g_Close_Flag == 0)//���ǹر��ź�
		{
			KdPrint(("g_Close_Flag != 0 //���ǹر��ź�"));

			PMY_EVENT pEvent = GetEvent();
			RtlCopyMemory(buffer, &pEvent->nType, sizeof(int));//����4���ֽڵ�����ֵ
			
			RtlCopyMemory((PVOID)((char *)buffer+4), &pEvent->nLength, sizeof(int));//����4���ֽ��ַ�������
			RtlCopyMemory((PVOID)((char *)buffer + 8), pEvent->wStr, pEvent->nLength);//����wchar�ַ���
			KdPrint(("nType:%d,nLength:%d", pEvent->nType, pEvent->nLength));
			Irp->IoStatus.Information = 8 + pEvent->nLength;//д�볤��
		}
		else
		{
			KdPrint(("g_Close_Flag��0���ر��ź�"));
			Irp->IoStatus.Information = 0;//д�볤��Ϊ0
		}
		//strcpy((char *)buffer, "just soso");
		
		
		//KdPrint(("inlen:%d,outlen:%d", inlen, outlen));
		KdPrint(("IOCTL_RECV����"));
		break;
	case IOCTL_CLOSE:
		KdPrint(("IOCTL_CLOSE"));
		g_Close_Flag = 1;
		//�����¼�
		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);
		DeleteAllList();
		Irp->IoStatus.Information = 0;//д�볤��
		break;

	}

	
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Status;
}
NTSTATUS MyCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrintEx((DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "MyCreate\r\n"));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}
NTSTATUS MyClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrintEx((DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "MyClose\r\n"));
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}

VOID MyUnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	UnloadDevice();//ɾ���豸����������
	UnLoadProcessRoutine();//�رս��̻ص�
	KdPrint(("Unload"));
}
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	KdPrint(("Driver load"));
	NTSTATUS Status;

	//�����豸����������
	Status = CreateDevice(pDriverObject);
	if (!NT_SUCCESS(Status))
	{
		KdPrint(("create Defense device error"));
	}
	else
	{
		KdPrint(("create Defense  device success"));
	}
	//��ʼ��ȫ���¼�,ͬ���¼�
	KeInitializeEvent(&g_kEvent, SynchronizationEvent, FALSE);
	//�����ʼ��
	EventListInit();

	//���÷ַ�����
	pDriverObject->DriverUnload = MyUnloadDriver;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDeviceControl;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = MyCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = MyClose;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = MyClose;

	CreateProcessRoutine();
	/*
	PMY_EVENT pEvent = (PMY_EVENT)ExAllocatePoolWithTag(PagedPool, sizeof(MY_EVENT),112);//�����ڴ�
	if (pEvent == NULL)
	{
		KdPrint(("����ռ�Ϊ��"));
	}
	else
	{
		KdPrint(("����ռ�ɹ�"));
	}
	pEvent->nType = 1;
	pEvent->nLength = 123;
	KdPrint(("Entry:nTtype:%d,nLen:%d", pEvent->nType, pEvent->nLength));
	AddEventToList(pEvent);//��������
	ShowList();
	KdPrint(("------------"));
	GetEvent();

	RemoveEventFromList();
	*/
	return STATUS_SUCCESS;
}

