#include "stdafx.h"
#include "BTMline.h"
#include "FPTM App.h"


// id: line number on display
BTMline::BTMline(int id, int width)
{
	int colw = (int)( (width) / 21.0);
	int base = 75;
	int lheight = 26;
	int i = 0;
	int y = base + (id * lheight);
	
	CRect		cr = CRect(0,y,colw,y+lheight);
	CString		cs1;
	CString		ds2;
	CWnd		*mwin = NULL;	// pointer to main window
	CWinApp		*mapp = NULL;// pointer to theApp

	mwin = AfxGetMainWnd();
	mapp = AfxGetApp();

	lineID = id;
	displayID = id;  // should be same when init

	// Line
	line = new CEditDevice(id,1);
	line->Create(ES_NUMBER | ES_CENTER | WS_VISIBLE | WS_BORDER | ES_READONLY, cr, mwin, 1000 + (id * 100) + 1);
	line->EnableWindow(1);
	cs1.Format(L"%d", id);
	line->SetWindowText(cs1);
	line->SetReadOnly(1);

	// Date
	cr = CRect(colw, y, 3 * colw, y + lheight);
	date = new CMDateTimeCtrl(id);
	date->Create(WS_CHILD | WS_VISIBLE, cr, mwin, 1000 + (id * 100) + 2);

	// Time
	cr = CRect(3*colw, y, 5 * colw, y + lheight);
	time = new CMDateTimeCtrl(id);
	time->Create(WS_CHILD | WS_VISIBLE | DTS_TIMEFORMAT, cr, mwin, 1000 + (id * 100) + 3);
	time->SetFormat(L"HH:mm:ss");

	// Mode
	cr = CRect(5 * colw, y, 7 * colw, y + lheight);
	mode = new CMComboBox(id,0);
	mode->Create(WS_CHILD | WS_VISIBLE | ES_CENTER | CBS_DROPDOWNLIST, cr, mwin, 1000 + (id * 100) + 4);
	mode->AddString(L"READ");
	mode->AddString(L"WRITE");
	mode->AddString(L"RESET");
	mode->SetCurSel(0);

	// Device
	cr = CRect(7 * colw, y, 11 * colw, y + lheight);
	device = new CMComboBox(id, 1);
	device->Create(WS_CHILD | WS_VISIBLE | ES_CENTER | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL, cr, mwin, 1000 + (id * 100) + 5);
	device->ModifyStyle(CBS_SORT, 0, 0);

	device->EnableWindow(1);
	device->AddString(L"Select Device");
	i = device->GetItemHeight(0);
	device->GetClientRect(&cr);
	device->SetWindowPos(NULL, 0, 0, cr.right, i * 7, SWP_NOMOVE | SWP_NOZORDER);
	device->SetCurSel(0);
	device->SetDroppedWidth(6 * colw);

	// BACnet Object
	cr = CRect(11 * colw, y, 13 * colw, y + lheight);
	object = new CMComboBox(id,2);
	object->Create(WS_CHILD | WS_VISIBLE | ES_CENTER | CBS_DROPDOWNLIST, cr, mwin, 1000 + (id * 100) + 6);
	i = object->GetItemHeight(-1);
	object->EnableWindow(1);
	object->AddString(L"AI");
	object->AddString(L"AO");
	object->AddString(L"AV");
	object->AddString(L"BI");
	object->AddString(L"BO");
	object->AddString(L"BV");
	object->AddString(L"MI");
	object->AddString(L"MO");
	object->AddString(L"MV");
	i = object->GetItemHeight(0);
	object->GetClientRect(&cr);
	object->SetWindowPos(NULL, 0, 0, cr.right, i*7, SWP_NOMOVE | SWP_NOZORDER);
	object->SetDroppedWidth(6 * colw);
	object->SetCurSel(0);

	// Instance
	cr = CRect(13 * colw, y, 15 * colw, y + lheight);
	instance = new CEditDevice(id,2);
	instance->Create(ES_NUMBER | ES_CENTER | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_GROUP, cr, mwin, 1000 + (id * 100) + 7);
	instance->EnableWindow(1);
	instance->SetWindowText(L"0");
	if (id > 0) instance->SetWindowPos(theApp.m_BACdlg->mybtm[id - 1].instance, cr.left, cr.top, cr.Width(), cr.Height(), SWP_NOMOVE);

	// Value
	cr = CRect(15 * colw, y, 17 * colw, y + lheight);
	value = new CEditDevice(id,3);
	value->Create(ES_CENTER | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_GROUP, cr, mwin, 1000 + (id * 100) + 9);

	if (id > 0) value->SetWindowPos(theApp.m_BACdlg->mybtm[id-1].value, cr.left, cr.top, cr.Width(), cr.Height(), SWP_NOMOVE);
	value->EnableWindow(0);	// default is read value
	value->SetWindowText(L"-999.00");

	// Comment
	cr = CRect(17 * colw, y, 21 * colw, y + lheight);
	comment = new CEditDevice(id,4);
	comment->Create(ES_LEFT | ES_LEFT | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_GROUP | ES_AUTOHSCROLL, cr, mwin, 1000 + (id * 100) + 10);
	comment->EnableWindow(1);
	comment->SetWindowText(L"Enter comment");
	if (id > 0) comment->SetWindowPos(theApp.m_BACdlg->mybtm[id - 1].comment, cr.left, cr.top, cr.Width(), cr.Height(), SWP_NOMOVE);

#ifdef USEAFDD
	SetStart();  // this is just to disable user input
#endif

	return;
}  // end BTMline(int id, int width)


