#pragma once

#include "FPvar.h"
#include <atltime.h>  // for CTime and CTimeSpan


#define AI 0
#define AO 1
#define AV 2
#define BI 3
#define BO 4
#define BV 5
#define MSI 13
#define MSO 14
#define MSV 19

// MAG 241028 add class to hold metadata for timedRequest list (test block). Supports CalNEXT effort.
// A "timed request" is one read or write event, corresponding to a single cell in a test script worksheet
class TimedRequestData {
public:
	TimedRequestData();
	TimedRequestData(CString label);

	CString GetLabel()			{ return label; };
	CString GetTestIdentifier() { return testIdentifier; };

	void SetLabel(const CString cs1) { label = cs1; };
	void SetLabel(const std::string cs1) { label = cs1.c_str(); };
	void SetTestIdentifier(const CString cs1) { testIdentifier = cs1; };
	void SetTestIdentifier(const std::string cs1) { testIdentifier = cs1.c_str(); };

	void Clear() { label = ""; testIdentifier = ""; };

protected:
	CString		label;	// label describing current test, from row 1 (test block description)
	CString		testIdentifier; // test block, i.e. A, B, C, etc.
};

// aka event, event list
// each instance of this class represents one read/write request as displayed on the GUI
class TimedRequest {
public:
	TimedRequest();
	TimedRequest(int dev, CTime	evTime, int obj, int inst, int rw, double val);
	TimedRequest(int dev, CTime	evTime, int obj, int inst, int rw, double val, FPvar *fp);
	~TimedRequest();
protected:
	int		bacdevice;  // position in BACnetList
	CTime	eventTime;	// time/date to set it off
	int		object;		// BACnet object
	int		instance;	// instance of BACnet object
	int		readwrite;	// read=0, write=1, reset=2 MAG 200512 add reset
	double	value;		// value to be written, or last value read
	double	sentValue;	// for write requests, actual value last sent (may differ from 'value' or may be from a math function)
	int		fpvar;		// index to VARlist, -1 for not used
	int		useValue;	// use 'value' for this event, also save value written to controller to VAR[fpvar] (used as bool)
	CString	comment;	// text comment, may be changed by user
	int		fprelate;	// variable relation to value: 0:== 1:add 2:subtract 3:multiply 4:divide by 5:(inverse subtract) 6:(inverse divide). If < 0 var is not used, value is used.
	double	fpvalue;	// <modvalue> used with operator NOTE NOT VALUE READ/WRITE from/to controller- this modifies write values with fprelate
	int		fpmodvar;	// <modvar> used instead of fpvalue as second term in operation- used only with fprelate
	int		initOutOfService;	// MAG 170327 add to check if point is already out of service when testing starts.  scan when run button is pressed
	int		initPriorityLevel;	// MAG 170410 initial priority level (above default), or -1.  Should match write priority level.  Really acts like a bool to check if initValue is used.
	double	initValue;	// MAG 170410 initial value at priority level (above default)
	int		stale;		// MAG 170421 number of cycles value not read, timeout on request
	int		scaleAO;	// MAG 170502 if AO is read at range 0-100, need to scale to 0-1.  Scale if set to 1
	double	scaleFactor;// MAG 170519 scaling factor, set to 0.01 to scale 0-100 to 0-1.
	int		processVarIndex; // MAG 230516 index into VARlist for output of processed variable (ProcessVar), var holds calculated value sent to device
	int		rampID;		// MAG 250422 add rampID: -1 for no ramp, >= 0 is index into rampList
	int		interpID;	// MAG 250519 add interpID, index to interpolate
	int		testStep;	// MAG 250131 add to support Taylor/CalNEXT. Intended to act like a category, all events from a column have the same testStep identifier.
						// testStep is coordinated with testStep in test step conditions (var name/var value tests to move to next step vs clock time).
						// Zero for not set. Else should have value from Test Step cell, row 7 in V8. Note test step values start at 1.
	int		sourceCol;	// Indicates column count into test step, starting at 1. For use with T3 log.
	int		runTest;	// MAG 250520 run this test at end of cycle where event is run. -1 for none.
	int		valueLock;	// MAG 250522 For events writing a value (no vars involved). Locks value, useValue, and fpvar

public:
	void		ResetLine();	// MAG 170502 use to reset line to init values, i.e. when changing parameters
	CTime		GetEventTime();
	int			SetEventTime(CTime newtime);
	int			SetEventTime(struct tm *newtime);
	void		AdjustEventTime(CTimeSpan cts, int decr = 1);// { if (decr == 1) eventTime += cts; else eventTime -= cts; }; // add time by default, sub if decr != 1
	void		SetValueLock() { valueLock = 1; };  // MAG no way to undo this on purpose

	int			SetDevice(int id);			// Sets the value of bacdevice

	const int	GetDevice();				// Returns the value of bacdevice

	int			SetBACnetObject(int nobj);	// sets the value of object
	int			SetBACnetObjectIndex(int index);// sets the value of object by drop-down index, NOT setting instance

	const int	GetBACnetObject();			// returns the value of the object

	int			SetInstance(int ninst);		// sets the value of the object instance
	const int	GetInstance();				// returns the value of the oject instance

	int			SetRW(int nrw);				// sets the value of the read/write property
	const int	GetRW();					// returns the value of the read/write property

	void		SetRunTest(int nrw) { runTest = nrw; };	// sets the value of runTest
	const int	GetRunTest() { return runTest; };		// returns the value of runTest

	int			SetValue(double nval);		// sets the value of the value variable (really?)
	const double GetValue();				// if you have to ask...

	int			SetSentValue(double nval);		// sets the value of the sent value (really?)
	const double GetSentValue();				// if you have to ask...

	void		SetUseValue(int nval);		// sets the value of the useValue variable
	const int	GetUseValue();				// returns the value of useValue

	void		SetSourceColumn(int nval) { sourceCol = nval; };
	const int	GetSourceColumn() { return sourceCol; };

	void		SetTestStep(int val) { testStep = val; }
	int			GetTestStep() { return testStep; }
	int			SetComment(CString ncomment) { comment = ncomment; return 1; }
	const CString GetComment() { return comment; }

	double		ProcessVar();
	int			GetInitOOS();
	void		SetInitOOS(int i);
	void		SetInitValues(int i, double val);
	void		GetInitValues(int *i, double *val);
	void		IncStale();
	const int	GetStale();
	void		ResetStale();
	const int	GetScaleAO();
	void		SetScaleAO(int i = 0);// for when a line is changed, i.e. to a new AO point or on a diff controller.
	int			SetScaleFactor(double nv);		// MAG 170519 add scale factor
	const double GetScaleFactor();		// MAG 170519 add scale factor

	void		SetFPV(int i);				/// FPV sets the variable ID (array position) 
	void		SetFPVOperator(int i) { fprelate = i; };
	void		SetFPVValue(double i) { fpvalue = i; };
	void		SetFPVModvar(int i);
	void		SetFPVProcvar(int i);
	void		SetRampID(int i) { rampID = i; };
	void		SetInterpID(int i) { interpID = i; };

	const int		GetFPV() { return fpvar; }
	const int		GetFPVRelate() { return fprelate; }
	const double	GetFPVValue() { return fpvalue; }
	const int		GetFPVModvar() { return fpmodvar; }	// returns index into var array
	const int		GetPVI() { return processVarIndex; };	// returns process var index
	const int		GetRampID() { return rampID; };			// returns -1 if rampID is not set (default value)
	const int		GetInterpID() { return interpID; };			// returns -1 if rampID is not set (default value)

	int			CheckValid();
};
