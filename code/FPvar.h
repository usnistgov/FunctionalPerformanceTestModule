#pragma once

#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <stdlib.h>
#include <ctime>
#include <string>
// functional performance scripting variables
// pretty basic to start, add more functionality as needed.
// NOTE- when a TR with a non-null FPvar is read, the value read is written to the FPvar
// MAG 241028 add errBound (error bounds) to support CalNEXT project
class FPvar {
protected:
	double	value;		// if using a stored value
	double	prevValue;	// value at end of last test step, used with LAST
	double	errBound;	// output tolerance (column C in D9). err bounds are +/-
	std::string	label;
public:
	FPvar() { value = 0; prevValue = 0; errBound = 0; label = ""; };
	FPvar(std::string l1) { value = 0; prevValue = 0; errBound = 0; label = l1; }
	FPvar(std::string l1, double v1) { value = v1; prevValue = 0; errBound = 0; label = l1; }
	FPvar(std::string l1, double v1, double v2) { value = v1; prevValue = 0; errBound = v2; label = l1; }
	double	GetValue() { return value; };
	double	GetPrevValue() { return prevValue; };
	void	SetValue(double v1);
	void	SetValue(char* v1);
	void	SetValue(std::string v1) { SetValue((char *)v1.c_str()); }; // call text version
	void	BumpPrevious() { prevValue = value; };
	std::string GetLabel() { return label; }
	void	GetLabel(char* vlabel) { if (vlabel == NULL) return; strcpy_s(vlabel, 100, label.c_str()); };
	void	SetLabel(std::string opl) { label = opl; }
	int		IsVar(char* inlab);
	int		IsVar(std::string inlab) { if (inlab.compare(label) == 1) return 1; return 0; };
	void	SetErrBound(double v1) { errBound = v1; };
	void	SetErrBound(char* v1);
	void	SetErrBound(std::string v1) { SetErrBound((char *)v1.c_str()); };
	double	GetErrBound() { return errBound; };
};