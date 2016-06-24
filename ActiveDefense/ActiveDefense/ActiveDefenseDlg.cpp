
// ActiveDefenseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ActiveDefense.h"
#include "ActiveDefenseDlg.h"
#include "afxdialogex.h"

#pragma comment(lib,"user32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SOFT_CAPTION L"SML��������"
// CActiveDefenseDlg �Ի���

extern bool g_isProcessOver;//ָʾ�����Ƿ�Ҫ�˳�
extern int g_ThreadNum;//��¼�̸߳���
HANDLE g_ThreadHandle;

TCHAR szTitle[MAX_PATH] = { L"DefenseDriver" };

CActiveDefenseDlg::CActiveDefenseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActiveDefenseDlg::IDD, pParent)
	, m_EDIT_STR(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CActiveDefenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, btn_close);
	DDX_Control(pDX, IDC_MIN, btn_min);
	DDX_Text(pDX, IDC_EDIT1, m_EDIT_STR);
}

BEGIN_MESSAGE_MAP(CActiveDefenseDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_CLOSE, &CActiveDefenseDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_MIN, &CActiveDefenseDlg::OnBnClickedMin)
	ON_BN_CLICKED(IDC_LOAD_DRV, &CActiveDefenseDlg::OnBnClickedLoadDrv)
	ON_BN_CLICKED(IDC_UNLOAD_DRV, &CActiveDefenseDlg::OnBnClickedUnloadDrv)
	//ON_BN_CLICKED(IDC_START_THREAD, &CActiveDefenseDlg::OnBnClickedStartThread)
	ON_MESSAGE(WM_SHOW_MSG, &CActiveDefenseDlg::OnShowMsg)    // OnCountMsg���Զ������Ϣ�����������������������������Զ������Ϣ�������
	ON_BN_CLICKED(IDC_PROCESS_PROTECT, &CActiveDefenseDlg::OnBnClickedProcessProtect)
	ON_BN_CLICKED(IDC_LOAD_FILTER, &CActiveDefenseDlg::OnBnClickedLoadFilter)
	ON_BN_CLICKED(IDC_PROCESS_FILTER, &CActiveDefenseDlg::OnBnClickedProcessFilter)
	ON_BN_CLICKED(IDC_DRIVER_FILTER, &CActiveDefenseDlg::OnBnClickedDriverFilter)
	ON_BN_CLICKED(IDC_HELP, &CActiveDefenseDlg::OnBnClickedHelp)
END_MESSAGE_MAP()


// CActiveDefenseDlg ��Ϣ�������
BOOL CActiveDefenseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	ControlInit();//��ʼ�� ���� �ؼ� 
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CActiveDefenseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CActiveDefenseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CActiveDefenseDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rctClient;
	GetClientRect(rctClient);
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap m_bmp;
	m_bmp.LoadBitmap(IDB_BACK);
	BITMAP bitmap;
	m_bmp.GetBitmap(&bitmap);
	CBitmap   *pbmpOld = dcMem.SelectObject(&m_bmp);
	CString buff;
	pDC->StretchBlt(0, 0, rctClient.Width(), rctClient.Height(), &dcMem, 0, 0,
		bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);//���Ʊ���

	/*������� ����*/
	CFont font;
	font.CreatePointFont(125, L"΢���ź�");

	CDC*pdc = GetDC();
	pdc->SetBkMode(TRANSPARENT);
	pdc->SelectObject(&font);
	pdc->SetTextColor(RGB(255, 255, 255));
	pdc->TextOutW(20, 7, SOFT_CAPTION);
	
	ReleaseDC(pdc);
	/**/
	return true;
}

