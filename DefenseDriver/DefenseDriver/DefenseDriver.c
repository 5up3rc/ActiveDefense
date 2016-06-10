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
;
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
	case IOCTL_SEND:
		KdPrint(("IOCTL_SEND:%s",buffer));
		break;
	case IOCTL_RECV:
		//�ȴ��ź�  
		KeWaitForSingleObject(&g_kEvent,Executive,KernelMode,FALSE,0);
		strcpy((char *)buffer, "just soso");
		KdPrint(("IOCTL_RECV"));
		break;
	case IOCTL_CLOSE:
		KdPrint(("IOCTL_CLOSE"));
		//�����¼�
		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);
		break;

	}

	Irp->IoStatus.Information = outlen;
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

	//���÷ַ�����
	pDriverObject->DriverUnload = MyUnloadDriver;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDeviceControl;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = MyCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = MyClose;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = MyClose;

	CreateProcessRoutine();
	return STATUS_SUCCESS;
}

