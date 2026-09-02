#include "stdafx.h"
#include "TimedRequest.h"
#include "FPTM App.h"

extern CFPTMApp theApp;

TimedRequest::TimedRequest()
{
	bacdevice = -1;
	eventTime = CTime::GetCurrentTime();
	CTimeSpan ts = eventTime.GetSecond();
	eventTime -= ts;  // cut off seconds
	object = -1;
	instance = -1;
	readwrite = -1;
	value = -999.0;
	sentValue = -999.0;
	fpvar = -1;
	fprelate = -1;
	fpmodvar = -1;
	initOutOfService = -1;  // not yet scanned
	initValue = -999;
	initPriorityLevel = -1;	// not scanned or not used
	stale = 0;
	scaleAO = 0;
	scaleFactor = 1;
	processVarIndex = -1;
	fpvalue = 0.0;
	testStep = 0;
	rampID = -1;
	sourceCol = 0;  // not set
	runTest = -1;
	valueLock = 0;
	interpID = -1;
	useValue = 1;
}


TimedRequest::TimedRequest(int dev, CTime evTime, int obj, int inst, int rw, double val)
{
	bacdevice = dev;
	eventTime = evTime;
	object = obj;
	instance = inst;
	readwrite = rw;
	value = val;
	sentValue = -999.0;
	fpvar = -1;
	fprelate = -1;
	fpmodvar = -1;
	initOutOfService = -1;  // not yet scanned
	initValue = -999;
	initPriorityLevel = -1;	// not scanned or not used
	stale = 0;
	scaleAO = 0;
	scaleFactor = 1;
	processVarIndex = -1;
	fpvalue = 0.0;
	testStep = 0;
	rampID = -1;
	sourceCol = 0;  // not set
	runTest = -1;	
	valueLock = 0;
	interpID = -1;
	useValue = 1;
}


TimedRequest::TimedRequest(int dev, CTime	evTime, int obj, int inst, int rw, double val, FPvar *fpv)
{
	bacdevice = dev;
	eventTime = evTime;
	object = obj;
	instance = inst;
	readwrite = rw;
	value = val;
	sentValue = -999.0;
	fpvar = -1;
	fprelate = -1;
	fpmodvar = -1;
	initOutOfService = -1;  // not yet scanned
	initValue = -999;
	initPriorityLevel = -1;	// not scanned or not used
	stale = 0;
	scaleAO = 0;
	scaleFactor = 1;
	processVarIndex = -1;
	fpvalue = 0.0;
	testStep = 0;
	rampID = -1;
	sourceCol = 0;  // not set
	runTest = -1;
	valueLock = 0;
	interpID = -1;
	useValue = 1;
}


TimedRequest::~TimedRequest()
{
	return;
}


TimedRequestData::TimedRequestData()
{
	label = "";
	testIdentifier = "";
}


TimedRequestData::TimedRequestData(CString newLabel)
{
	label = newLabel;
	testIdentifier = "";
}


void
TimedRequest::ResetLine()
{
	bacdevice = -1;
	eventTime = CTime::GetCurrentTime();
	CTimeSpan ts = eventTime.GetSecond();
	eventTime -= ts;  // cut off seconds
	object = -1;
	instance = -1;
	readwrite = -1;
	value = -999.0;
	sentValue = -999.0;
	fpvar = -1;
	fprelate = -1;
	fpmodvar = -1;
	initOutOfService = -1;  // not yet scanned
	initValue = 0;
	initPriorityLevel = -1;	// not scanned or not used
	stale = 0;
	scaleAO = 0;
	scaleFactor = 1;
	processVarIndex = -1;
	fpvalue = 0.0;
	testStep = 0;
	rampID = -1;
	valueLock = 0;
	interpID = -1;
	useValue = 1;
	runTest = -1;

	return;
}


CTime
TimedRequest::GetEventTime()
{
	return(eventTime);
}


int
TimedRequest::SetEventTime(CTime newtime)
{
	if (newtime == -1) return 0;
	eventTime = newtime;

	return 1;
}


