/*
=============================================================================
	File:	pxPositionCorrector.h
	Desc:	
=============================================================================
*/

#ifndef __PX_POSITION_SOLVER_H__
#define __PX_POSITION_SOLVER_H__

//
//	pxPositionCorrector
//
class pxPositionCorrector {
public:
			pxPositionCorrector();
	virtual	~pxPositionCorrector();

	virtual void Solve( pxSolverInput& input, pxSolverOutput& output ) = 0;

};

#endif // !__PX_POSITION_SOLVER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
