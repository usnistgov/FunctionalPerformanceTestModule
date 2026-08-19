// MDialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "MDialogEx.h"
#include "afxdialogex.h"


// MDialogEx dialog

IMPLEMENT_DYNAMIC(MDialogEx, CDialogEx)

MDialogEx::MDialogEx(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MDIALOGEX, pParent)
{
	choiceValue = 0;
}

MDialogEx::~MDialogEx()
{
}

void MDialogEx::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MDialogEx, CDialogEx)
	ON_BN_CLICKED(IDC_DELETE_TASK, &MDialogEx::OnBnClickedDeleteTask)
	ON_BN_CLICKED(IDCANCEL, &MDialogEx::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RESET_VALUE, &MDialogEx::OnBnClickedResetValue)
END_MESSAGE_MAP()


// MDialogEx message handlers


void MDialogEx::OnBnClickedDeleteTask()
{
	// TODO: Add your control notification handler code here
	choiceValue = 1;
}

void MDialogEx::OnBnClickedResetValue()
{
	// TODO: Add your control notification handler code here
	choiceValue = 2;
}

void MDialogEx::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	choiceValue = 0;
	CDialogEx::OnCancel();
}