int
TimedRequest::SetEventTime(struct tm* newtime)
{
	if (newtime == NULL) {	// set to now
		CTime eventTime2 = CTime::GetCurrentTime();
		CTimeSpan nextmin = CTimeSpan(0, 0, 0, 60 - eventTime2.GetSecond());
		eventTime = eventTime2 + nextmin;
		return 1;
	}
	int y = newtime->tm_year;
	if (y < 1900) y += 1900;
	CTime nt(y, newtime->tm_mon + 1, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec, newtime->tm_isdst);
	eventTime = nt;

	return 1;
}


void		
TimedRequest::AdjustEventTime(CTimeSpan cts, int decr)
{
	if (decr == 1) { 
		eventTime += cts;
	} else {
		CTime tnow = CTime::GetCurrentTime();
		if (eventTime - cts <= tnow) {
			eventTime = tnow;
		} else {
			eventTime -= cts;
		}
	}
}; // add time by default, sub if decr != 1


// Sets the value of bacdevice
int
TimedRequest::SetDevice(int id)
{
	if (id < 0) {
		return 0;
	}
	bacdevice = id;

	return 1;
}


// Returns the value of bacdevice
const int
TimedRequest::GetDevice()
{
	return bacdevice;
}


// sets the value of object
// NOTE nobj is the index of the drop-down, not the object identifier
int
TimedRequest::SetBACnetObject(int nobj)
{
	object = nobj;

	return 1;
}


// sets the value of object type by index
// NOTE nobj is the index of the drop-down, not the object identifier
int
TimedRequest::SetBACnetObjectIndex(int nobj)
{
	switch (nobj) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: object = nobj; break;
		case 6: object = 13; break;	// multi-state input
		case 7: object = 14; break;	// multi-state output
		case 8: object = 19; break;	// multi-state value
		default:
			return(0);  // only allowing AI to BV, and MI to MV
	}
	//object = nobj;
	//cs1.Format(L"Change object to %d", object);
	//theApp.m_BACdlg->m_status.AddString(cs1);

	return 1;
}


// returns the value of the object
const int
TimedRequest::GetBACnetObject()
{
	return object;
}


// sets the value of the object instance
int
TimedRequest::SetInstance(int ninst)
{
	if ((ninst < 0)) {// || (ninst >= 4194303)){	// MAG 160325 fix line, max was set to 0xFFFF
		return 0;
	}
	if (ninst >= 4194303) ninst &= 0x3FFFFF;// MAG 170420 sometimes it's been combined with object encoding, this strips off the type and leaves instance

	instance = ninst;
	//cs1.Format(L"Change BACnet Object instance to %d", instance);
	//theApp.m_BACdlg->m_status.AddString(cs1);
	return 1;
}


// returns the value of the oject instance
const int
TimedRequest::GetInstance()
{
	return (instance);
}


int
TimedRequest::SetScaleFactor(double nval)
{
	if (nval <= 0) return 0;

	scaleFactor = nval;
	scaleAO = 1;	  // just set value instead of call SetScale

	return 1;
}


const double
TimedRequest::GetScaleFactor()
{
	return scaleFactor;
}


// sets the value of the read/write property
// NOTE- 0=read, 1=write, 2=reset
int
TimedRequest::SetRW(int nrw)
{
	if ((nrw < 0) || (nrw > 2)) return(0);
	readwrite = nrw;
	//cs1.Format(L"Change RW to %s", (readwrite)?L"write":L"read");
	//theApp.m_BACdlg->m_status.AddString(cs1);

	return (1);
}


// returns the value of the read/write property
const int
TimedRequest::GetRW()
{
	return (readwrite);
}


// sets the value of the value variable (really?)
int
TimedRequest::SetValue(double nval)
{
	// NOTE no range check on input needed
	if (valueLock == 1) {
		if (nval != value) {
			CString csx = L"DEBUG attempt to overwrite const value.";
		}
		return 0;
	}
	value = nval;
	stale = 0;
	if ((object == AO) && (value > 1.1))
	{
		scaleAO = 1;	  // just set value instead of call SetScale
	}

	useValue = 1;  // MAG 260114 add this line

	// xxx MAG 250324 NOTE disable overwriting variable- it should be updated before evaluation
	//// MAG 180702 if a read event, and it has a variable, update variable value
	//if ((readwrite == 0)&&(fpvar > 0)){
	//	theApp.myFPTM.VARlist[fpvar].SetValue(nval);
	//}
	//cs1.Format(L"Change Value to %f", value);
	//theApp.m_BACdlg->m_status.AddString(cs1);

	return 1;
}


