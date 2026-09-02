
// FPTM App.h : main header file for the FPTM application
//
// MAG July 2016 NOTE- Officially change name of project to Functional Performance Test Module (FPTM)
// MAG 190808 Add capability to use variable for second value when modifying a write value. Add TimedReqest::fpmodvar and associated
//     set/get functions. Modified script read/write subroutines. Modified TimedRequest::ProcessVar().
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#ifdef USEAFDD
#include <future>
#include "dllExports.h"
typedef std::vector< std::string > Alerts_t;
#endif
#include <string.h>
#include <regex>
#include <locale>	// std::locale?
#include <locale.h>     /* struct lconv, setlocale, localeconv */
#include <vector>
#include "FPTM Dlg.h"
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <cctype>

#include "mxml.h"
#include "FPTMbase.h"
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

#ifdef USEAFDD
typedef int(*f_IntTimeDouble)(time_t dataTime, double *input);
typedef int( *f_IntVoid)();
typedef int( *f_IntInt)(int input);
#endif

#ifndef BACOFFLINE
namespace BACdll {
	// MAG 260227 add Device functions
	//extern "C" __declspec(dllimport) int ReadPVPropertyByDeviceSLI(char* devid, char* obj, char* inst, char* bv, char* fault);
	//extern "C" __declspec(dllimport) int ReadObjPropertyByDeviceSLI(char* devid, char* cobj, char* cinst, char* cprop, char* cbv, char* cfault);
	//extern "C" __declspec(dllimport) int WritePVPropertyByDeviceSLI(char* devid, char* cobj, char* cinst, char* cbv, char* cfault);
	//extern "C" __declspec(dllimport) int WriteObjPropertyByDeviceSLI(char* devid, char* cobj, char* cinst, char* cprop, char* cbv, char* cfault);
	extern "C" __declspec(dllimport) int ReadObjPropertyByDevice(int count, int* devid, int* obj, int* inst, int* prop, int* index, char** bv, int* fault);
	extern "C" __declspec(dllimport) int ReadPVPropertyByDevice(int count, int* devid, int* obj, int* inst, char** bv, int* fault);
	extern "C" __declspec(dllimport) int WritePVPropertyByDevice(int count, int* devid, int* obj, int* inst, char** bv, int* fault);
	extern "C" __declspec(dllimport) int WriteObjPropertyByDevice(int count, int* devid, int* obj, int* inst, int* prop, int* index, int* prty, char** bv, int* fault);

	extern "C" __declspec(dllimport) int GetDeviceInfoSLI(int id, char* mac, char* ip, char* ipport, char* net, char* router, char* routerip, char* routerport);
	extern "C" __declspec(dllexport) int AddDevListInfo(int devobjid, char* mac, unsigned char* ip, int ipport, unsigned int net, char* router, unsigned char* routerip, int routerport);

	extern "C" __declspec(dllimport) int GetDeviceListID(int devid);
	//extern "C" __declspec(dllimport) int GetDeviceIndexFromIdentifier(int objectID);

	extern "C" __declspec(dllimport) int __stdcall WPCInit(void);
	extern "C" __declspec(dllimport) void WPCClose(void);
	extern "C" __declspec(dllimport) void sendWhoIsUDP(void);	// for UDPIP
	extern "C" __declspec(dllimport) int sendWhoIsB(void);		// for ethernet
	extern "C" __declspec(dllexport) int SendIAM(int devid, int subnet, int segsup, int vendor);
	extern "C" __declspec(dllexport) int GetAdapterIP(int i, unsigned char* label);

