#include "TestList.h"
#include "FPTM App.h"

extern CFPTMApp theApp;

TestList::TestList()
{
	varIndex = -1;
	value = 0.0;
	relate = -3;
	triggerEvent = -1;
	result = -1;
	level = 0;
	triggerTime = -1;
	Label = "";
	runTime = -1;
	usePrevV1 = 0;
	result = -1;
	testStep = -1;
	evalV1 = 0;
	evalV2 = 0;
	usePrevV1 = 0;
	prevValue = 0.0;
	sourceColumn = 0;
	sourceRow = 0;
}


void
TestList::SetRelate(int rel)
{
	if (rel < -2) return;
	if (rel > 5) return;
	relate = rel;

	return;
}


void
TestList::SetVarIndex(int ind)
{
	if (ind < 0) return;
	if (ind >= (int)theApp.myFPTM.VARlist.size()) return;
	varIndex = ind;

	return;
}


void
TestList::SetTriggerEvent(int var)
{
	if (var < -1) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Trigger event set out of range low. Reset to default.");
		triggerEvent = -1;
		return;
	}
	if (var > (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size()) { // NOTE not >= to prevent error when reading in script file, this is called before all TR are added to list.
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Trigger event set out of range high. Reset to default.");
		triggerEvent = -1;
		return;
	}

	triggerEvent = var;

	return;
}


void
TestList::SetResult(int var)
{
	if (var < -1) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Result set out of range low. Reset to default.");
		result = -1;
		return;
	}
	if (var > 1) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Result set out of range high. Reset to default.");
		result = -1;
		return;
	}

	result = var;

	return;
}


// allows evalV1 and evalV2 to be set
void
TestList::SetResult(int var, double v1, double v2)
{
	if (var < -1) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Result set out of range low. Reset to default.");
		result = -1;
		return;
	}
	if (var > 1) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Result set out of range high. Reset to default.");
		result = -1;
		return;
	}

	result = var;
	evalV1 = v1;
	evalV2 = v2;

	return;
}


void
TestList::Reset()
{
	varIndex = -1;
	value = 0.0;
	relate = -3;
	triggerEvent = -1;
	result = -1; 
	level = 0; 
	triggerTime = -1; 
	Label = ""; 
	runTime = -1; 
	usePrevV1 = 0;
	result = -1;
	testStep = -1;
	evalV1 = 0;
	evalV2 = 0;
	usePrevV1 = 0;
	sourceColumn = 0;
	sourceRow = 0;
	myInterpolate.Reset();
	testNotes.clear();
}


void
DualVarTestList::SetVarIndex2(int ind)
{
	if (ind < 0) {  // reset
		varIndex2 = -1;
		usevar2 = 0;
		return;
	}

	if (ind >= (int)theApp.myFPTM.VARlist.size()) return;

	varIndex2 = ind;
	usevar2 = 1;

	return;
}
