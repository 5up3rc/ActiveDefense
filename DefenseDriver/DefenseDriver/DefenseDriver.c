#include "common.h"
#define CDO_DEVICE_NAME    L"\\Device\\DefenseDevice"
#define CDO_SYB_NAME    L"\\??\\DefenseDevice"

// ��Ӧ�ò����������һ���ַ�����
#define  IOCTL_SEND (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x911,METHOD_BUFFERED, FILE_ANY_ACCESS)
// ��������ȡһ���ַ���
#define  IOCTL_RECV (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0x912,METHOD_BUFFERED, FILE_ANY_ACCESS)
// Ӧ�ò�ر��ź�
#define  IOCTL_CLOSE (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x913,METHOD_BUFFERED, FILE_ANY_ACCESS)
// Ӧ�ò㿪�����̱���
#define  IOCTL_PROCESS_PROTECT (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x914,METHOD_BUFFERED, FILE_ANY_ACCESS)
//Ӧ�ò�رս��̱���
#define  IOCTL_PROCESS_UNPROTECT (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x915,METHOD_BUFFERED, FILE_ANY_ACCESS)
// Ӧ�ò㿪�����̼��
#define  IOCTL_PROCESS_FILTER (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x916,METHOD_BUFFERED, FILE_ANY_ACCESS)
//Ӧ�ò�رս��̼��
#define  IOCTL_PROCESS_UNFILTER (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x917,METHOD_BUFFERED, FILE_ANY_ACCESS)
// Ӧ�ò㿪���������
#define  IOCTL_DRIVER_FILTER (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x918,METHOD_BUFFERED, FILE_ANY_ACCESS)
//Ӧ�ò�ر��������
#define  IOCTL_DRIVER_UNFILTER (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x919,METHOD_BUFFERED, FILE_ANY_ACCESS)

PDEVICE_OBJECT g_pDevObj;//���ɵ��豸����ָ��
KEVENT g_kEvent;	//ȫ���¼�����
INT g_Close_Flag = 0;//�����˳���־λ

extern LIST_ENTRY		my_list_head;
extern INT g_isRefuse;//ָʾ�����Ƿ񱻷���
extern PEPROCESS g_ProcectEProcess;//�����Ľ���
extern ULONG g_OpDat;

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
	
	switch (code)
	{
	case IOCTL_SEND://Ӧ�÷����ź�
		KdPrint(("IOCTL_SEND"));
	
		if (!IsListEmpty(&my_list_head) && inlen==4 )//����Ϊ��
		{
			g_isRefuse = *(int *)(buffer);//ȡ��Ӧ�ò㷵�ؽ��
			PMY_EVENT pEvent = RemoveEventFromList();//������ɾ��һ���¼�

			if (pEvent->pProcessEvent)
			{
				KeSetEvent(pEvent->pProcessEvent, IO_NO_INCREMENT, FALSE);//�����¼�,�ص����̼�������,��������Ƿ�����
			}
			
			ExFreePool(pEvent);//�ͷ��ڴ�
		}
		

		Irp->IoStatus.Information = 0;//д�볤��
		KdPrint(("IOCTL_SEND��Over"));
		break;
	//-----------------------------------------------------------------------------------------------------
	case IOCTL_RECV://Ӧ�ö�ȡ�ź�
		KdPrint(("IOCTL_RECV"));
	
		while (IsListEmpty(&my_list_head) && g_Close_Flag == 0)//����Ϊ���ұ�־λΪ0
		{
			KdPrint(("ѭ���ȴ�,g_Close_Flag:%d", g_Close_Flag));
			KeWaitForSingleObject(&g_kEvent, Executive, KernelMode, FALSE, 0);//�ȴ��¼��ź�  
		}

		if (g_Close_Flag == 0)//���ǹر��ź�
		{
			KdPrint(("���ǹر��ź�"));

			PMY_EVENT pEvent = GetEvent();
			RtlCopyMemory(buffer, &pEvent->nType, sizeof(int));//����4���ֽڵ�����ֵ
			
			RtlCopyMemory((PVOID)((char *)buffer+4), &pEvent->nLength, sizeof(int));//����4���ֽ��ַ�������
			RtlCopyMemory((PVOID)((char *)buffer + 8), pEvent->wStr, pEvent->nLength);//����wchar�ַ���
			KdPrint(("nType:%d,nLength:%d", pEvent->nType, pEvent->nLength));
			Irp->IoStatus.Information = 8 + pEvent->nLength;//д�볤��

			if (pEvent->nType== 2 )//�����˳��¼�
			{
				pEvent = RemoveEventFromList();//������ɾ��һ���¼�
				ExFreePool(pEvent);//�ͷ��ڴ�
			}
		}
		else
		{
			KdPrint(("g_Close_Flag��0���ر��ź�"));
			Irp->IoStatus.Information = 0;//д�볤��Ϊ0
			g_Close_Flag = 0;//��������Ϊ0 ,�´�Ӧ�ö�ȡ������
		}

		KdPrint(("IOCTL_RECV����"));
		break;
	//-----------------------------------------------------------------------------------------------------
	case IOCTL_CLOSE:
		KdPrint(("IOCTL_CLOSE"));
		g_Close_Flag = 1;
	
		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//�����¼�
		DeleteAllList();//ɾ��������������
		Irp->IoStatus.Information = 0;
		break;
	//-----------------------------------------------------------------------------------------------------------------------------
	case IOCTL_PROCESS_PROTECT:
		KdPrint(("IO_PROTECT:%s", buffer));
		Irp->IoStatus.Information=ProcessProcectByName(buffer);
		break;
	//-----------------------------------------------------------------------------------------------------------------------------
	case	IOCTL_PROCESS_UNPROTECT:
		KdPrint(("IOCTL_PROCESS_UNPROTECT"));
		
		Irp->IoStatus.Information = UnloadProcessProtect();//�رս��̱���

		KdPrint(("IOCTL_PROCESS_UNPROTECT Over"));
		break;
	//-----------------------------------------------------------------------------------------------------------------------------
	case IOCTL_PROCESS_FILTER:
		KdPrint(("IOCTL_PROCESS_FILTER"));
		Irp->IoStatus.Information = CreateProcessRoutine();//�������̻ص�
		break;
	//-----------------------------------------------------------------------------------------------------------------------------
	case IOCTL_PROCESS_UNFILTER:
		KdPrint(("IOCTL_PROCESS_UNFILTER"));
		g_Close_Flag = 1;

		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//�����¼�
		DeleteAllList();//ɾ��������������
		UnLoadProcessRoutine();//�رս��̻ص�

		Irp->IoStatus.Information = 0;
		break;
	//-----------------------------------------------------------------------------------------------------------------------------
	case IOCTL_DRIVER_FILTER:
		KdPrint(("IOCTL_DRIVER_FILTER"));
		Irp->IoStatus.Information = MoudleFilterLoad();//�������̻ص�
		break;
	//-----------------------------------------------------------------------------------------------------------------------------
	case IOCTL_DRIVER_UNFILTER:
		KdPrint(("IOCTL_DRIVER_UNFILTER"));
		g_Close_Flag = 1;

		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//�����¼�
		DeleteAllList();//ɾ��������������
		
		Irp->IoStatus.Information = MoudleFilterUnLoad();//�ر�ģ��ص�
		break;
	//-----------------------------------------------------------------------------------------------------------------------------
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

	UnloadProcessProtect();//�رս��̱���
	MoudleFilterUnLoad();//�ر�ģ����
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

	
	
	return STATUS_SUCCESS;
}