BTMline::BTMline()
{
	lineID = 0;
	displayID = 0;
	startTime = 0;

	line = NULL;
	date = NULL;
	time = NULL;
	mode = NULL;
	device = NULL;
	object = NULL;
	instance = NULL;
	value = NULL;
	comment = NULL;
}


BTMline::~BTMline()
{
}


// MAG note- lineID is the place in the array- this should not change
// displayID is the index to the TRlist item currently displayed
void
BTMline::SetDisplayID(int id)
{
	CString tx;

	tx.Format(L"%d", id);
	line->SetWindowText(tx);
	displayID = id;
}


int
BTMline::GetDisplayID()
{
	return displayID;
}


void
BTMline::SetStart()
{
	date->EnableWindow(0);
	time->EnableWindow(0);
	mode->EnableWindow(0);		// read or write
	device->EnableWindow(0);	// mac / router
	object->EnableWindow(0);	// BACnet object
	instance->EnableWindow(0);
	value->EnableWindow(0);
}


void
BTMline::SetStop()
{
#ifndef AFDD	// MAG controls are never enabled in AFDD version
	date->EnableWindow(1);
	time->EnableWindow(1);
	mode->EnableWindow(1);		// read or write
	device->EnableWindow(1);	// mac / router
	object->EnableWindow(1);	// BACnet object
	instance->EnableWindow(1);
	if(mode->GetCurSel() == 1) value->EnableWindow(1);
#endif
}


CTime
BTMline::GetActivationTime()
{
	return(startTime);
}


int
BTMline::AddDevice(CString ips)
{
	if (ips.GetLength() == 0) return(0);
	device->AddString(ips);

	return(1);
}


int
BTMline::RemoveDevices()
{
	while (device->GetCount() > 0) device->DeleteString(0);

	return(1);
}


int 
BTMline::SetActivationTime(CTime ntime)
{
	startTime = ntime;
	date->SetTime(&ntime);
	time->SetTime(&ntime);

	return 1;
}


int 
BTMline::GetActivationTime(CTime *ntime)
{
	if (ntime == NULL) return 0;

	*ntime = startTime;

	return 1;
}


// Returns the current value.  Could be value entered for write, value read from device, or -999.0
double
BTMline::GetValue()
{
	CString cs1;

	value->GetWindowText(cs1);

	return _ttof(cs1);
}


// Sets the value of the request
int 
BTMline::SetValue(double nval)
{
	CString cs1;

	cs1.Format(L"%5.2f", nval);
	value->SetWindowText(cs1);

	return 1;
}


// Changes display states to match read/write state.  0 = read, 1 = write, 2 = reset
int
BTMline::SetRW(int rw)
{
	CString cs1;

	if (rw == 0){// reading, disable value window
		value->EnableWindow(0);
	} else if (rw == 1){  // writing
		double v1;
		value->EnableWindow(1);
		v1 = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].GetValue();  // retrieve last value from TRlist for display
		cs1.Format(L"%5.2f", v1);
		value->SetWindowText(cs1);
	} else { // reset
		cs1 = "";
		value->SetWindowText(cs1);
		value->EnableWindow(0);
	}
	
	return 0;
}


// Returns the current line ID
int
BTMline::GetLine()
{
	return lineID;
}


// Sets visibility of referenced line
int 
BTMline::SetVisible(int isvis)
{
	int st=0;	// adjust this if needed for performance

	line->ShowWindow(isvis);
	Sleep(st);
	date->ShowWindow(isvis);
	Sleep(st);
	time->ShowWindow(isvis);
	Sleep(st);
	mode->ShowWindow(isvis);	// read or write - read=0, write=1
	Sleep(st);
	device->ShowWindow(isvis);	// mac / router
	Sleep(st);
	object->ShowWindow(isvis);	// BACnet object
	Sleep(st);
	instance->ShowWindow(isvis);
	Sleep(st);
	value->ShowWindow(isvis);
	if (isvis){
		value->EnableWindow(theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].GetRW());
	}
	Sleep(st);
	comment->ShowWindow(isvis);

	return 0;
}


// updates the display showing nline
int
BTMline::DisplayLine(int nline)
{
	TimedRequest *ntr = NULL;
	CString cs1;

	if (nline < 0) {
		return 0;
	}

	if (nline >= (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size()){
		return 0;
	}

	SetVisible(1);
	
	ntr = &theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][nline];

	if (ntr == NULL){
		return 0;
	}

	SetDisplayID(nline);
	SetActivationTime(ntr->GetEventTime());
	mode->SetCurSel(ntr->GetRW());
	device->SetCurSel(ntr->GetDevice()+1);
	object->SetObjectSel(ntr->GetBACnetObject());
	cs1.Format(L"%d", ntr->GetInstance());
	instance->SetWindowText(cs1);
	cs1.Format(L"%5.2f", ntr->GetValue());
	value->SetWindowText(cs1);
	comment->SetWindowText(ntr->GetComment());
		
	/* for reference
	CEditDevice		*line;
	CMDateTimeCtrl	*date;
	CMDateTimeCtrl	*time;
	CMComboBox		*mode;		// read or write - read=0, write=1
	CMComboBox		*device;	// mac / router
	CMComboBox		*object;	// BACnet object
	CEditDevice		*instance;
	//CEditDevice		*action;
	CEditDevice		*value;
	CEditDevice		*comment;
	*/
	
	return 0;
}


int 
BTMline::GetDeviceCount()
{
	return device->GetCount() - 1;  // -1 is because of the 'select device' line
}