#pragma once
#include <vector>
#include <string>
#include <sstream>
// MAG 250204 
// Purpose: To allow management of periodic value inputs. Input consists of a value/variable, and an interval.
// Value is sent to controller at specified interval until next time step. Value is recalculated if necessary each time it is sent.
// PeriodicInput is initiated in TestStep structure
class PeriodicInput
{
protected:
	int		varExpression;	// index into VARlist for evaluated variable (source)
	int		period;			// period= -1 indicates not used/configured
	double	mod;			// if modType in range, mod value is used to modify output value
	int		modType;		// -1:not used, 0:+, 1:-, 2:*, 3:/, 4: mod/var (inverse of #3)
	int		varSet;			// indicates VARlist index value is saved to (destination)
	int		trSource;		// indicates TR list element index (needed for value)

public:
	PeriodicInput();// { varExpression = -1; period = 0; mod = 0.0; modType = -1; varSet = -1; };
	
	const int	GetExpressionIndex() { return varExpression; };	/// Returns index of evaluated (source) variable
	void		SetExpressionIndex(int var) { varExpression = var; };

	const int	GetValueIndex() { return varSet; };				/// Returns index of output (destination) variable
	void		SetValueIndex(int var) { varSet = var; };

	int		GetPeriod() { return period; };
	void	SetPeriod(int var) { period = var; };

	double	GetModValue() { return mod; };
	void	SetModValue(double var) { mod = var; };

	int		GetModType() { return modType; };
	void	SetModType(int var) { if (var < -1) return; if (var > 4) return; modType = var; };

	int		GetTRSource() { return trSource; };
	void	SetTRSource(int var) { trSource = var; };

	void	Reset();
	
	// keep this inline?
	PeriodicInput& operator=(const PeriodicInput& other) {
		if (this != &other) { // Prevent self-assignment
			varExpression = other.varExpression;
			period = other.period;
			mod = other.mod;
			modType = other.modType;
			varSet = other.varSet;
			trSource = other.trSource;
		}
		return *this; // Return a reference to the object for chaining
	}
};

