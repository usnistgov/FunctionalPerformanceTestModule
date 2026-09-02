#pragma once

#include "resource.h"
//#include "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\atlmfc\include\afxwin.h"
// MDialog dialog

class MDialog : public CDialog
{
	DECLARE_DYNAMIC(MDialog)

public:
	MDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~MDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();

	int choiceValue;// 1- delete task, 2- reset task, 0- no choice or cancel
	int taskID;
	CEdit m_tasklabel;
	
	virtual BOOL OnInitDialog();

	void SetTaskID(int id);
	virtual INT_PTR DoModal(int id);
};
