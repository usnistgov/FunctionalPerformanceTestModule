// MComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "FPTM App.h"
#include "MComboBox.h"


// CMComboBox

IMPLEMENT_DYNAMIC(CMComboBox, CComboBox)

CMComboBox::CMComboBox()
{

	lineID = 0;
}

CMComboBox::~CMComboBox()
{
}


BEGIN_MESSAGE_MAP(CMComboBox, CComboBox)
//	ON_WM_USERCHANGED()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CMComboBox::OnCbnSelchange)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CMComboBox message handlers

void CMComboBox::OnCbnSelchange()
{
	// TODO: Add your control notification handler code here
	int i = GetCurSel();
	switch (typeID)
	{
		case 0:	// change read/write
			theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].SetRW(i);
			theApp.m_BACdlg->mybtm[lineID].SetRW(i);
			break;
		//MAG need to finish these cases?
		case 1:	// change BACnet device
			theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].SetDevice(i - 1);
			break;
		case 2: // change BACnet object
			theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].SetBACnetObjectIndex(i);
			break;
		default:
			break;
	}
	
}


// Constructor with line ID
CMComboBox::CMComboBox(int lid, int ntype)
{
	lineID = lid;
	typeID = ntype;
}


void 
CMComboBox::OnRButtonDown(UINT nFlags, CPoint point)
{
	int i = 0;
	int cur = 0;
	int objType = 0;

	switch (typeID){
		case 0: // read/write
			cur = GetCurSel();
			for (i = lineID + 1 + theApp.m_BACdlg->DisplayLineStart; i < (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size(); i++){
				theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][i].SetRW(cur);
			}
			theApp.m_BACdlg->UpdateEvents();
			break;
		case 1: // device list
			cur = GetCurSel();
			if (cur == 0){
				return;
			}
			cur--;  // offset for label line
			for (i = lineID + 1 + theApp.m_BACdlg->DisplayLineStart; i < (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size(); i++){
				theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][i].SetDevice(cur);
			}
			theApp.m_BACdlg->UpdateEvents();
			break;
		case 2: // object type
			cur = GetCurSel();
			// use selection to get object type- not a direct mapping for all objects
			switch (cur){
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5: objType = cur; break;
				case 6: objType = 13; break;	// multi-state input
				case 7: objType = 14; break;	// multi-state output
				case 8: objType = 19; break;	// multi-state value
				default:
					return; // MAG this should never happen
				break;
			}
			for (i = lineID + 1 + theApp.m_BACdlg->DisplayLineStart; i < (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size(); i++){
				theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][i].SetBACnetObject(objType);
			}
			theApp.m_BACdlg->UpdateEvents();
			break;
		default:
			break;
	}

	return;
}


void
CMComboBox::SetObjectSel(int obj){
	int i = 0;

	switch (obj){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: i = obj; break;
		case 13: i = 6; break;	// multi-state input
		case 14: i = 7; break;	// multi-state output
		case 19: i = 8; break;	// multi-state value
		default:
			SetCurSel(0);
			return; // MAG this should never happen
		break;
	}
	SetCurSel(i);

	return;
}
