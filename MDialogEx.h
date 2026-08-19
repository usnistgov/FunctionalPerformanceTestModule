#pragma once

#include "resource.h"
// MDialogEx dialog

class MDialogEx : public CDialogEx
{
	DECLARE_DYNAMIC(MDialogEx)

public:
	MDialogEx(CWnd* pParent = NULL);   // standard constructor
	virtual ~MDialogEx();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MDIALOGEX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDeleteTask();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedResetValue();

	int choiceValue;// 1- delete task, 2- reset task, 0- no choice or cancel
};
