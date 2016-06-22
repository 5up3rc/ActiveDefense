
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedMin();
	afx_msg void OnBnClickedLoadDrv();
	afx_msg void OnBnClickedUnloadDrv();
	afx_msg void OnBnClickedProcessProtect();
	afx_msg void OnBnClickedLoadFilter();
	afx_msg void OnBnClickedProcessFilter();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnShowMsg(WPARAM, LPARAM);

	CTraButton btn_close;
	CTraButton btn_min;
	CString m_EDIT_STR;

	// �ؼ���Ϣ��ʼ��
	void ControlInit();
	void EditBoxToBottom(void);
	void ShowInText(CString *str);

	
	

};
