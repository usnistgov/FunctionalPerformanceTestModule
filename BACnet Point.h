#pragma once

#include <string>

// MAG 241114
// Purpose: to hold info and metadata about one BACnet object on a controller.
// Added as part of CalNEXT collaboration
// MAG NOTE not implemented yet- maybe not necessary?
// TODO Class is not finished (or used much). Might be redundant?
class BACnetPoint
{
protected:
	int		object_type;		// the type of object
	int		obj_inst;			// object instance
	int		prop_id;			// property number requested
	double	value;				// value of data at this point
	double	bounds;				// boundry +/- range for this point
	int		mapIndex;			// index in PointVarMap

};