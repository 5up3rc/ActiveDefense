
#define  IOCTL_SEND  0x911// ��Ӧ�ò����������һ���ַ�����
#define  IOCTL_RECV 0x912// ��������ȡһ���ַ���
#define  IOCTL_CLOSE 0x913// Ӧ�ò�ر��ź�
#define  IOCTL_PROCESS_PROTECT 0x914// Ӧ�ò㿪�����̱���
#define  IOCTL_PROCESS_UNPROTECT  0x915//Ӧ�ò�رս��̱���
#define  IOCTL_PROCESS_FILTER 0x916// Ӧ�ò㿪�����̼��
#define  IOCTL_PROCESS_UNFILTER 0x917//Ӧ�ò�رս��̼��
#define  IOCTL_DRIVER_FILTER  0x918// Ӧ�ò㿪���������
#define  IOCTL_DRIVER_UNFILTER  0x919//Ӧ�ò�ر��������


#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"

DWORD CTL_CODE_GEN(DWORD lngFunction);
//szFullPath����·����szName��������iType��������(��װ�����С�ֹͣ���Ƴ�)
BOOL operaType(TCHAR *szFullPath, TCHAR *szName, int iType);
int SendMsgToDriver(int nCode);