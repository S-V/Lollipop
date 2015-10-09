/*
=============================================================================
	File:	pxConstraintSolver_PGS.h
	Desc:	A very simple Projected Gauss-Seidel constraint solver.
			The implementation is mainly based on the paper
			"Iterative Dynamics with Temporal Coherence" by Erin Catto, [2005]
			http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf
=============================================================================
*/

#ifndef __PX_CONSTRAINT_SOLVER_PGS_H__
#define __PX_CONSTRAINT_SOLVER_PGS_H__

//
//	pxJacobian - represents a two-body constraint jacobian (J1*v1 + J2*v2 = C)
// (two jacobian blocks from each pairwise constraint)
// it's a 1x12 vector for two bodies.
//
MX_ALIGN_16(struct) pxJacobian
{
	pxVec3	j0Linear, j0Angular, j1Linear, j1Angular;//16*4=64

public:
	PX_INLINE void Set( const pxVec3& linA, const pxVec3& angA, const pxVec3& linB, const pxVec3& angB )
	{
		j0Linear = linA;	j0Angular = angA;		j1Linear = linB;	j1Angular = angB;
	}
};

//
// B = M^1 * J^T (inverse of mass matrix multiplied by transposed jacobian).
// it's a 12x1 matrix.
//
MX_ALIGN_16(struct) pxBMatrix
{
	pxVec3	b0Linear, b0Angular, b1Linear, b1Angular;//16*4=64
};

//
//	pxAVector - represents a delta velocity accumulator.
// a = B*lambda, for a single body it's a 1x6 vector.
//
MX_ALIGN_16(struct) pxAVector
{
	pxVec3	linear, angular;
};

PX_OPTIMIZE("reduce size; switch from AoS(array of structures) to SoA?");
//
//	pxSolverConstraint
//
MX_ALIGN_64(struct) pxSolverConstraint
{
	pxJacobian		J;		//«64 J is 1x12
	pxBMatrix		B;		//«64 B = M^1 * J^T, B is 12x1
	pxReal			JB;		//«4 precomputed effective inverse mass: JB = dot(J,B)
	pxReal			invJB;	//«4 precomputed effective mass: invJB = (J*B)^-1

	pxReal			lambda;	//«4 lambda - solution of the equation: J*B*lambda = rhs
	pxReal			lo;		//«4 lower limit
	pxReal			hi;		//«4 higher limit
	pxReal			rhs;	//«4 precomputed: rhs = (bias/dt) - J * ( V1/dt + M^-1 * Fext )

	pxU4			iA;	//«4 index of the first body
	pxU4			iB;	//«4 index of the second body
	//total: 160 bytes (aligned to 176 bytes), 128 bytes without padding
};

typedef TList< pxSolverConstraint >	pxConstraintArray;

//
//	pxConstraintSolver_PGS
//
class pxConstraintSolver_PGS : public pxConstraintSolver {
public:
						pxConstraintSolver_PGS();
	OVERRIDEN			~pxConstraintSolver_PGS();

	OVERRIDES(pxConstraintSolver)	void Solve( pxSolverInput& input, pxSolverOutput& output );

private_internal:
	void SetupContactConstraints( const pxSolverInput& input );

	pxUInt BuildSingleContactConstraint(
		const pxSolverInput& input,
		const pxContactPoint& contact,
		pxRigidBody* oA, pxRigidBody* oB,
		pxReal frictionLimit,
		pxReal restitution,
		pxSolverConstraint* constraints,	// array of constraint rows
		pxUInt offset // total number of filled rows in the above array
	);

private:
	pxReal		mDeltaTime, mInvDeltaTime;	// Time step information.
	
	pxConstraintArray	mConstraints;
};

#endif // !__PX_CONSTRAINT_SOLVER_PGS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