LRESULT CActiveDefenseDlg::OnNcHitTest(CPoint point)
{
	UINT nRet = CDialog::OnNcHitTest(point);
	if (HTCLIENT == nRet)
	{
		ScreenToClient(&point);
	//	if (point.y<34 && point.x<415)
	//	{
			return HTCAPTION;
	//	}
	}
	return nRet;
}
// ˢ�£���ʹ�ı��� ������ ��׶���ʾ
void CActiveDefenseDlg::EditBoxToBottom(void)
{
	UpdateData(false);
	CEdit* pedit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pedit->LineScroll(pedit->GetLineCount());
}
void CActiveDefenseDlg::ShowInText(CString *str)
{
	m_EDIT_STR += *str;
	m_EDIT_STR += "\r\n";
	EditBoxToBottom();
}
LRESULT CActiveDefenseDlg::OnShowMsg(WPARAM wParam, LPARAM lParam)
{
	ShowInText((CString *)wParam);
	return 0;
}

// �ؼ���Ϣ��ʼ��
void CActiveDefenseDlg::ControlInit()
{
	CRect rect;
	GetWindowRect(&rect);
	btn_close.MoveWindow(rect.Width() - 43,3, 40, 29);//���ð�ťλ�úʹ�С
	btn_close.Load(IDP_CLOSE, 45);//���밴ťͼƬ

	btn_min.Load(IDP_MIN, 31);//���밴ťͼƬ
	btn_min.MoveWindow(rect.Width() - 43 - 39, 3, 40, 29);//���ð�ťλ�úʹ�С
}


void CActiveDefenseDlg::OnBnClickedClose()
{
	OnBnClickedUnloadDrv();
	//g_isProcessOver = 1;//�ر��߳�
	ExitProcess(0);
	//EndDialog(0);//�رմ���
	/*if (IDYES == MessageBox(L"�����Ҫ�˳���������",L"ϵͳ��ʾ", MB_ICONQUESTION | MB_YESNO))
	{
		EndDialog(0);//�رմ���
	}*/
}


void CActiveDefenseDlg::OnBnClickedMin()
{
	ShowWindow(SW_SHOWMINIMIZED);//��С��
}

void GetAppPath(TCHAR *szCurFile, TCHAR *szSysName) //���ر���Ŀ¼��sys·��
{
	GetModuleFileNameW(0, szCurFile, MAX_PATH);
	
	for (SIZE_T i = wcslen(szCurFile) - 1; i >= 0; i--)
	{
		if (szCurFile[i] == '\\')
		{
			szCurFile[i + 1] = '\0';
			break;
		}
	}
	wcscat(szCurFile, szSysName);
	
	//MessageBox(NULL,szCurFile,NULL,NULL);
}




void StartThread()
{
	//�����߳�
	if (g_ThreadNum < 1)//�߳�����С��һ��
	{
		g_isProcessOver = 0;//�߳̿�����־λ
		g_ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadHandle, NULL, 0, NULL);
	}
	else
	{
		ShowInfoInDlg (L"ֻ������һ������߳�");
	}
	
}

void CActiveDefenseDlg::OnBnClickedLoadDrv()
{
	TCHAR szFullPath[256];
	GetAppPath(szFullPath, L"DefenseDriver.sys");

	BOOLEAN bRet;
	bRet=operaType(szFullPath, szTitle, 0);
	if (!bRet)
	{
		return;
	}
		
	bRet = operaType(szFullPath, szTitle, 1);
	if (!bRet)
	{
		return;
	}

	//�����߳�
	StartThread();
}


int SendDrvClose()//���͸��豸CLOSE�źţ������̣߳���ֹ�߳��������ں˲�
{
	HANDLE device = NULL;
	ULONG ret_len;

	//���豸
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		ShowInfoInDlg(L"���̣߳��豸�򿪴����߳��˳�\r\n----------------------------------------------");
		return -1;
	}
	if (!DeviceIoControl(device, CTL_CODE_GEN(0x913), NULL, NULL, NULL, 0, &ret_len, 0))//����
	{
		ShowInfoInDlg(L"���̣߳�������������Ϣʧ��");
	}

	CloseHandle(device);
	return 0;
}
void CActiveDefenseDlg::OnBnClickedUnloadDrv()
{
	TCHAR szFullPath[256];
	GetAppPath(szFullPath, L"DefenseDriver.sys");
	g_isProcessOver = 1;//�߳��˳���־λ
	SendDrvClose();
	
	operaType(szFullPath, szTitle, 2);
	//operaType(szFullPath, szTitle, 3);
	StopDriver(L"FsFilter");
}




