
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

#define SOFT_CAPTION L"SML������������"
// CActiveDefenseDlg �Ի���

extern bool g_isProcessOver;//ָʾ�����Ƿ�Ҫ�˳�
extern int g_ThreadNum;//��¼�̸߳���
HANDLE g_ThreadHandle;
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
	ON_BN_CLICKED(IDC_START_THREAD, &CActiveDefenseDlg::OnBnClickedStartThread)
	ON_MESSAGE(WM_SHOW_MSG, &CActiveDefenseDlg::OnShowMsg)    // OnCountMsg���Զ������Ϣ�����������������������������Զ������Ϣ�������
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


TCHAR szTitle[MAX_PATH] = { L"DefenseDriver" };
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"


void CActiveDefenseDlg::OnBnClickedLoadDrv()
{
	TCHAR szFullPath[256];
	GetAppPath(szFullPath, L"DefenseDriver.sys");

	operaType(szFullPath, szTitle, 0);
	operaType(szFullPath, szTitle, 1);
	/*
	char *msg = { "Hello driver, this is a message from app.\r\n" };
	char str[100];
	ULONG ret_len;
	HANDLE device = NULL;
	//���豸
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		MessageBox( L"�豸�򿪴���");
		return;
	}


	if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), msg, strlen(msg) + 1, NULL, 0, &ret_len, 0))
	{
		MessageBox(L"���ʹ���");
		return;
	}
	if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), msg, strlen(msg) + 1, str, sizeof(str), &ret_len, 0))
	{
		MessageBox(L"���մ���");
		return;
	}
	CloseHandle(device);
	CString Str_Temp(str);
	MessageBox(Str_Temp);
	*/
}

void StartThread()
{
	 g_ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadHandle, NULL, 0, NULL);
	//WaitForSingleObject(handle, INFINITE); //�ȴ��߳̽���
	//CloseHandle(handle);
}
int SendDrvClose()//���͸��豸CLOSE�źţ������̣߳���ֹ�߳��������ں˲�
{
	HANDLE device = NULL;
	ULONG ret_len;

	//���豸
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		ShowInfoInDlg(L"���̣߳��豸�򿪴����߳��˳�");
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
	operaType(szFullPath, szTitle, 3);
}


void CActiveDefenseDlg::OnBnClickedStartThread()
{
	if (g_ThreadNum < 1)//�߳�����С��һ��
	{
		g_isProcessOver = 0;//�߳̿�����־λ
		StartThread();
	}
	else
	{
		MessageBox(L"ֻ������һ������߳�");
	}
	
}




BOOL CActiveDefenseDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	else
	return CDialogEx::PreTranslateMessage(pMsg);
}
