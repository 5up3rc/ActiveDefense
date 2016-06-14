// WarningDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ActiveDefense.h"
#include "WarningDlg.h"
#include "afxdialogex.h"

extern struct EventStrInfo g_EventStrInfo;
// WarningDlg �Ի���

IMPLEMENT_DYNAMIC(WarningDlg, CDialogEx)

WarningDlg::WarningDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(WarningDlg::IDD, pParent)
{

}

WarningDlg::~WarningDlg()
{
}

void WarningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(WarningDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_ACCEPCT, &WarningDlg::OnBnClickedAccepct)
	ON_BN_CLICKED(IDC_REFUSE, &WarningDlg::OnBnClickedRefuse)
END_MESSAGE_MAP()


// WarningDlg ��Ϣ�������


BOOL WarningDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rctClient;
	GetClientRect(rctClient);
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap m_bmp;
	m_bmp.LoadBitmap(IDB_BACK2);
	BITMAP bitmap;
	m_bmp.GetBitmap(&bitmap);
	CBitmap   *pbmpOld = dcMem.SelectObject(&m_bmp);
	CString buff;
	pDC->StretchBlt(0, 0, rctClient.Width(), rctClient.Height(), &dcMem, 0, 0,
		bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);//���Ʊ���

	/*������� ����*/
	CFont font;
	font.CreatePointFont(120, L"΢���ź�");

	CDC*pdc = GetDC();
	pdc->SetBkMode(TRANSPARENT);
	pdc->SelectObject(&font);
	pdc->SetTextColor(RGB(255, 255, 255));
	//pdc->TextOutW(20, 7, L"Υ���¼�");

	//�����Ϣ
	pdc->TextOutW(20, 20, g_EventStrInfo.szCaption); 
	pdc->TextOutW(20, 50, g_EventStrInfo.szType);
	pdc->SetTextColor(RGB(0, 0, 200));
	pdc->TextOutW(20, 97, g_EventStrInfo.szName);
	pdc->SetTextColor(RGB(0, 0, 0));
	pdc->TextOutW(20, 127, g_EventStrInfo.szDescribe);

	int len = g_EventStrInfo.szPath.GetLength();
	int line = 50;
	if (len > line)
	{
		pdc->TextOutW(20, 157, g_EventStrInfo.szPath.Mid(0, line));
		if (len > 2 * line)
		{
			pdc->TextOutW(20, 177, g_EventStrInfo.szPath.Mid(line, line-4)+L" ....");
		}
		else
		{
			pdc->TextOutW(20, 177, g_EventStrInfo.szPath.Mid(line, len - line));
		}
		
	}
	else
	{
		pdc->TextOutW(20, 157, g_EventStrInfo.szPath);
	}
	
	
	ReleaseDC(pdc);
	/**/
	return true;
}


LRESULT WarningDlg::OnNcHitTest(CPoint point)
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


BOOL WarningDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������С
	CRect rectWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, SPIF_SENDCHANGE);

	// ��öԻ����С
	CRect rectDlg;
	GetWindowRect(&rectDlg);
	int nW = rectDlg.Width();
	int nH = rectDlg.Height();

	// ���������õ����½�
	::SetWindowPos(GetSafeHwnd(), HWND_BOTTOM, rectWorkArea.right - nW-5, rectWorkArea.bottom - nH, nW, nH, SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control

}


void WarningDlg::OnBnClickedAccepct()
{
	EndDialog(0);
}


void WarningDlg::OnBnClickedRefuse()
{
	EndDialog(1);
}