BOOL CActiveDefenseDlg::PreTranslateMessage(MSG* pMsg)
{
	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	else
	return CDialogEx::PreTranslateMessage(pMsg);
}


int StartProcessProtect()//������������Ϣ�������̱���
{
	int nReturn = 0;
	HANDLE device = NULL;
	ULONG ret_len;
	char Buffer[MAX_PATH];
	memset(Buffer, 0, MAX_PATH);

	//UNICODE תANSI
	USHORT *pShort = (USHORT *)AfxGetApp()->m_pszExeName;
	UCHAR *pChr = (UCHAR *)Buffer;
	while (*pShort != 0)
	{
		*pChr = *pShort;
		pChr++;
		pShort++;
	}
	strcat(Buffer, ".exe");
	if (strlen(Buffer) >= 14)//����14���ֽھͽ�ȡ
	{

		Buffer[14] = 0;
	}

	//���豸
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		ShowInfoInDlg(L"���̣߳��豸�򿪴���\r\n----------------------------------------------");
		return -1;
	}
	if (!DeviceIoControl(device, CTL_CODE_GEN(0x914), Buffer, sizeof(Buffer), NULL, 0, &ret_len, 0))//����
	{

		ShowInfoInDlg(L"���̣߳�������������Ϣʧ��");
		nReturn = -1;
	}
	if (ret_len == 0)
	{
		ShowInfoInDlg(L"���̱��� ��ȷ����,���Գ��Խ���������==\r\n----------------------------------------------");
		nReturn = 0;
	}
	else
	{
		ShowInfoInDlg(L"���̱��� ����ʧ��\r\n---------------------------------------------");
		nReturn = -1;
	}

	CloseHandle(device);
	return nReturn;
}
void CActiveDefenseDlg::OnBnClickedProcessProtect()
{
	CString caption;
	GetDlgItem(IDC_PROCESS_PROTECT)->GetWindowText(caption);//��ȡ��ť����
	if (caption == L"�������̱���")
	{
		int nRet = StartProcessProtect();
		if (nRet == 0)//�ɹ���������
		{
			GetDlgItem(IDC_PROCESS_PROTECT)->SetWindowText(L"���̱�����");
		}
	}
	else
	{
		ShowInfoInDlg(L"������ ���� ��������==\r\n----------------------------------------------");
	}

}

void CreateProtectFile()
{
	DeleteFile(L"README.TXT");
	CFile file;
	char *szContent = "�㲢�����޸� it\n";
	if (file.Open(L"README.TXT", CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL))
	{
		file.Write(szContent, strlen(szContent));
		file.Close();
	}

}
 
void CActiveDefenseDlg::OnBnClickedLoadFilter()
{
	CString caption;
	GetDlgItem(IDC_LOAD_FILTER)->GetWindowText(caption);//��ȡ��ť����
	if (caption == L"�����ļ�����")
	{
		CreateProtectFile();
		CFileFind fFind;
		if (fFind.FindFile(L"FsFilter.sys"))
		{
			ShowInfoInDlg(L"��飺�ļ�������������");
		}
		else
		{
			ShowInfoInDlg(L"�ļ��������������ڣ������ļ���\r\n----------------------------------------------");
			return;
		}
		BOOLEAN bRet;
		
	//InstallDriver("HelloDDK", ".\\HelloDDK.sys", "370030"/*Altitude*/);
		//��������
		bRet = InstallDriver("FsFilter", ".\\FsFilter.sys", "370030");
		if (bRet)
		{
			ShowInfoInDlg(L"�ļ������������سɹ�==");
		}
		else
		{
			ShowInfoInDlg(L"�ļ�������������ʧ��==");
			return;
		}
		//��������
		bRet = StartDriver (L"FsFilter");
		if (bRet)
		{
			ShowInfoInDlg(L"�ļ��������������ɹ�==\r\n----------------------------------------------");
			ShowInfoInDlg(L"��Ŀ¼��README.TXT���������ļ�\r\n----------------------------------------------");
		}
		else
		{
			ShowInfoInDlg(L"�ļ�������������ʧ��==\r\n----------------------------------------------");
			return;
		}

		GetDlgItem(IDC_LOAD_FILTER)->SetWindowText(L"�ر��ļ�����");
	}
	else
	{
		StopDriver(L"FsFilter");
		ShowInfoInDlg(L"�ļ����������Ѿ��ر�\r\n----------------------------------------------");
		GetDlgItem(IDC_LOAD_FILTER)->SetWindowText(L"�����ļ�����");
	}
	
}