// sets the value of the sent value variable (really?)
int
TimedRequest::SetSentValue(double nval)
{
	sentValue = nval;

	return 1;
}


// if you have to ask...
// NOTE- if there is a variable or operator used, it will be evaluated in ProcessVar
const double
TimedRequest::GetValue()
{
	if (useValue == 1) return (value);

	if (fpvar < 0) return (value);  // variable not used, most common case

	if (fprelate < 0) return (value);  // MAG 260109 test this, not returning init values for TESTstep

	return ProcessVar();
}


// if you have to ask...
// NOTE- if there is a variable or operator used, it will be evaluated in ProcessVar
const double
TimedRequest::GetSentValue()
{
	return sentValue;  // nothing tricky about this one...
}


void
TimedRequest::SetUseValue(int nval)
{
	if (valueLock == 1) {
		return;
	}
	useValue = (nval == 1)?1:0;
}


const int
TimedRequest::GetUseValue()
{
	return useValue;
}


int
TimedRequest::GetInitOOS()
{
	return initOutOfService;
}


void
TimedRequest::SetInitOOS(int i) {
	if (i < -1) return;
	if (i > 1) return;
	initOutOfService = i;

	return;
}


// Calculates value to write to controller, saves to processVar is set
double
TimedRequest::ProcessVar()
{
	double curval = 0;
	double retval = 0;
	CString cs1;

	if (fpvar < 0) return 0;
	if (fpvar >= (int)theApp.myFPTM.VARlist.size()) return 0;

	// MAG 251209 comment this out, vars are read for periodic and maybe others.
	//if (readwrite == 0) return theApp.myFPTM.VARlist[fpvar].GetValue();// read values not affected by fpvalue, operator
	//if (readwrite == 2) return theApp.myFPTM.VARlist[fpvar].GetValue();// reset values should not have variables etc.

	if (fpmodvar == -1) {
		curval = fpvalue; // use value, not variable
	} else {
		curval = theApp.myFPTM.VARlist[fpmodvar].GetValue();
	}

	switch (fprelate) {
		case 1: retval = theApp.myFPTM.VARlist[fpvar].GetValue() + curval; break;
		case 2: retval = theApp.myFPTM.VARlist[fpvar].GetValue() - curval;	break;
		case 3: retval = theApp.myFPTM.VARlist[fpvar].GetValue() * curval;	break;
		case 4:	if (curval == 0) return 0;
			retval = theApp.myFPTM.VARlist[fpvar].GetValue() / curval; break;
		case 5: retval = curval - theApp.myFPTM.VARlist[fpvar].GetValue(); break;  // MAG order switched from case 2
		case 6:	if (theApp.myFPTM.VARlist[fpvar].GetValue() == 0) return 0;
			retval = curval / theApp.myFPTM.VARlist[fpvar].GetValue(); break;
		case 7:
			if (readwrite == 0) {	// reading
				theApp.myFPTM.VARlist[fpvar].SetValue(value);
				return 1;
			}
			break;
		case 0: // just the var, no relate
		default: retval = theApp.myFPTM.VARlist[fpvar].GetValue();	// MAG 180709 NOTE- can get rid of this and let fall through if it remains the same
			break;
	}

	value = retval; // MAG 230516 add this so that last written value can be viewed

	//if (readwrite == 0) {
	//	theApp.myFPTM.VARlist[processVarIndex].SetValue(retval);// MAG 230516 add this so that last written value can be used in tests NOTE- can write to "dummy" address and still save value, need "calc only" flag in input file?
	//	cs1.Format(L"Change value of var %S (%d) to %5.2f", theApp.myFPTM.VARlist[processVarIndex].GetLabel().c_str(), processVarIndex, retval);
	//	theApp.m_BACdlg->m_status.AddString(cs1);
	//}

	// write vars only
	if ((readwrite == 1) && (processVarIndex > 0)) {
		theApp.myFPTM.VARlist[processVarIndex].SetValue(retval);// MAG 230516 add this so that last written value can be used in tests NOTE- can write to "dummy" address and still save value, need "calc only" flag in input file?
		cs1.Format(L"Change value of var %S (%d) to %5.2f", theApp.myFPTM.VARlist[processVarIndex].GetLabel().c_str(), processVarIndex, retval);
		theApp.m_BACdlg->m_status.AddString(cs1);
	}

	return value;
} // end ProcessVar


