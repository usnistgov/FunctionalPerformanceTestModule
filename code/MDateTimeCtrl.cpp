// MDateTimeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FPTM App.h"
#include "MDateTimeCtrl.h"
#include "FPTM App.h"


// CMDateTimeCtrl

IMPLEMENT_DYNAMIC(CMDateTimeCtrl, CDateTimeCtrl)

CMDateTimeCtrl::CMDateTimeCtrl()
{

	lineID = 0;
	lastinc = 1;
}

CMDateTimeCtrl::~CMDateTimeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMDateTimeCtrl, CDateTimeCtrl)
	ON_NOTIFY_REFLECT(DTN_DATETIMECHANGE, &CMDateTimeCtrl::OnDtnDatetimechange)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CMDateTimeCtrl message handlers
#define SHIFTED 0x8000	// MAG 170323 used for getkeystate
void CMDateTimeCtrl::OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	int i = 0;
	CTime	evtime;		// event timd
	CTime	newtime;	// new time as set on controls
	CTimeSpan	cts;
	struct tm  ttime;	// = NULL;
	time_t		ttime2=0;

	if (lineID < 0) return;

	this->GetTime(newtime);
	evtime = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].GetEventTime();
	evtime.GetLocalTm(&ttime);

	if ((GetStyle() & DTS_TIMEFORMAT) == DTS_TIMEFORMAT){  // time format
		ttime.tm_hour = newtime.GetHour();
		ttime.tm_min = newtime.GetMinute();
	} else { // date format
		ttime.tm_mday = newtime.GetDay();
		ttime.tm_mon = newtime.GetMonth()-1;		// month numberint starts at 0 in struct tm
		ttime.tm_year = newtime.GetYear() - 1900;  // struct tm is from year 1900, GetYear returns 20XX
		ttime.tm_wday = 0;
		ttime.tm_yday = 0;
	}
	ttime2 = mktime(&ttime);
	newtime = CTime(ttime2);
	cts = newtime - evtime; // add this to other lines if requested

	i = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].SetEventTime(newtime);
	if (i == 0){  // there was a problem, set the time in the display back to prev value.
		evtime = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].GetEventTime();
		this->SetTime(&newtime);
		return;
	}

	int nret = GetKeyState(VK_SHIFT);
	if (nret & SHIFTED) {
		// MAG TODO?
		// propagate change to lower datetimectrls
		nret = 0;// debug breakpoint only
		for (i = lineID + 1 + theApp.m_BACdlg->DisplayLineStart; i < (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size(); i++)
		{
			newtime = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][i].GetEventTime();
			newtime += cts;
			theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][i].SetEventTime(newtime);
			//if(i < 10) theApp.m_BACdlg->mybtm[i].SetRW(cur);
		}
		theApp.m_BACdlg->UpdateEvents();
	}

	*pResult = 0;

	return;
}

CMDateTimeCtrl::CMDateTimeCtrl(int lid)
{
	lineID = lid;
	if (lineID < 0) lineID = 0;
	lastinc = 1;
}


// MAG Function handles right clicks on date or time boxes
// right clicking on date fills that date into subsequent date boxes
// time boxes have two modes- incrementing and non-incrementing
// NOTE edge case- when events span more than one day, right clicking date fills clicked date
// without accounting for changes in date- this is the intended behavior.
void CMDateTimeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	int i;
	int timeLine;	// 1- I'm a time display  0- I'm a date display
	CTime newtime;
	CTime starttime;  // used for date changes, tracks time on clicked line
	CTimeSpan delt = 0;
	int startday;	// stores the calendar day of the clicked line
	int j=1;
	struct tm tnewtime;
	
	timeLine = (GetStyle() & DTS_TIMEFORMAT) == DTS_TIMEFORMAT;  // time format
	if (timeLine) {
		if (lastinc == 0) {
			lastinc = 1;
			delt = CTimeSpan(0, 0, 1, 0);
		}
		else {
			lastinc = 0;
			delt = CTimeSpan(0, 0, 0, 0);
		}
	}

	newtime = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][lineID + theApp.m_BACdlg->DisplayLineStart].GetEventTime();
	starttime = newtime;
	startday = starttime.GetDay();

	for (i = lineID + 1 + theApp.m_BACdlg->DisplayLineStart; i < (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size(); i++) {
		if ((timeLine == 1)&&(lastinc == 1)) {
			delt = CTimeSpan(0, 0, j, 0);
			j++;
		}
		if (timeLine == 0) {	// read the time off each line before changing the date, time should be preserved
			newtime = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][i].GetEventTime();
			newtime.GetLocalTm(&tnewtime);
			newtime = CTime(starttime.GetYear(), starttime.GetMonth(), starttime.GetDay(), tnewtime.tm_hour, tnewtime.tm_min, 0, tnewtime.tm_isdst);
		}
		theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][i].SetEventTime(newtime + delt);
	}
	theApp.m_BACdlg->UpdateEvents();

	return;
}
