
// ActiveDefenseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ActiveDefense.h"
#include "ActiveDefenseDlg.h"
#include "afxdialogex.h"

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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

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
	if (IDYES == MessageBox(L"�����Ҫ�˳���������",L"ϵͳ��ʾ", MB_ICONQUESTION | MB_YESNO))
	{
		EndDialog(0);//�رմ���
	}
}


void CActiveDefenseDlg::OnBnClickedMin()
{
	ShowWindow(SW_SHOWMINIMIZED);//��С��
}