void CActiveDefenseDlg::OnBnClickedProcessFilter()
{
	CString caption;
	GetDlgItem(IDC_PROCESS_FILTER)->GetWindowText(caption);//��ȡ��ť����
	if (caption == L"�������̼��")
	{
		int nRet = SendMsgToDriver(IOCTL_PROCESS_FILTER);
		if (nRet==0)
		{
			ShowInfoInDlg(L"�������̼�سɹ�==\r\n----------------------------------------------");
			GetDlgItem(IDC_PROCESS_FILTER)->SetWindowText(L"�رս��̼��");
		}
		else
		{
			ShowInfoInDlg(L"�������̼��ʧ��==\r\n----------------------------------------------");
		}
			
	}
	else
	{
		int nRet = SendMsgToDriver(IOCTL_PROCESS_UNFILTER);
		if (nRet==0)
		{
			ShowInfoInDlg(L"�رս��̼�سɹ�==\r\n----------------------------------------------");
			GetDlgItem(IDC_PROCESS_FILTER)->SetWindowText(L"�������̼��");
		}
		else
		{
			ShowInfoInDlg(L"�رս��̼��ʧ��==\r\n----------------------------------------------");
		}
		
	}
}


void CActiveDefenseDlg::OnBnClickedDriverFilter()
{
	CString caption;
	GetDlgItem(IDC_DRIVER_FILTER)->GetWindowText(caption);//��ȡ��ť����
	if (caption == L"�����������")
	{
		int nRet = SendMsgToDriver(IOCTL_DRIVER_FILTER);
		if (nRet == 0)
		{
			ShowInfoInDlg(L"����������� �ɹ�==\r\n----------------------------------------------");
			GetDlgItem(IDC_DRIVER_FILTER)->SetWindowText(L"�ر��������");
		}
		else
		{
			ShowInfoInDlg(L"����������� ʧ��==\r\n----------------------------------------------");
		}

	}
	else
	{
		int nRet = SendMsgToDriver(IOCTL_DRIVER_UNFILTER);
		if (nRet == 0)
		{
			ShowInfoInDlg(L"�ر�������� �ɹ�==\r\n----------------------------------------------");
			GetDlgItem(IDC_DRIVER_FILTER)->SetWindowText(L"�����������");
		}
		else
		{
			ShowInfoInDlg(L"�ر��������ʧ��==\r\n----------------------------------------------");
		}

	}
}


void CActiveDefenseDlg::OnBnClickedHelp()
{
	CString temp;
	temp = "1.������Win7 64λ�µ���ͨ������δ����ǩ�����������ʧ������� ����ǿ����������ǩ�� ģʽ�������´򿪳���\n";
	temp += "2.���̼����ͨ��ע��ص���������ؽ��̵���Ϊ��\n";
	temp += "3.���̱�������DKOM��ʽ����EPROCESS�޸�FLAGS��־λʵ�ֽ��̱�����\n";
	temp += "4.ʹ��MiniFilter��ܹ����ļ���ʵ���ļ�������\n";
	temp += "5.ͨ������ӳ�����ͨ�����̣�ʵ���������صļ�ء�\n";
	MessageBox(temp, L"���˵��", 0);
}
