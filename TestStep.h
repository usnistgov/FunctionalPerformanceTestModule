#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <time.h>
#include <algorithm>

#include "MathVar.h"
#include "PeriodicInput.h"

// MAG 250203 Add Class TestStep
// Purpose: provide method to connect TimedRequest elements with a time and a test, to help align with
// Taylor/CalNEXT test steps.
// NOTE on timekeeping- if a TRelement is NOT in a TestStep it keep its own time and interval. When in
// a TestStep then TRelement time and interval are set to the values in the TestStep!
class TestStep
{
protected:
	std::vector<int>			trList; // array of indexes of TimedRequest elements (TRlist) in current test step
	CTime						tsTime; // test step time MAG should use trList[0] time!! no dup data
	CTimeSpan					tsInterval; // test step interval
	// MAG todo- should tsTime be a time span instead?
	std::string					label;
	int							tsTest;	// -1 if none, else index into eventTimerTestList
	int							status; // 0 = not run, 1 = has run
public:
	TestStep() { tsTest = -1; status = 0; }; //tsTime = 0; 
	
	std::vector<MathVar>		mvList; // list of MathVar to run during each teststep- should be protected, work on set/get functions
	std::vector<PeriodicInput>	piList;	// list of periodic inputs to run during each teststep, should also be protected...

	int		GetStatus() { return status; };
	void	SetStatus(int val);// { status = (val == 0, 0, 1); };
	void	CleartrList() { trList.clear(); };

	// MAG NOTE TODO: adjust time so time is only kept in TestStep, TRlist elements should be in a TestStep and reference it for time
	// xxx not ->it references time in trList[0]. Get rid of tsTime.
	void	SetTime(CTime ct);// { tsTime = ct; };
	CTime	GetTime() { return tsTime; }; // MAG should use GetListMinEvent, use that to get time if needed
	void	AdjustTime(CTimeSpan cts, int decr = 1);//// Note does not allow decrementing to before current time  { if (decr == 1) tsTime += cts; else tsTime -= cts; };
	CString	GetTimeString() {
		CString cs1; cs1.Format(L"%02d/%02d/%02d %02d:%02d:%02d\t", tsTime.GetMonth(), tsTime.GetDay(), tsTime.GetYear() - 2000, tsTime.GetHour(), tsTime.GetMinute(), tsTime.GetSecond()); return cs1;
	};

	void	SetTimeInterval(CTimeSpan ct) { tsInterval = (ct.GetTotalSeconds() >= 0) ? ct : 0; };  // negative values are set to 0
	CTimeSpan GetTimeInterval() { return tsInterval; };

	void	SetTest(int val) { tsTest = val; };
	int		GetTest() { return tsTest; };

	void	SetLabel(std::string val) {label = val; };
	std::string GetLabel() { return label; };

	std::vector<int> GetList();
	int		TestStep::CheckList(int val);
	void	AddToList(int val);
	size_t	GetListSize() { return trList.size(); };
	int		GetListEventID(int val) { if (val < 0) return -1; if ((unsigned)val >= trList.size()) return -1; return(trList[val]); };
	int		GetListMinEvent() { if (trList.size() == 0) return -1; return trList[0]; };
	int		GetListMaxEvent() { if (trList.size() == 0) return -1; return trList.back(); };

	void	Reset();
};

