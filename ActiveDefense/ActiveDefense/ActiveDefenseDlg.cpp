
// ActiveDefenseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ActiveDefense.h"
#include "ActiveDefenseDlg.h"
#include "afxdialogex.h"
#include "ScmDrvCtrl.h"

#include "DriverContrl.h"
#pragma comment(lib,"user32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SOFT_CAPTION L"SML������������"
// CActiveDefenseDlg �Ի���



CActiveDefenseDlg::CActiveDefenseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActiveDefenseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CActiveDefenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, btn_close);
	DDX_Control(pDX, IDC_MIN, btn_min);
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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


//HRESULT CActiveDefenseDlg::accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
//{
//	// TODO:  �ڴ����ר�ô����/����û���
//
//	return CDialogEx::accHitTest(xLeft, yTop, pvarChild);
//}


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

void GetAppPath(char *szCurFile) //����б��
{
	GetModuleFileNameA(0, szCurFile, MAX_PATH);
	for (SIZE_T i = strlen(szCurFile) - 1; i >= 0; i--)
	{
		if (szCurFile[i] == '\\')
		{
			szCurFile[i + 1] = '\0';
			break;
		}
	}
}

TCHAR szFullPath[] = { L"C:\\Users\\Administrator\\DeskTop\\DefenseDriver.sys" };
TCHAR szTitle[MAX_PATH] = { L"DefenseDriver" };
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"
/*
// ��Ӧ�ò����������һ���ַ�����
#define  CWK_DVC_SEND_STR \
	(ULONG)CTL_CODE( \
	FILE_DEVICE_UNKNOWN, \
	0x911,METHOD_BUFFERED, \
	FILE_WRITE_DATA)

// ��������ȡһ���ַ���
#define  CWK_DVC_RECV_STR \
	(ULONG)CTL_CODE( \
	FILE_DEVICE_UNKNOWN, \
	0x912,METHOD_BUFFERED, \
	FILE_READ_DATA)
	*/
void CActiveDefenseDlg::OnBnClickedLoadDrv()
{
	operaType(szFullPath, szTitle, 0);
	operaType(szFullPath, szTitle, 1);
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
}


void CActiveDefenseDlg::OnBnClickedUnloadDrv()
{/*
	CString str;
	str.Format(L"hwnd:%d", dc.m_hService);
	MessageBox(str);

	//�رշ������Ӿ��
	CloseHandle(dc.m_hDriver);
	BOOL b;
	b = dc.Stop();
	if (b == true)
		MessageBox(L"����ֹͣ�ɹ�");
	else
	{
		str.Format(L"stop error:%d", dc.m_dwLastError);
		MessageBox(str); 
	}
	b = dc.Remove();
	if (b == true)
		MessageBox(L"����ж�سɹ�");
	else
	{
		str.Format(L"remove error:%d", dc.m_dwLastError);
		MessageBox(str);
	}
	*/

	
	operaType(szFullPath, szTitle, 2);
	operaType(szFullPath, szTitle, 3);
}
