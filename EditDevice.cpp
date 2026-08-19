// EditDevice.cpp : implementation file
//

#include "stdafx.h"
#include "FPTM App.h"
#include "EditDevice.h"
#include "mdialog.h"

// CEditDevice

IMPLEMENT_DYNAMIC(CEditDevice, CEdit)

CEditDevice::CEditDevice()
{
	typeID = 0;
	lineID = 0;
}


CEditDevice::CEditDevice(int nid, int ntype)
{
	lineID = nid;
	typeID = ntype;
}


CEditDevice::~CEditDevice()
{
}


BEGIN_MESSAGE_MAP(CEditDevice, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CEditDevice message handlers


void 
CEditDevice::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	int i = 1;
	int val = 0;
	CString cs;
	
	// the value is used by multiple cases
	GetWindowText(cs);
	val = _ttoi(cs);
	switch (typeID){
		case 1: // line ID, should not be changed manually
			cs.Format(L"%d", lineID+theApp.m_BACdlg->DisplayLineStart);
			// MAG TODO: verify this is actually needed functionality, maybe need to move things at the vector level
			break;
		case 2:	// instance
			if (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() == 0) break;// this happens when deleting the last event
			if (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].SetInstance(val) == 0){// fail-out of range
				i = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID].GetInstance();  // it hasn't changed
				cs.Format(L"%d", i);
				SetWindowText(cs);
			}
			break;
		case 3: // value
			double valf;
			valf = _ttof(cs);
			if (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() == 0) break;
			theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].SetValue(valf);
			break;
		case 4:	// comment
			GetWindowText(cs);
			if (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() == 0) break;
			theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].SetComment(cs);
			break;
		default:
			break;
	}

	return;
}


void 
CEditDevice::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEdit::OnLButtonDown(nFlags, point);
	MDialog mdg;
	int i = 0;
	int delID = -1;;

	// TODO: Add your message handler code here

	if (typeID != 1) return;

	CString cs1;
	cs1.Format(L"Functional Performance Test Module");

	delID = lineID + theApp.m_BACdlg->DisplayLineStart;

	mdg.DoModal(delID);
	i = mdg.choiceValue;

	if (i == 1){
		theApp.myFPTM.DeleteTask(delID);
	}
	if (i == 2){	// sending reset value
		theApp.myFPTM.RunReset(delID);
	}

	return;
}


void 
CEditDevice::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CString cs1;
	
	if (!((typeID == 2) || (typeID == 3))) return;  // only do for instance or value

	int delID = lineID + theApp.m_BACdlg->DisplayLineStart;
	if (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][delID].GetDevice() == -1){
		return;	// the set up is not finished, no valid device to send to
	}
	cs1.Format(L"Send Task %d now?", delID);
	int i = AfxMessageBox(cs1, MB_YESNO);
	if (i == IDYES){
		theApp.myFPTM.RunRequest(delID);
		return;
	}

	CEdit::OnLButtonDblClk(nFlags, point);
}
