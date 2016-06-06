
// ActiveDefenseDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "TraButton.h"
#include "DLEdit.h"

// CActiveDefenseDlg �Ի���
class CActiveDefenseDlg : public CDialogEx
{
// ����
public:
	CActiveDefenseDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ACTIVEDEFENSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	CTraButton btn_close;
	CTraButton btn_min;
	// �ؼ���Ϣ��ʼ��
	void ControlInit();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedMin();
	afx_msg void OnBnClickedLoadDrv();
};
