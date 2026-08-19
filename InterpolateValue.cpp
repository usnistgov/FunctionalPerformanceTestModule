#include "InterpolateValue.h"

//extern CFPTMApp theApp;

InterpolateValue::InterpolateValue()
{
	x = 0;
	x0 = 0;
	x1 = 0;
	y0 = 0;
	y1 = 0;
	minOut = 0;		// optional
	maxOut = 0;		// optional
	mode = -1;	// -1=not used 0=no min/max, 1=min only, 2=min and max

	varx = -1;
	varx0 = -1;
	varx1 = -1;
	vary0 = -1;
	vary1 = -1;
	varminOut = -1;
	varmaxOut = -1;
}


InterpolateValue::InterpolateValue(double xin, double x0in, double x1in, double y0in, double y1in)
{
	x = xin;
	x0 = x0in;
	x1 = x1in;
	y0 = y0in;
	y1 = y1in;
	minOut = 0;		// optional
	maxOut = 0;		// optional
	mode = 0;	// 0=no min/max, 1=min only, 2=min and max

	varx = -1;
	varx0 = -1;
	varx1 = -1;
	vary0 = -1;
	vary1 = -1;
	varminOut = -1;
	varmaxOut = -1;
}


InterpolateValue::InterpolateValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin)
{
	x = xin;
	x0 = x0in;
	x1 = x1in;
	y0 = y0in;
	y1 = y1in;
	minOut = minOutin;		// optional
	maxOut = 0;		// optional
	mode = 1;	// 0=no min/max, 1=min only, 2=min and max

	varx = -1;
	varx0 = -1;
	varx1 = -1;
	vary0 = -1;
	vary1 = -1;
	varminOut = -1;
	varmaxOut = -1;
}


InterpolateValue::InterpolateValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin, double maxOutin)
{
	x = xin;
	x0 = x0in;
	x1 = x1in;
	y0 = y0in;
	y1 = y1in;
	minOut = minOutin;		// optional
	maxOut = maxOutin;		// optional
	mode = 2;	// 0=no min/max, 1=min only, 2=min and max

	varx = -1;
	varx0 = -1;
	varx1 = -1;
	vary0 = -1;
	vary1 = -1;
	varminOut = -1;
	varmaxOut = -1;
}


// MAG note values copied from basic init
void
InterpolateValue::Reset()
{
	x = 0;
	x0 = 0;
	x1 = 0;
	y0 = 0;
	y1 = 0;
	minOut = 0;		// optional
	maxOut = 0;		// optional
	mode = -1;	// -1=not used 0=no min/max, 1=min only, 2=min and max

	varx = -1;
	varx0 = -1;
	varx1 = -1;
	vary0 = -1;
	vary1 = -1;
	varminOut = -1;
	varmaxOut = -1;
}


double 
InterpolateValue::GetValue(double xin, double x0in, double x1in, double y0in, double y1in)  // Mode 0
{
	double rv = 0.0;

	rv = y0in + (y1in - y0in) * ((xin - x0in) / (x1in - x0in));

	return rv;
}


double 
InterpolateValue::GetValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin)  // Mode 1, provided with min out value
{
	double rv = 0.0;
	double lval = 0.0;
	double rval = 0.0;

	lval = minOutin;
	rval = y0in + (y1in - y0in) * ((xin - x0in) / (x1in - x0in));
	if (lval > rval) rv = lval;  // use min boundary value
	else rv = rval;		// rval is higher than min boundary value
	//rv = std::min(minOutin, y0in + (y1in - y0in) * ((xin - x0in) / (x1in - x0in)));

	return rv;
}


double 
InterpolateValue::GetValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin, double maxOutin) // Mode 2 if min==std::numeric_limits<double>::lowest(), else Mode 3
{
	double rv = 0.0;
	double lval = 0.0;
	double rval = 0.0;

	if(minOutin == DBL_MIN){
		lval = maxOutin;
		rval = maxOutin, y0in + (y1in - y0in) * ((xin - x0in) / (x1in - x0in));
		if (lval < rval) rv = lval;
		else rv = rval;
		//rv = std::min(maxOutin, y0in + (y1in - y0in) * ((xin - x0in) / (x1in - x0in)));
	} else {
		// calc inner comparison first
		lval = minOutin;
		rval = y0in + (y1in - y0in) * ((xin - x0in) / (x1in - x0in));
		if (lval > rval) rval = lval; // else rval remains the same
		lval = maxOutin;		// now outer comparison
		if (lval > rval) rv = rval; // find min
		else rv = lval;
		//rv = std::min(maxOutin, std::max(minOutin, y0in + (y1in - y0in) * ((xin - x0in) / (x1in - x0in))));
	}

	return rv;
}