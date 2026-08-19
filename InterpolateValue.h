#pragma once
#include <stdafx.h>
#include <algorithm>
#include <limits>

//#include "FPTM App.h"
// MAG 250205 Add Class
// Purpose: To store interpolate command values and to calculate the interpolated value.
// Note this is to support Taylor/CalNEXT test scripts.
// Note: This is called "interpolate" in the script files but it is just scaling values, not interpolating.
//extern CFPTMApp theApp;

class InterpolateValue
{
protected:
	// MAG NOTE- these might all also be variable, need to add var index options... values will have to be set externally then GetValue called.
	double	x;
	double	x0;
	double	x1;
	double	y0;
	double	y1;
	double	minOut;		// optional
	double	maxOut;		// optional

	// indexes for when variables are used, -1 if no var
	int	varx;
	int	varx0;
	int	varx1;
	int	vary0;
	int	vary1;
	int	varminOut;		// optional
	int	varmaxOut;		// optional

	int		mode;	// -1=not used 0=no min/max, 1=min only, 2=max only, 3=min and max

public:
	InterpolateValue();
	InterpolateValue(double xin, double x0in, double x1in, double y0in, double y1in);
	InterpolateValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin);
	InterpolateValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin, double maxOutin);

	void Reset();

	int GetMode() { return mode; };
	void SetMode(int var) { mode = var; };

	//double GetValue(double xt); // calculate value at x=xt
	double GetValue(double xin, double x0in, double x1in, double y0in, double y1in);  // Mode 0
	double GetValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin);  // Mode 1
	double GetValue(double xin, double x0in, double x1in, double y0in, double y1in, double minOutin, double maxOutin); // Mode 2 if min==std::numeric_limits<double>::min(), else Mode 3

	void SetX(double var) { x = var; };
	void SetX0(double var) { x0 = var; };
	void SetX1(double var) { x1 = var; };
	void SetY0(double var) { y0 = var; };
	void SetY1(double var) { y1 = var; };
	void SetMinOut(double var) { minOut = var; mode = 1; };
	void SetMaxOut(double var) { maxOut = var; mode = 2; };
	void SetMinMaxOut(double varMin, double varMax) { minOut = varMin; maxOut = varMax; mode = 3; };

	double GetX( ) { return  x; };
	double GetX0() { return x0; };
	double GetX1() { return x1; };
	double GetY0() { return y0; };
	double GetY1() { return y1; };
	double GetMinOut() { return minOut; };
	double GetMaxOut() { return maxOut; };
	//double GetMinMaxOut(double varMin, double varMax) { minOut = varMin; maxOut = varMax; mode = 3; };

	void SetVarIndexX( int var) { varx = var; };
	void SetVarIndexX0(int var) { varx0 = var; };
	void SetVarIndexX1(int var) { varx1 = var; };
	void SetVarIndexY0(int var) { vary0 = var; };
	void SetVarIndexY1(int var) { vary1 = var; };
	void SetVarIndexMinOut(int var) { varminOut = var; mode = 1; };
	void SetVarIndexMaxOut(int var) { varmaxOut = var; mode = 3; };
	void SetVarIndexMinMaxOut(int varMin, int varMax) { varminOut = varMin; varmaxOut = varMax; mode = 3; };

	int GetVarIndexX() {  return varx; };
	int GetVarIndexX0() { return varx0; };
	int GetVarIndexX1() { return varx1; };
	int GetVarIndexY0() { return vary0; };
	int GetVarIndexY1() { return vary1; };
	int GetVarIndexMinOut() { return varminOut; };
	int GetVarIndexMaxOut() { return varmaxOut; };
};

