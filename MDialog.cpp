// MDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MDialog.h"
#include "afxdialogex.h"


// MDialog dialog

IMPLEMENT_DYNAMIC(MDialog, CDialog)

MDialog::MDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MDIALOG, pParent)
{
	choiceValue = 0;
	taskID = 0;
}

MDialog::~MDialog()
{
}

void MDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tasklabel);
}

BOOL MDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString cs1;

	SetWindowText(L"Functional Performance Test Module- delete or reset task");
	// NOTE-taskID should be set by DoModal override function
	cs1.Format(L"Select a button below to delete task %d, to reset the value, or to cancel.", taskID);
	m_tasklabel.SetWindowText(cs1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

INT_PTR MDialog::DoModal(int id)
{
	// TODO: Add your specialized code here and/or call the base class

	taskID = id;
	return CDialog::DoModal();
}

// MAG not used- id is set in domodal override
void 
MDialog::SetTaskID(int id) 
{
	CString cs1;
	taskID = id;
	cs1.Format(L"Select a button below to delete task %d, to reset the value, or to cancel.", taskID);
	m_tasklabel.SetWindowText(cs1);
}

BEGIN_MESSAGE_MAP(MDialog, CDialog)
	ON_BN_CLICKED(IDOK, &MDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &MDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &MDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// MDialog message handlers

// delete event
void MDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	choiceValue = 1;
	CDialog::OnOK();
}

// reset value
void MDialog::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	choiceValue = 2;
	CDialog::OnOK();
}


void MDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	choiceValue = 0;
	CDialog::OnCancel();
}




