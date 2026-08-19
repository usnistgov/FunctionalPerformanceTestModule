#include "NamedPoint.h"


int
NamedPoint::SetBACnetObject(int nobj)
{
	if (nobj < 0) return 0;
	if (nobj > 19) return 0; // MAG Multi-State-Value (19) is currently the highest object id supported

	object = nobj;

	return 1;
}

int
NamedPoint::SetInstance(int ninst)
{
	if (ninst < 0) {
		return 0;
	}
	if (ninst >= 4194303) ninst &= 0x3FFFFF;// MAG 170420 sometimes it's been combined with object encoding, this strips off the type and leaves instance

	instance = ninst;

	return 1;
}

// MAG NOTE- check against BACnetList.size() before call to this subroutine!!!
int
NamedPoint::SetDevice(int ninst)
{
	if (ninst < 0) return 0;

	bacdevice = ninst;

	return 1;
}
