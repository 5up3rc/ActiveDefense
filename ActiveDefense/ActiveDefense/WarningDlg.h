#pragma once
#include "resource.h"

// WarningDlg �Ի���

class WarningDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WarningDlg)

public:
	WarningDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~WarningDlg();

// �Ի�������
	enum { IDD = IDD_WARING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAccepct();
	afx_msg void OnBnClickedRefuse();
};
