#pragma once

#include <string>

// MAG  240829 add this to support Taylor Engineering collaboration
// This supports the points on a controller, not point names and variables names read from an input file.
class NamedPoint {
protected:
	std::string	label;		// as read from device object-name property (77)
	std::string description;// if used, description label from config file or set by user. BACnet property (28) description.
	std::string alias;		// Optional additional description, probably for BDS-FPTM mapping. Not a BACnet property.
	int			object;		// BACnet object type (i.e. AI=0, BV=5, etc.)
	int			instance;	// instance of BACnet object (i.e. 2 for AI02, 5 for BV05)
	int			bacdevice;	// identifier for device, index into BACnetList (NOT device instance)
public:
	NamedPoint() { object = instance = bacdevice = -1; };
	NamedPoint(char* l1) { label = l1; object = instance = bacdevice = -1; }
	NamedPoint(char* l1, int obj, int ins, int bac) { label = l1; object = obj; instance = ins; bacdevice = bac; }

	void GetLabel(char* opl) { if (opl != NULL) strcpy_s(opl, 30, label.data()); }
	std::string GetLabel() { return label; };
	void SetLabel(char* opl) { if (opl != NULL) label = opl; };
	void SetLabel(std::string opl) { label = opl; };

	void GetDescription(char* opl) { if (opl != NULL) strcpy_s(opl, 30, description.data()); }
	std::string GetDescription() { return description; };
	void SetDescription(char* opl) { if (opl != NULL) description = opl; };
	void SetDescription(std::string opl) { description = opl; };
	
	void SetAlias(char* opl) { if (opl != NULL) alias = opl; };
	void SetAlias(std::string opl) { alias = opl; };
	std::string GetAlias() { return alias; };

	// sets the value of object
	int SetBACnetObject(int nobj);
	
	// returns the value of the object
	const int GetBACnetObject() { return object; };
	
	// sets the value of the object instance
	int SetInstance(int ninst);
	
	// returns the value of the oject instance
	const int GetInstance() { return instance; };
	
	// Sets the value of bacdevice
	int SetDevice(int id);// { bacdevice = id; };  // MAG NOTE - check against BACnetList.size() before call to this subroutine!!!
	
	// Returns the value of bacdevice
	const int GetDevice() { return bacdevice; };
};
