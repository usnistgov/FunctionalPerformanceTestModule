#pragma once

#include "stdafx.h"

//#include <format> // MAG 260326 add this to support std::format use
#include <string.h>
#include <regex>
#include <locale>	// std::locale?
#include <locale.h>     /* struct lconv, setlocale, localeconv */
#include <vector>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <MLang.h>
#include <cctype>
#include <direct.h>	// for _getcwd
#include <strsafe.h> // for error message code related to loading dll

#include "mxml.h"
#include "BACdev.h"
#include "FPvar.h"
#include "TimedRequest.h"
#include "NamedPoint.h"
#include "PointVarMap.h"
#include "TestList.h"
#include "RampValue.h"
#include "InterpolateValue.h"
#include "TestStep.h"
#include "PeriodicInput.h"


// copied from BACnet DLL for reference
#define IPHMAX 50	// size of IPH array
#define IPHLEN 20	// length of strings in IPH array
// stores network information for BACnet device
// MAG NOTE- this is also defined in the BACnet DLL, keep the versions aligned
typedef struct tagIPhelper {
	char** mac = NULL;
	char** router = NULL;
	char** bv = NULL;
	unsigned char** ip = NULL;
	unsigned char** routerip = NULL;
	int* ipport = NULL;
	int* routerport = NULL;
	int* net = NULL;
	int* obj = NULL;
	int* inst = NULL;
	int* prop = NULL;
	int* prty = NULL;
	int* index = NULL;
	int* devObjInst = NULL;
} IPhelper;


class FPTMbase
{
public:
	FPTMbase();		// init class 
	~FPTMbase();

	std::vector<std::vector<TimedRequest>>		TRlist;	// array of lists of timed requests- should mirror content of dialog->mybtm
	std::vector<std::vector<RampValue>>			rampList;	// array of list of ramp values, associated with each TR event list.  Uses TRactive;
	std::vector<std::vector<DualVarTestList>>	TESTlist;	// array of list of tests to determine success or failure
	std::vector<TimedRequestData>				TRdata;		// metadata for each TR list, also can use TRactive
	std::vector<std::vector<TestStep>>			testSteps;	// 
	int											TRactive;	// array index of the active TR list (Test Block) (add 'A' for display)
	int											TSactive;	// array index of the active Test Step (array index, NOT the test script column)
	std::vector<std::vector<DualVarTestList>>	eventTimerTestList;	// array of list of value threshold tests for time steps from Taylor/CalNEXT test scripts (OR/VarName/VarVal, Row 42-44 in draft V7)
															// Test list index should be referenced in first read event at each time step
	//NOTE! The above std::vector variables must all be maintained at the same size!!!

	std::vector<PointVarMap>	pvmInput;	// list of mappings from inputs section of Taylor/CalNEXT test script
	std::vector<PointVarMap>	pvmOutput;	// list of mappings from outputs section of Taylor/CalNEXT test script
	std::vector<NamedPoint>		namedPointList; // list of named points from FPTM or BDS input file. Label should match pointVarMap pointName

	// Tier 1 pass fail at each test step, values not needed, block pass/fail summary (no HVAC values)
	// Tier 2 what happened at each step, for ramp etc, any time values are sent.Values sent, values read if changed at this step.Only changed data!
	// Tier 3 CSV values, all the data 10 second interval
	std::ofstream				reportT1;	// MAG 250129 Store Tier 1 reporting here.
	std::ofstream				reportT2;	// MAG 250129 Store Tier 2 reporting here.
	std::ofstream				reportT3;	// MAG 250129 Store Tier 3 reporting here.
	std::vector<std::string>	reportT1log;// MAG 250616 store reportT1 lines here, pump out when test is complete.

	std::string					reportdts;	// date/time string used in log files. Used to save status window text.
	void						SaveLogT3(); //MAG 
	void						SaveExpectedValues();// MAG 260325 exports to CSV Expected Values (vs read values) for R and W events for each test step

	std::vector<BACdev>			BACnetList;			// list of BACnet DEVICES, from settings file or from scan
	int							BACnetReadCount;	// count of devices read from DLL, to differentiate from ones read from settings file
	int							TRlistRepeat;		// if == 0, no repeat, if > 0 repeat at (value) minutes
	std::vector<FPvar>			VARlist;			// list of variables used with timed requests
	IPhelper					iph;
	int*						fault;				// used in RunRequest and RunReset
	std::vector<std::string>	FilesRead;			// List of files read in
	std::string					scriptLabel;
	int							sparseEvents;		// MAG 241223 Add for Taylor script files, 0(default)=event for every input/output, 1=skip inputs where value is same as previous cell
	int							setScreenCapture;	// MAG 250129 Add for Taylor script files, 0(default)=no, 1=yes
	int							runtimeVar;			// MAG 250509 count of new VAR added at runtime, probably by Interpolate or a math function

	int							AddTask();			// adds an item to TRlist
	int							DeleteTask(int id);	// deletes an item from TRlist

	void						InitIPH();  // init iph structure

	// functions for setting the send-out-of-service request
	int							GetSendOutOfService(int objectid);
	void						SetSendOutOfService(int objectid);	// calling this flips the value of sendOutOfService
	void						SetListRepeat(int i);
	int							GetListRepeat();

	void						AddRamp(double sv, double ev, int rt, int dur, int evid);	// MAG 241231 add function

	int							SetStartState();  // MAG 250407 does all the things that need to be done to start sim
	void						EndTestStep();		// MAG 250620 adjusts time stamps of following test steps (called after test step evaluation criteria are met