	extern "C" __declspec(dllimport) int GetDeviceListCount();
	extern "C" __declspec(dllimport) int PrintDeviceListInfo(int id, char* mac, char* ip, char* ipport, char* net, char* router, char* routerip, char* routerport);
	extern "C" __declspec(dllimport) int PrintDeviceListInfo2(int id, char* devid, char* mac, char* ip, char* ipport, char* net, char* router, char* routerip, char* routerport);
	//extern "C" __declspec(dllimport) int readPVProperty(int count, char** mac, int* net, char** router, int* obj, int* inst, char** bv, int* fault);
	//extern "C" __declspec(dllimport) int readPVPropertyIP(int count, char** mac, unsigned char** ip, int* ipport, int* net, char** router, unsigned char** routerip, int* routerport, int* obj, int* inst, char** bv, int* fault);
	//extern "C" __declspec(dllimport) int readObjProperty(int count, char** mac, int* net, char** router, int* obj, int* inst, int* prop, int* index, char** bv, int* fault);
	//extern "C" __declspec(dllimport) int readObjPropertyUDPIP(int count, char** mac, unsigned char** ip, int* ipport, int* net, char** router, unsigned char** routerip, int* routerport, int* obj, int* inst, int* prop, int* index, char** bv, int* fault);
	//extern "C" __declspec(dllimport) int writePVProperty(int count, char** mac, int* net, char** router, int* obj, int* inst, int* prty, char** bv, int* fault);
	//extern "C" __declspec(dllimport) int WriteObjProperty(int count, char** mac, int* net, char** router, int* obj, int* inst, int* prop, int* index, int* prty, char** bv, int* fault);
	//extern "C" __declspec(dllimport) int readObjPropertyIPch(char* mac, char* ip, char* ipport, char* net, char* router, char* routerip, char* routerport, char* obj, char* inst, char* prop, char* bv, char* fault);
	//extern "C" __declspec(dllimport) int writePVPropertyIP(int count, char** mac, unsigned char** ip, int* ipport, int* net, char** router, unsigned char** routerip, int* routerport, int* obj, int* inst, int* prty, char** bv, int* fault);
	//extern "C" __declspec(dllimport) int writeObjPropertyIPch(int count, char** mac, unsigned char** ip, int* port, int* net, char** router, unsigned char** routerip, int* routerport, int* obj, int* inst, int* prop, int* index, int* prty, char** bv, int* fault);
	extern "C" __declspec(dllimport) int GetAdapterIPSLI(int i, char* label);
	extern "C" __declspec(dllimport) int GetDefaultAdapter(void);
	extern "C" __declspec(dllexport) int SetDefaultAdapter(int i);
}
#endif

//#define DEVREADCOUNT 40


// These are here for reference?
// MAG 180904 note- used in other projects via exports, do not change unless other projects are modified too!
// used in: -BACdll and all updated projects using it
typedef struct tagdevAddr { // Holds all network address info for a source, dest, or router
	unsigned char	mac_device[6] = {};		// mac of device (raw, not printable chars)
	int				mac_device_len = 0;		// length of mac_device, in octets
	int				mac_device_dec = 0;		// mac of device in decimal (if possible, or 0)
	char			mac_device_pr[15] = {};	// mac of device (in printable chars, might include 0x and \n)
	unsigned char	ip[4] = {};				// ip address of this device
	int				ip_port = 47808;		// should be 0xBAC0 (47808) by default
	char			ip_or_eth = 0;			// ip = 1, eth = 0, both = 3
	int				net = 0;				// network device is on
} DevAddr;

#define WRITE_LEVEL 8 // initial priority level for writing BACnet values

//// copied from BACnet DLL for reference
//#define IPHMAX 50	// size of IPH array
//typedef struct tagIPhelper {
//	char			**mac = NULL;
//	char			**router = NULL;
//	char			**bv = NULL;
//	unsigned char	**ip = NULL;
//	unsigned char	**routerip = NULL;
//	int				*ipport = NULL;
//	int				*routerport = NULL;
//	int				*net = NULL;
//	int				*obj = NULL;
//	int				*inst = NULL;
//	int				*prop = NULL;
//	int				*prty = NULL;
//	int				*index = NULL;
//} IPhelper;
//

// CBACnetTestModuleApp:
// See BACnet Test Module.cpp for the implementation of this class
// for reference:
//#define AI 0
//#define AO 1
//#define AV 2
//#define BI 3
//#define BO 4
//#define BV 5
//#define MSI 13
//#define MSO 14
//#define MSV 19

class CFPTMApp : public CWinApp
{
public:
	CFPTMApp(void);
	~CFPTMApp(void);

	FPTMbase myFPTM;


// Overrides
public:
	virtual BOOL InitInstance();
	CFPTMDlg *m_BACdlg;
	CString GetDTS(CTime ct = NULL);
	int GetDTSC(char *dtss, CTime ct = NULL, int useComma = 0);

// Implementation

//DECLARE_MESSAGE_MAP()
	
public:
	// 250410 MAG NOTE Leave these functions here!
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRuntimeoptionsNewEventList();
	afx_msg void OnRuntimeoptionsDeleteEventList();
};

extern CFPTMApp theApp;