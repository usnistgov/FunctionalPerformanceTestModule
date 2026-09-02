#pragma once

#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <time.h>
#include "InterpolateValue.h"

//#include "FPvar.h"
// MAG 210607 add class and related code to support pass/fail of scripts
// MAG 250129 add triggerTime to support Taylor/CalNEXT scripts. 
// Purpose: Usually test value from [varIndex] against 'value', using 'relate'
// Variation: if 'usePrevV1' is set to 1, comparison is 'value' 'relate' 'prevValue' (used for =LAST and similar evaluations)
class TestList {
public:
	TestList();

	void			SetValue(double val) { /*prevValue = value;*/ value = val; }; //MAG 251218 NOTE prevValue is value at end of previous test step.
	const double	GetValue() { return value; };
	void			BumpLastValue() { prevValue = value; };

	void			SetVarIndex(int ind);
	const int		GetVarIndex() { return varIndex; };

	void			SetRelate(int rel);
	const int		GetRelate() { return relate; };

	CString			Label;	// description of test, or requirements for manual (relate == 4)

	void			SetTriggerEvent(int val);
	const int		GetTriggerEvent() { return triggerEvent; };

	void			SetResult(int val);
	void			SetResult(int val, double v1, double v2);
	const int		GetResult() { return result; };

	void			SetLevel(int val) { if (val < 0) return; level = val; };
	const int		GetLevel() { return level; };

	void			SetTriggerTime(CTime val) { triggerTime = val; };
	const CTime		GetTriggerTime() { return triggerTime; };
	void			AdjustTriggerTime(CTimeSpan cts, int dec = 0) { if (dec == 0) triggerTime += cts; else triggerTime -= cts; };
	void			Reset();
	CString			GetTriggerTimeString() {
		CString cs1; 
		cs1.Format(L"%02d/%02d/%02d %02d:%02d:%02d\t", triggerTime.GetMonth(), triggerTime.GetDay(), triggerTime.GetYear() - 2000, triggerTime.GetHour(), triggerTime.GetMinute(), triggerTime.GetSecond()); 
		return cs1;
	};

	const double	GetEvalV1(){ return evalV1;};
	const double	GetEvalV2(){ return evalV2;};

	void			SetRunTime(CTime val) { runTime = val; };
	const CTime		GetRunTime() { return runTime; };
	CString			GetRunTimeString() {
		CString cs1; 
		cs1.Format(L"%02d/%02d/%02d %02d:%02d:%02d\t", runTime.GetMonth(), runTime.GetDay(), runTime.GetYear() - 2000, runTime.GetHour(), runTime.GetMinute(), runTime.GetSecond()); 
		return cs1;
	};

	void	SetSourceColumn(int scol) { if (scol < 0) return; sourceColumn = scol; };
	int		GetSourceColumn() { return sourceColumn; };

	void	SetSourceRow(int srow) { if (srow < 0) return; sourceRow = srow; };
	int		GetSourceRow() { return sourceRow; };

	void			SetUsePrevV1(int val) { usePrevV1 = val; };
	const int		GetUsePrevV1() { return usePrevV1; }

	const double	GetPrevV1() { return prevValue; };

	std::vector<std::string> testNotes;

	InterpolateValue myInterpolate;
protected:
	int		varIndex;		// Variable array index to retrieve test value from
	double  value;			// test against this value
	double	prevValue;		// previous value at end of last test step, used when usePrevV1 is nonzero, set when LAST is used
	int		level;			// 0 for primary test, +1 for secondary level (i.e. save value as last time cycle in Taylor test scripts) MAG  241220
	int		relate;			// -2 for LT, -1 for LTE, 0 for EQ, +1 for GTE, +2 for GT, 3 for NE (!=), 4 for manual review, 5 for ANY
	int		triggerEvent;	// MAG 221109 add this variable. Value is -1 for after all events (default), or value is index into event list
	CTime	triggerTime;	// MAG 250129 add this to support eventTimerTestList for Taylor/CalNEXT 
	CTime	runTime;		// MAG 250402 add to save time test is run, used for reporting
	int		result;			// MAG 221109 -1 for not analyzed, 0 for fail, 1 for pass
	int		testStep;		// MAG 250131 aligns with testStep for timedRequest events.
	double	evalV1;			// MAG 250326 LVAL set upon evaluation.
	double	evalV2;			// MAG 250326 RVAL set upon evaluation.
	int		usePrevV1;		// MAG 250408 for >LAST evaluations, 0=not set, 1=set
	int		sourceColumn;	// MAG 251118 column number in section (A, B, ...) in config file, for print so base index=1
	int		sourceRow;		// MAG 251118 row number in section (A, B, ...) and sourceColumn in config file, for print so base index=1
};

// MAG 250130
// This is designed to support Taylor/CalNEXT input files, conditions for evaluation of test step where the second value is also a variable
// Note- 'previous value' not needed for dual var.
// Note- Also used for threshold evaluations
class DualVarTestList : public TestList
{
public:
	DualVarTestList() : TestList() { varIndex2 = -1; usevar2 = 0; };
	void			SetVarIndex2(int ind);		// set to <0 to reset
	const int		GetVarIndex2() { return varIndex2; };
	void			Reset() { varIndex2 = -1; usevar2 = 0; TestList::Reset(); };

protected:
	int		varIndex2;
	int		usevar2;
};