	//DECLARE_MESSAGE_MAP()
	void OnRuntimeoptionsNewEventList();
	void OnRuntimeoptionsDeleteEventList();

	//enum ObjectTypes
	//{
	//	AI, AO, AV, BI, BO, BV, MSI, MSO, MSV
	//};

	int			InitBACnetAdapters();	// 221114 MAG add to explicitely call WPCInit
	int			InitLogFiles();			// MAG 250130 add to support Taylor/CalNEXT data/log files
	void		CloseLogFiles();		// MAG 250130 add to support Taylor/CalNEXT data/log files
	void		FlushLogFiles();		// MAG 250912 add to support Taylor/CalNEXT data/log files
	int			AddBACnetDevices();
	void		UpdateBACnetDevices();
	int			FindBACnetObjectType(std::string si);
	int			FindVariablePos(std::string si);
	NamedPoint	FindNamedPoint(std::string si);
	PointVarMap	FindPVM(std::string si);
	FPvar		FindVariable(std::string si);
	const int	GetNetworkAdapterCount() { return adapterCount; };
	const int	GetDefaultNetworkAdapter() { return defaultAdapter; };
	void		SetDefaultAdapter(int i) { defaultAdapter = i; };
	int			GetNetworkAdapterIP(int id, char* iptext);
	int			ReadBDSSettingsFile(xml_element* bdsDatapointList); // MAG 240528, as subtly indicated, this element must point to bdsDatapointList
	int			ChangeActiveTRlist(int updown);
	void		AddEventList() { OnRuntimeoptionsNewEventList(); }
	int			CheckTestStepTest();

	int			ScanPeriodicString(std::string rs, PeriodicInput* pi);
	int			ScanMathString(std::string rs, int mathType, TimedRequest* tr);
	int			ScanMathStringToVar(std::string rs, MathVar* tr);
	int			ScanInterpolateString(std::string cellData, InterpolateValue* myInterp, TestStep* mts);
	FILE*		evOut;					// output from events
	char		evOutLabel[MAX_PATH];	// store the label if needed to save config
	int			LaunchState;			// 0 == stopped 1 == started

	// Overrides
	int		RunRequest(int rid);
	int		RunReset(int rid);
	int		RunPeriodic();		// MAG 251120 add function to run periodic events incurrent test step
	int		SaveToFile(char* opname);
	int		ReadFromFile(char* ipname);
	int		ResetDelay(int interval);
	void	PrintVariables();	// MAG 210607 prints values of all variables to status
	void	RunTests();			// MAG 210607 runs tests and prints pass/fail to status
	void	RunTest(int tid);	// MAG 221110 runs test one and saves result
	void	RunTestsOnDemand();	// MAG 250314 calls RunTests on demand (primarily for testing)
	int		ReadCalNEXTScript();
	// The event save file name is passed from the GUI.  This sub opens the CFile to be used.
	int		SetEventSavefile(char* fileName);
	int		SetEventSavefile(std::string fileName);
	int		SetCheckOOS(int i);
	void	SetModelStartTime(CTime val);
	CTime	GetModelStartTime() { return modelStartTime; };
	double	GetInterpolatedValue(int testID, int testTime); // MAG 250501 add function.
	double	GetInterpolatedValue(int testID);				// MAG 251210 add function.

	// functions for the priority level when writing
	int		GetWritePriorityLevel() { return writePriorityLevel; };
	void	SetWritePriorityLevel(int lvl);

	int		adapterCount;	// the count of IP network adapters
	int		defaultAdapter;
	int		EvalRamp(RampValue* rv, std::string rs);
	int		EvalRamp(std::string rs, int evid);			// for when we don't have a rampValue already
	std::vector<std::string> bacnetObjectTypes;		// MAG 260310 added

	void		SetCurrentTestStep(int i);
	const int	GetCurrentTestStep() { return currentTestStep; };

	void		SetActiveColumn(int i);
	const int	GetActivecolumn() { return activeColumn; };

#ifdef USEAFDD
	// MAG Note- AFDD related code is deprecated, and remains only for reference
	// Added March 2016
	//EInput* p_DllInput = EPortHandles::GetPortIn();
	//EOutput* p_DllOutput = EPortHandles::GetPortOut();
	//Alerts_t *Alerts;// (0);
	//std::tm tBogus = { 0 };
	//std::string strmBuffer = " ";

	int SendDataAFDD();

	HINSTANCE hLinkDLL;
	f_IntTimeDouble AFDD;
	f_IntVoid InitAFDD;
	f_IntInt AFDDGetStates;
	f_IntVoid StartTimer;
	f_IntVoid OpenAFDDWindow;

	//f_AFDD AFDD;
	//f_InitAFDD InitAFDD;
	//f_AFDDGetStates AFDDGetStates;
	//f_InitAFDD StartTimer;
#endif

protected:
		mxml	filex;
		int		sendOutOfService[20];	// MAG NOTE- use native object type value, i.e. AI = 0, MSV = 19;
		int		writePriorityLevel;		// default to 7
		int		checkOOS;				// MAG 230503 add check if oos was set when resetting variables 1=check, 0=don't check
		CTime	modelStartTime;			// set when 'start' is pressed
		int		currentTestStep;		// set from RunRequest, NOT used in T3 log file NOTE this is array index not display value
		int		activeColumn;			// set from RunRequest, used in T3 log file
		int		sleepTimer;				// default 5, trim and respond in RunRequest based on failed reads
		int		localDeviceID;			// sent out with i-am on network init
};

