#include "BACdev.h"
#include "stdafx.h"
#include "FPTM App.h"

int BACdev::maxIndex = 0;

// Constructor
BACdev::BACdev()
{
	memset(mac, 0, 20);
	memset(routermac, 0, 20);
	memset(routerip, 0, 20);
	memset(ip, 0, 20);
	ipport = 0xBAC0;
	net = 10;
	routeripport = 0xBAC0;
	label[0] = 0;
	//maxIndex = 0;
	index = 0;
	maxIndex++;
	configID = -1;
	devObjInst = 0;
}


// Constructor
BACdev::BACdev(int ind1)
{
	index = ind1;
	if (ind1 > maxIndex) maxIndex = ind1;
	memset(mac, 0, 20);
	memset(routermac, 0, 20);
	memset(routerip, 0, 20);
	memset(ip, 0, 20);
	ipport = 0xBAC0;
	net = 10;
	routeripport = 0xBAC0;
	label[0] = 0;
	//maxIndex = 0;
	index = 0;
	maxIndex++;
	configID = -1;
	devObjInst = 0;
}


BACdev::~BACdev()
{
}


// Resets all member variables to default values
void
BACdev::Reset()
{
	mac[0] = 0;		// printable, human readable version of MAC
	ip[0] = 0;		// ex. "129.6.162.135"
	ipport = 0xBAC0;
	net = 0;
	routermac[0] = 0;
	routerip[0] = 0;
	routeripport = 0xBAC0;
	configID = 0;	//0:IP/rIP, 1: MAC/rIP, 2: MAC/rMAC, 3: IP only, 4: MAC only
	index = maxIndex;	// MAG identifier, used when reading or writing file to associate with event.  
	label[0] = 0;	// text label for display
	configID = 0;
}


// Saves human readable version of MAC
int 
BACdev::SetMAC(char *nmac)
{
	char *cp = nmac;

	if (nmac == NULL) return 0;
	if (strlen(nmac) == 0) return(0);

	if (strncmp(nmac, "0x", 2) == 0) cp += 2;
	strncpy_s(mac, cp, 12);

	return(1);
}


// Saves human readable version of MAC
int 
BACdev::SetMAC(unsigned char *nmac)
{
	if (nmac == NULL) return 0;

	sprintf_s(mac, "%X%X%X%X%X%X", nmac[0], nmac[1], nmac[2], nmac[3], nmac[4], nmac[5]);

	return(1);
}