void
TimedRequest::SetInitValues(int i, double val)
{
	initPriorityLevel = i;
	initValue = val;

	return;
}


void
TimedRequest::GetInitValues(int* i, double* val)
{
	if (val == NULL) return;
	if (i == NULL) return;

	try {
		memcpy(i, &initPriorityLevel, sizeof(int));
		memcpy(val, &initValue, sizeof(double));
	}
	catch (const std::exception& e) { // MAG 260409 update catch statements
		// Log the error message
		//std::stringstream ss1;
		//lastErrorList.push_back("ERROR: Caught exception in TimedRequest::GetInitValues!");
		//ss1 << "ERROR: " << e.what();
		//lastErrorList.push_back(ss1.str());
		CString err;
		err.Format(L"ERROR in TimedRequest::GetInitValue: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);

		return;
	}
	catch (...) {
		CString err;
		err.Format(L"ERROR in TimedRequest::GetInitValue: Unknown");
		theApp.m_BACdlg->m_status.AddString(err);
		return;
	}

	return;
}


void
TimedRequest::IncStale()
{
	stale++;

	return;
}


const int
TimedRequest::GetStale()
{
	return stale;
}


// MAG Note- stale set to 0 in SetValue
void
TimedRequest::ResetStale()
{
	stale = 0;
}


const int
TimedRequest::GetScaleAO()
{
	return scaleAO;
}


void
TimedRequest::SetScaleAO(int i)
{
	if (i == 0) scaleAO = 0;
	else scaleAO = 1;

	return;
}


#define READREQCOUNT 5

// Set VARlist index to fpvar
void
TimedRequest::SetFPV(int i)
{
	if (valueLock == 1) {
		return;
	}
	if (i < -1) return;
	if (i >= (int)theApp.myFPTM.VARlist.size()) return;
	fpvar = i;

	useValue = 0;  // MAG 260114 add line

	return;
}


void
TimedRequest::SetFPVModvar(int i)
{
	if (i < -1) return;
	if (i >= (int)theApp.myFPTM.VARlist.size()) return;
	fpmodvar = i;

	return;
}


void
TimedRequest::SetFPVProcvar(int i)
{
	if (i < -1) return;
	if (i >= (int)theApp.myFPTM.VARlist.size()) return;
	processVarIndex = i;

	return;
}


int
TimedRequest::CheckValid()
{
	if (bacdevice < 0) return 0;
	if (object < 0) return 0;
	if (instance < 0) return 0;
	if (readwrite < 0) return 0;

	return 1;
}


/* Not used
void 
TimedRequest::SetFPV(int i) 
{
	if (i < -1) return; 
	if (i >= (int) theApp.myFPTM.VARlist.size()) return; 
	fpvar = i; 

	return;
}


void
TimedRequest::SetFPVModvar(int i) 
{
	if (i < -1) return;
	if (i >= (int)theApp.myFPTM.VARlist.size()) return;
	fpmodvar = i;

	return;
}


void
TimedRequest::SetFPVProcvar(int i)
{
	if (i < -1) return;
	if (i >= (int)theApp.myFPTM.VARlist.size()) return;
	processVarIndex = i;

	return;
}
*/

// MAG 250306
// Returns 0 if current TR is not in a valid state to start testing
/*	
Init values for reference:
	bacdevice = -1;
	eventTime = CTime::GetCurrentTime();
	CTimeSpan ts = eventTime.GetSecond();
	eventTime -= ts;  // cut off seconds
	object = -1;
	instance = -1;
	readwrite = -1;
	value = -999.0;
	fpvar = -1;
	fprelate = 0;
	fpmodvar = -1;
	initOutOfService = -1;  // not yet scanned
	initValue = 0;
	initPriorityLevel = -1;	// not scanned or not used
	stale = 0;
	scaleAO = 0;
	scaleFactor = 1;
	processVarIndex = -1;
	fpvalue = 0.0;
	testStep = 0;
*/

/*
int TimedRequest::CheckValid()
{
	if (bacdevice < 0) return 0;
	if (object < 0) return 0;
	if (instance < 0) return 0;
	if (readwrite < 0) return 0;

	return 1;
}

*/



