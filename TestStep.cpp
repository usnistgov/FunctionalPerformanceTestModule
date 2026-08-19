#include "TestStep.h"

std::vector<int>
TestStep::GetList()
{
	return trList;
}


int
TestStep::CheckList(int val)
{
	if (val < 0) return 0;
	auto it = find(trList.begin(), trList.end(), val);
	if (it != trList.end()) return 1; // val is present in list

	return 0; // val was not found in list
}


// comparator function, sorts increasing integers
bool 
comp(int a, int b)
{
	return a < b;
}

void
TestStep::AddToList(int val)
{
	if (val < 0) return;
	if (val > 10000) return;// exceeds reasonable test list size
	trList.push_back(val);
	std::sort(trList.begin(),trList.end(),comp);
}


void 
TestStep::SetStatus(int val) 
{
	if (val <= 0) {
		status = 0; 
	} else {
		status = 1;
	}

	return;
}


void
TestStep::SetTime(CTime ct) 
{
	if (ct == 0) 
		tsTime = CTime::GetCurrentTime();
	else 
		tsTime = ct; 
}


// Note does not allow decrementing to before current time
void
TestStep::AdjustTime(CTimeSpan cts, int decr) 
{ 
	CTime tnow;

	if (decr == 1) {
		tsTime += cts;
	} else {
		tnow = CTime::GetCurrentTime();
		if (tsTime - cts <= tnow) { 
			tsTime = tnow; 
		} else {
			tsTime -= cts;
		}
	}
}


void
TestStep::Reset()
{
	trList.clear(); // array of indexes of TimedRequest elements (TRlist) in current test step
	mvList.clear();
	piList.clear();
	tsTime = 0; // test step time
	tsInterval = 0; // test step interval
	tsTest = -1;	// -1 if none, else index into eventTimerTestList
	label.clear();
	status = 0;
}