// Returns the human readable MAC address
const int 
BACdev::GetMAC(char *nmac)
{
	if (nmac == NULL) return 0;

	try {
		strncpy_s(nmac, 14, mac, 12);
		nmac[12] = 0;
	}
	catch (const std::exception& e) {
		CString err;
		err.Format(L"ERROR in GetMAC: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}
	catch (...) {
		CString err;
		err.Format(L"ERROR in GetMAC: Unknown");
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}

	return(1);
}


// Sets the IP address using integer array
int 
BACdev::SetIP(unsigned char nip[4])
{
	memcpy(ip, nip, 4);

	return(1);
}


// Sets the IP address using character string
int 
BACdev::SetIP(char *nip)
{
	char *cp = NULL;

	if (nip == NULL) return 0;

	if (strcmp(nip, "0.0.0.0") == 0){
		memset(ip, 0, 4 * sizeof(char));
		return 0;
	}

	cp = nip;
	ip[0] = atoi(cp);
	if (ip[0] == 0) return(1);  // if sent "" it will be caught here

	for (int i = 1; i < 4; i++){
		cp = strchr(cp, '.');
		if (cp == NULL){
			ip[0] = 0;
			return(0);
		}
		cp++;
		ip[i] = atoi(cp);
	}

	return(1);
}


// Returns the IP address as unsigned character array
int 
BACdev::GetIP(unsigned char nip[4])
{
	if (nip == NULL) return 0;
	
	try {
		memcpy(nip, ip, 4);
	}
	catch (const std::exception& e) {
		CString err;
		err.Format(L"ERROR in GetIP: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}
	catch (...) {
		CString err;
		err.Format(L"ERROR in GetMAC: Unknown");
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}
	return(1);
}


// Returns the IP address as integer array
int
BACdev::GetIP(unsigned int nip[4])
{
	if (nip == NULL) return 0;
	
	try {
		for (size_t i = 0; i < 4; i++)
		{
			nip[i] = ip[i];
		}
	}
	catch (const std::exception& e) {
		CString err;
		err.Format(L"ERROR in GetIP: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}
	catch (...) {
		CString err;
		err.Format(L"ERROR in GetMAC: Unknown");
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}
	return(1);
}


// Returns the IP port as integer
int 
BACdev::GetIPPort()
{
	return(ipport); // usually 0xBAC0 (47808) or 0xBAC1 (47809)
}


//Sets the IP port from a character string i.e. "0x47808" 
int 
BACdev::SetIPPort(char *nipp)
{
	int i = 0;

	if (nipp == NULL) return(0);
	
	if (strlen(nipp) < 3) return 0;  // to catch strings like "0x". All valid values will be longer than this

	if (strncmp(nipp, "0x", 2) == 0){
		i = strtol(nipp, NULL, 16);
	} else {
		i = atoi(nipp);
	}

	// check for valid range in called function
	return(SetIPPort(i));
}


// MAG NOTE- set nipp to 0 to set to default value 0xBAC0
int 
BACdev::SetIPPort(int nipp)
{
	// check for valid range see Annex J.1.2
	if (nipp < 0xBAC0){
		ipport = 0xBAC0;
		return(0);
	}
	if ((nipp > 47823) && (nipp < 49152)) {
		ipport = 0xBAC0;
		return 0;
	}
	if (nipp > 0xFFFF) {
		ipport = 0xBAC0;
		return(0);
	}

	ipport = nipp;
	if (ipport == 0) ipport = 0xBAC0;

	return(1);
}


// Returns the network the controller is on
const int 
BACdev::GetNet()
{
	return(net);
}


// Sets the network the controller is on from character string
int
BACdev::SetNet(char *nipp)
{
	int i = 0;

	if (nipp == NULL) return(0);

	try {
		i = atoi(nipp);
	}
	catch (const std::exception& e) {
		CString err;
		err.Format(L"ERROR in SetNet: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}
	catch (...) {
		CString err;
		err.Format(L"ERROR in SetNet: Unknown");
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}

	return(SetNet(i));
}


// Returns the network the controller is on from an integer
int
BACdev::SetNet(int nnet)
{
	if (nnet < 0) return(0);		// range os 1-0xFFFF, but allowing 0 for cases when net is not known
	if (nnet >= 0xFFFF) return(0);  // 0xFFFF is broadcast

	net = nnet;

	return(1);
}


// Sets the Router MAC from a human readable string
int 
BACdev::SetRouterMAC(char *nmac)
{
	if (nmac == NULL) return 0;
	if (strlen(nmac) == 0) return(0);

	strncpy_s(routermac, nmac, 12);
	routermac[12] = 0;
	
	return(1);
}


// Sets the Router MAC from a integer string to a human readable version MAC
int 
BACdev::SetRouterMAC(unsigned char *nmac)
{
	if (nmac == NULL) return 0;

	sprintf_s(routermac, "%X%X%X%X%X%X", nmac[0], nmac[1], nmac[2], nmac[3], nmac[4], nmac[5]);
	
	return(1);
}


// Returns human readable version of Router MAC
int 
BACdev::GetRouterMAC(char *nmac)
{
	if (nmac == NULL) return 0;

	strncpy_s(nmac, 14, routermac, 12);
	nmac[12] = 0;

	return(1);
}


// Sets router IP from unsigned char array i.e. [127][0][0][1]
int 
BACdev::SetRouterIP(unsigned char nip[4])
{
	memcpy(routerip, nip, 4);

	return(1);
}


// Sets router IP from char array i.e. "127.0.0.1"
int
BACdev::SetRouterIP(char *nip)
{
	char *cp = NULL;

	if (nip == NULL) return 0;

	cp = nip;
	if (strcmp(nip, "0.0.0.0") == 0){
		memset(routerip, 0, 4 * sizeof(char));
		return 0;
	}
	routerip[0] = atoi(cp);
	if (routerip[0] == 0) return(1);
	for (int i = 1; i < 4; i++)
	{
		cp = strchr(cp, '.');
		if (cp == NULL){
			routerip[0] = 0;
			return(0);
		}
		cp++;
		routerip[i] = atoi(cp);
	}

	return(1);
}


int 
BACdev::GetRouterIP(unsigned char nip[4])
{
	memcpy(nip, routerip, 4);

	return(1);
}


int 
BACdev::GetRouterPort()
{
	return(routeripport);
}


int 
BACdev::SetRouterPort(char *nipp)
{
	int i = 0;

	if (nipp == NULL) return(0);
	
	if (strncmp(nipp, "0x", 2) == 0){
		i = strtol(nipp, NULL, 16);
	} else {
		i = atoi(nipp);
	}
	
	return(SetRouterPort(i));
}


int 
BACdev::SetRouterPort(int nipp)
{
	if (nipp < 0){
		routeripport = 0xBAC0;
		return(0);
	}
	if ((nipp > 47823) && (nipp < 49152)) {
		routeripport = 0xBAC0;
		return 0;
	}
	if (nipp > 0xFFFF) {
		routeripport = 0xBAC0;
		return(0);
	}

	routeripport = nipp;
	if (routeripport == 0) routeripport = 0xBAC0;

	return(1);
}


// returns: 0 if there is any difference between in2 and this
//			1 if in2 is the same as this
int 
BACdev::Compare(BACdev *in2)
{
	if (in2 == NULL) return 0;

	if (ipport != in2->ipport) return 0;
	if (net != in2->net) return 0;
	if (routeripport != in2->routeripport) return 0;
	if (configID != in2->configID) return 0;

	if (strcmp(mac, in2->mac) != 0) return 0;
	if (strcmp(ip, in2->ip) != 0) return 0;
	if (strcmp(routermac, in2->routermac) != 0) return 0;
	if (strcmp(routerip, in2->routerip) != 0) return 0;

	return 1;
}


void 
BACdev::SetIndex(int ind1)
{
	index = ind1;
}


int 
BACdev::GetIndex()
{
	return(index);
}


// MAG 260210 add function, note range for dev obj id is 0 - 0x3FFFFE
// set to -1 for invalid/not set
void 
BACdev::SetDevObjInst(int nid)
{
	if (nid < -1) {
		devObjInst = -1;
		return;
	}
	
	if (nid > 0x3FFFFE) {
		devObjInst = -1;
		return;
	}
	
	devObjInst = nid;
	
	return;
}


// MAG 260227 add function, note range for dev obj id is 0 - 0x3FFFFE
// set to -1 for invalid/not set
void
BACdev::SetDevObjInst(char *nid)
{
	int i = 0;

	if (nid == NULL) return;

	if (strncmp(nid, "0x", 2) == 0) {
		i = strtol(nid, NULL, 16);
	} else {
		i = atoi(nid);
	}
	
	SetDevObjInst(i);
	
	return;
}


int 
BACdev::GetDeviceLabel(char *newlabel)
{
	if (newlabel == NULL) return 0;
	strcpy_s(newlabel, 260, label);
	
	return 1;
}


int
BACdev::SetDeviceLabel(char *newlabel)
{
	if (newlabel == NULL) return 0;
	strcpy_s(label, 260, newlabel);

	return 1;
}


// Sets the configID variable
int
BACdev::SetConfigID(int nid)
{
	if (nid < 0)	return 0;
	if (nid > 4)	return 0;
	configID = nid;

	return 1;
}


// Returns the value of ConfigID
const int
BACdev::GetConfigID()
{
	return configID;
}


// Returns the value of ConfigID
//0:IP/rIP, 1: MAC/rIP, 2: MAC/rMAC, 3: IP only, 4: MAC only
int
BACdev::AutoConfigID()
{
	if (strlen(routermac) == 0) {		//No router, mode 3 or 4
		if (ip[0] == 0) configID = 4;	// No router, Ethernet device mode 4
		else configID = 3;				// No router, UDP/IP device mode 3
	} else if (routerip[0] == 0) {		// has Ethernet router, Ethernet device mode 2
		configID = 2;
	} else if (ip[0] == 0) {			// mode 1
		configID = 1;					// UPD/IP router, UDP/IP device
	} else configID = 0;

	return configID;
}


// Converts string IP "127.0.0.1" to uchar array [127][0][0][1]
int
BACdev::IP_from_string(char* cpin, unsigned char* ip)
{
	char* cp = NULL;
	int i = 0;

	if (cpin == NULL) return(0);
	if (ip == NULL) return(0);
	if (strlen(cpin) < 7) return(0);  // can't be shorter

	cp = cpin;
	try {
		for (i = 0; (i < 4) && (cp != NULL); i++) {
			ip[i] = (unsigned char)atoi(cp);
			cp = strchr(cp, '.');
			if (cp != NULL) cp++;
		}
	}
	catch (const std::exception& e) {
		CString err;
		err.Format(L"ERROR in IP_from_string: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}
	catch (...) {
		CString err;
		err.Format(L"ERROR in IP_from_string");
		theApp.m_BACdlg->m_status.AddString(err);
		return 0;
	}

	return(1);
}
