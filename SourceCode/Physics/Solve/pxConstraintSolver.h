/*
=============================================================================
	File:	pxConstraintSolver.h
	Desc:	
=============================================================================
*/

#ifndef __PX_CONSTRAINT_SOLVER_H__
#define __PX_CONSTRAINT_SOLVER_H__

class pxSolverInput;

struct pxSolverOutput
{
};

//
// pxSolverSettings
//
class pxSolverSettings {
public:
	pxReal			erp;	// error reduction parameter (used as Baumgarte factor), usually [0.1 - 0.8]
	pxReal			cfm;	// constraint force mixing

	// A small length used as a constraint tolerance (e.g. allowed penetration).
	// Usually it is chosen to be numerically significant, but visually insignificant.
	pxReal			slop;

	pxReal			precision;	// requested solver precision
	pxUInt			minIterations;// early exit is only allowed after a certain minimum iteration count
	pxUInt			maxIterations;// to prevent infinite looping

public:
	pxSolverSettings()
	{
		setDefaults();
	}
	void setDefaults()
	{
		erp = 0.4f;
		cfm = 0.0f;
		slop = 0.01f;//0.005f;

		precision = 0.01f;
		minIterations = 4;
		maxIterations = 8;
	}
	bool isOk() const
	{
		return 1
			&& CHK(IsInRangeInc(erp,0.0f,1.0f))
			&& CHK( slop >= 0.0f )

			&& CHK(minIterations > 0)
			&& CHK(maxIterations < 1000)
			;
	}
};

//
//	pxSolverStats
//
class pxSolverStats {
public:
	pxUInt	numConstraints;	// number of constraints solved
	pxUInt	numIterations;	// number of iterations performed

public:
	pxSolverStats() {
		Reset();
	}
	void Reset() {
		numConstraints = 0;
		numIterations = 0;
	}
};

//
//	pxConstraintSolver
//
//	A constraint solver simultaneously solves and maintains
//	a set of constraints for a set of rigid bodies.
//
class pxConstraintSolver {
public:
	pxConstraintSolver();
	virtual ~pxConstraintSolver();

	virtual void Solve( pxSolverInput& input, pxSolverOutput& output ) = 0;

	const pxSolverStats& GetStats() const { return mStats; }

	pxSolverSettings & Settings() { return mSettings; }

protected_internal:
	pxSolverSettings	mSettings;
	pxSolverStats		mStats;
};

#endif // !__PX_CONSTRAINT_SOLVER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
