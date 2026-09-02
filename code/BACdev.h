#pragma once

#include "resource.h"		// main symbols
#include <string.h>
#include <vector>
#include "BACnet Point.h"
//#include "FPTM Dlg.h"
//

class BACdev
{
public:
	BACdev();
	BACdev(int index);	// constructor which sets the index
	~BACdev();

	// get/set functions
	int			SetMAC(char *mac);			// used for 12-byte human readable version
	int			SetMAC(unsigned char *mac);	// used for 6-byte digital version
	const int	GetMAC(char *mac);
	int			SetIP(unsigned char ip[4]);
	int			SetIP(char *ip);
	int			GetIP(unsigned char ip[4]);
	int			GetIP(unsigned int ip[4]);
	int			GetIPPort();
	int			SetIPPort(int ipp);
	int			SetIPPort(char *ipp);
	const int	GetNet();
	int			SetNet(int net);
	int			SetNet(char *net);
	int			SetRouterMAC(char *mac);
	int			SetRouterMAC(unsigned char *mac);	// used for 6-byte digital version
	int			GetRouterMAC(char *mac);
	int			SetRouterIP(unsigned char ip[4]);
	int			SetRouterIP(char *ip);
	int			GetRouterIP(unsigned char ip[4]);
	int			GetRouterPort();
	int			SetRouterPort(int ipp);
	int			SetRouterPort(char *ipp);
	int			SetDeviceLabel(char *newlabel);
	int			GetDeviceLabel(char *newlabel);
	// convert string-based IP to binary IP
	int			IP_from_string(char *cpin, unsigned char *ip);
	int			Compare(BACdev *in2);				// compare this with another BACdev
	int			GetIndex(void);	// returns the index
	//*BACdev GetDevice(int index);	// return pointer to BACdev indicated by index, or NULL
	void		SetIndex(int ind1);	// sets the index
	void		Reset();		// resets all data fields
	void		AddPoint(BACnetPoint ip) { pointList.push_back(ip); };
	// Sets the configID variable
	int			SetConfigID(int nid);
	// Returns the value of ConfigID
	const int	GetConfigID();
	// Auto determines the proper configID
	int			AutoConfigID();

	int			GetDevObjInst() { return devObjInst; };
	void		SetDevObjInst(int nid);
	void		SetDevObjInst(char* nid);
protected:
	// Note that all addresses are stored as text versions, not binary versions, except IP addresses
	char			mac[20];		// printable, human readable version of MAC
	char			ip[20];			// ex. "129.6.162.135" is [129][6][162][135], not readable text
	int				ipport;			// default 0xBAC0
	int				net;
	char			routermac[20];	// like mac, printable human readable version of MAC
	char			routerip[20];	// like ip, stored as numbers, not readable text
	int				routeripport;	// default 0xBAC0
	int				devObjInst;		// device object instance, unique on network.

	int				configID;	//0:IP/rIP, 1: MAC/rIP, 2: MAC/rMAC, 3: IP only, 4: MAC only
	int				index;		// MAG identifier, used when reading or writing file to associate with event.  
	static int		maxIndex;	// highest assigned index
	char			label[260];	// text label for display
	// MAG note- name simlar to namedPointList
	std::vector<BACnetPoint> pointList; // MAG 241115 list of bacnet points (objects, usally AO, SO, etc.) on the contoller
};