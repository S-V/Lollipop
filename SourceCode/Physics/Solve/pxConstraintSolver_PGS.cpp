/*
=============================================================================
	File:	pxConstraintSolver_PGS.cpp
	Desc:	A very simple Projected Gauss-Seidel constraint solver.
	ToDo:	use only diagonal inertia tensors ( mul(mat1x3,vec3) instead of mul(mat3x3, vec3) )
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

// We need to solve 'J*B*lambda = rhs' for 'lambda' (given initial 'lambda0'),
// where
// lamda - vector of Lagrange multipliers that we solve for (each lambda can be bounded by lower and higher limits),
// J - constraint jacobian,
// B = M^-1 * J^T (inverse of mass matrix multiplied by the transposed jacobian),
// rhs = (bias/dt) - J * ( V1/dt + M^-1 * Fext ),
//        bias - vector of constraint biases, V1 - initial velocities, Fext - external forces.
// (rhs is denoted as the greek letter 'eta' in Erin Catto's papers.)

// please note that (J*B) is constant and can be precomputed.
// Note that J*B == J * M^-1 * J^T is called effective inverse mass matrix
// and if it has full rank (i.e. none of constraints are redundant)
// then it's symmetric and positive definite (PD).
// (if matrix A is PD if (X^T * A * X > 0) for any non-zero X.)
// Note: the Gauss-Seidel method can not converge when A is not diagonally dominant (PD).

#define ALLOC( num )	(pxReal*)mxStackAlloc16( (num) * sizeof(pxReal) )
#define ALLOC2(typ,num)	(typ*)mxStackAlloc16((num) * sizeof(typ))

#define PX_SOLVER_INFINITY	9e6f

#define STAT(x)

#define ENSURE(x)

// HACKS:

// number of tangential (friction) directions, this can 0, 1 or 2
// 0 - completely eliminates friction (fast)
// 1 - sucks (esp. for spherical objects)
// 2 - looks ok, but slower
static const unsigned gNumFrictionDirections = 2;

static const bool gUseWarmstarting = 0;
static const pxReal gWarmstartingFactor = 1.0f;
PX_WHY("which values are the best?");
static const pxReal gInitialLambda = 0.1f;	// used as initial guess when warmstarting is disabled
static const pxReal gLambdaRelaxationFactor = 0.92f;	// must be in range [0.9 .. 1.0]

static const bool gApplyDampingAfterVelocityUpdate = 0;//prevent jitter
static const bool gClampVelocities = 0;//don't allow velocities go crazy

/*================================
	pxConstraintSolver_PGS
================================*/

pxConstraintSolver_PGS::pxConstraintSolver_PGS()
{
	mDeltaTime = 0.0f;
	mInvDeltaTime = 0.0f;
	mConstraints.Reserve( 1024 );
}

pxConstraintSolver_PGS::~pxConstraintSolver_PGS()
{
	mConstraints.Clear();
}

// computes (dot(a1,b1) + dot(a2,b2) + dot(a3,b3) + dot(a4,b4)).
FORCEINLINE static
pxReal Dot_4x4_Vec3(
	const pxVec3& a0, const pxVec3& a1, const pxVec3& a2, const pxVec3& a3,
	const pxVec3& b0, const pxVec3& b1, const pxVec3& b2, const pxVec3& b3 )
{
	return a0.Dot( b0 ) + a1.Dot( b1 ) + a2.Dot( b2 ) + a3.Dot( b3 );
}

// B = M^1 * J^T
FORCEINLINE static
void ComputeBMatrix(
	const pxJacobian& J,
	const pxRigidBody* RESTRICT( oA ), const pxRigidBody* RESTRICT( oB ),
	pxBMatrix & B )
{
	B.b0Linear	= J.j0Linear * oA->GetInvMass();
	B.b0Angular	= oA->GetInvInertiaWorld() * J.j0Angular;
	B.b1Linear	= J.j1Linear * oB->GetInvMass();
	B.b1Angular	= oB->GetInvInertiaWorld() * J.j1Angular;
}

// JB = dot(J,B)
FORCEINLINE static
pxReal ComputeJdotB( const pxJacobian& J, const pxBMatrix& B )
{
	return J.j0Linear.Dot( B.b0Linear ) + J.j0Angular.Dot( B.b0Angular )
		+ J.j1Linear.Dot( B.b1Linear ) + J.j1Angular.Dot( B.b1Angular );
}

// rhs = (bias/dt) - J * ( V1/dt + M^-1 * Fext )
FORCEINLINE static
pxReal ComputeRhs(
	const pxJacobian& J, const pxBMatrix& B,
	const pxReal bias, const pxReal invDeltaTime,
	const pxRigidBody* RESTRICT( oA ), const pxRigidBody* RESTRICT( oB ) )
{
	// JdotAcc = J * ( V1/dt + M^-1 * Fext )
	pxReal JdotAcc = 0.0f;

	// for the first body
	const pxVec3 acc_linA = oA->GetLinearVelocity() * invDeltaTime + oA->GetTotalForce() * oA->GetInvMass();
	const pxVec3 acc_angA = oA->GetAngularVelocity() * invDeltaTime + oA->GetInvInertiaWorld() * oA->GetTotalTorque();

	// for the second body
	const pxVec3 acc_linB = oB->GetLinearVelocity() * invDeltaTime + oB->GetTotalForce() * oB->GetInvMass();
	const pxVec3 acc_angB = oB->GetAngularVelocity() * invDeltaTime + oB->GetInvInertiaWorld() * oB->GetTotalTorque();

	// for the first body
	JdotAcc += J.j0Linear.Dot( acc_linA );
	JdotAcc += J.j0Angular.Dot( acc_angA );

	// for the second body
	JdotAcc += J.j1Linear.Dot( acc_linB );
	JdotAcc += J.j1Angular.Dot( acc_angB );

	// rhs = (bias/dt) - J * ( V1/dt + M^-1 * Fext )
	return ( (bias * invDeltaTime) - JdotAcc );
}

FORCEINLINE static
pxReal RestitutionCurve( pxReal relativeVelocity, pxReal restitution )
{
	return ( restitution * -relativeVelocity );
}

FORCEINLINE static
void DampBodyVelocities( pxVec3& linearVelocity, pxVec3& angularVelocity, pxReal deltaTime )
{
	const pxReal linearDamping = 0.5f;
	const pxReal angularDamping = 0.6f;
	linearVelocity *= pxReal(1.) - linearDamping * deltaTime;
	angularVelocity *= pxReal(1.) - angularDamping * deltaTime;
}

//----------------------------------------------------------------
//
// returns the number of added constraint rows
//
pxUInt pxConstraintSolver_PGS::BuildSingleContactConstraint(
	const pxSolverInput& input,
	const pxContactPoint& contact,
	pxRigidBody* oA, pxRigidBody* oB,
	pxReal frictionLimit,
	pxReal restitution,
	pxSolverConstraint* constraints,	// array of constraint rows
	pxUInt offset // number of filled rows in the above array so far
)
{
	PX_ASSERT_PTR2(oA,oB);
	AssertPtr(constraints);

	const pxReal invDeltaTime = mInvDeltaTime;	// inverse time step

	pxUInt numJacobianRows = 0;	//« this value is returned from the function

	const pxU4 indexA = oA->m_solverIndex;
	const pxU4 indexB = oB->m_solverIndex;

	// positions of contact points with respect to body PORs
	const pxVec3 rA( contact.position - oA->GetCenterOfMassPosition() );
	const pxVec3 rB( contact.position - oB->GetCenterOfMassPosition() );

	const pxVec3 normal( contact.getNormal() );

	const pxVec3 rA_cross_N( rA.Cross( normal ) );	// rA x normal
	const pxVec3 rB_cross_N( rB.Cross( normal ) );	// rB x normal

	// Compute the jacobians for non-penetration and friction constraints.

	//----------------------------------------
	//				Non-penetration.
	//----------------------------------------

	// build a contact constraint Jacobian

	pxSolverConstraint & contactConstraint = constraints[ offset + numJacobianRows++ ];

	// Constraint force due to normal acceleration will act in the direction
	// of (J0.linear) at the first body and (J1.linear) at the second body.
	// It will produce (J0.linear * lambda) force and (J0.angular * lambda) torque
	// on the first body and (J1.linear * lambda) force and (J1.angular * lambda)
	// torque on the second body.

	// build jacobians for normal direction
	contactConstraint.J.Set( normal, rA_cross_N, -normal, -rB_cross_N );

	// compute B = M^1 * J^T

	ComputeBMatrix( contactConstraint.J, oA, oB, contactConstraint.B );

	contactConstraint.JB = ComputeJdotB( contactConstraint.J, contactConstraint.B );
	contactConstraint.invJB = mxReciprocal( contactConstraint.JB );

	// So you have two objects and one contact point with a contact normal.
	// Start by computing the velocity at the contact point for both objects
	// and compute the difference between those vectors.
	// Project that difference onto the contact normal (dot product).
	// This is the contact's relative velocity along the contact normal
	// and it indicates how much the objects are moving towards each other or away from each other at the contact point.
	// If it's positive, the objects are moving away from each other and we're done.
	// If it's negative we need to proceed onto computing and applying an impulse.
/**/

	// projection of the relative velocity onto the contact normal
	const pxReal relVelDotN = Dot_4x4_Vec3(
		contactConstraint.J.j0Linear,	contactConstraint.J.j0Angular,	contactConstraint.J.j1Linear,	contactConstraint.J.j1Angular,
		oA->GetLinearVelocity(),		oA->GetAngularVelocity(),		oB->GetLinearVelocity(),		oB->GetAngularVelocity()
	);

//	NOTE: 'relVelDotN2' should be equal to the above value:
//	pxReal relVelDotN2 = (oA->GetVelocityInLocalPoint(rA) - oB->GetVelocityInLocalPoint(rB)).Dot( normal );
//	ENSURE( EqualFloats( relVelDotN, relVelDotN2 ) );

/** /
	const pxReal velocityThreshold = 0.1f;

	if( relVelDotN < -velocityThreshold ) {
		printf("-- colliding contact\n");
	}
	else
	if( relVelDotN > +velocityThreshold ) {
		printf("-- separating contact\n");
	}
	else {
		printf("-- resting contact\n");
	}
/**/

	// Baumgarte stabilization incorporated into velocity bias:
	// Instead of the bias (position error correction) use the relative velocity along the contact normal on the right hand side.
	pxReal bounce = RestitutionCurve( relVelDotN, restitution );
	PX_BRANCH("this is in Bullet,too");
	if( bounce < 0.0f ) {
		bounce = 0.0f;
	}

	// separation distance in the direction of contact normal (penetration depth if > epsilon)
	const pxReal depth = contact.getDepth();

	// Allow a very small amount of interpenetration and add a correction term only when the threshold is exceeded.
	const pxReal positionalError =
		( depth > mSettings.slop )
		?
		depth - mSettings.slop
		:
		REAL(0.0)
		;
	ENSURE( positionalError >= 0.0f );


	// Constraint bias (for stabilization, bounce, etc);
	// it will be multiplied by invDeltaTime in ComputeRhs().
	// Erin Catto: You can achieve restitution by simply augmenting the bias velocity (mentioned in the slides) with a restitution term.
	const pxReal bias = (positionalError * mSettings.erp) + bounce;	// Add bounce as a velocity bias.

	ENSURE( bias >= 0.0f );

	//DBGOUT("Constraint bias: %f\n",depth);
	//pxReal	cfm;	//« constraint force mixing
	//contactConstraint.cfm = 0.0f;



	// set LCP limits for normal
	contactConstraint.lo = 0.0f;
	contactConstraint.hi = PX_SOLVER_INFINITY;

	contactConstraint.rhs = ComputeRhs(
		contactConstraint.J, contactConstraint.B,
		bias, invDeltaTime,
		oA, oB
	);

	// start with initial guess - set initial values of 'lamda'
	if( gUseWarmstarting ) {
		// nothing
	} else {
		contactConstraint.lambda = gInitialLambda;
	}

	contactConstraint.iA = indexA;
	contactConstraint.iB = indexB;

	//----------------------------------------
	//				Friction.
	//----------------------------------------

	Assert(gNumFrictionDirections<=2);
	if( gNumFrictionDirections == 2 )
	{
		// now do jacobians for tangential forces

		// two vectors tangential to normal
		pxVec3 t1, t2; // two vectors tangential to normal
		TPlaneSpace( normal, t1, t2 );

		// Dirk Gregorius:
		// Would I gain an advantage when setting up the first friction direction
		// parallel to the tangential direction of the tangential velocity at the contact point
		// instead of choosing two arbitrary perpendicular vectors in the contact plane?

		// Kenny Erleben:
		// Depends on how you do the projection (I assume you are using straightforward PGS),
		// if you just project one direction after the other then you will gain better accuracy
		// in case of dynamic friction.

		// first friction direction
		pxSolverConstraint & friction_constraint_1 = constraints[ offset + numJacobianRows++ ];

		friction_constraint_1.J			.Set( t1, rA.Cross(t1), -t1, -(rB.Cross(t1)) );
		ComputeBMatrix( friction_constraint_1.J, oA, oB, friction_constraint_1.B );
		friction_constraint_1.JB		= ComputeJdotB( friction_constraint_1.J, friction_constraint_1.B );
		friction_constraint_1.invJB		= mxReciprocal(friction_constraint_1.JB);
		friction_constraint_1.lo		= -frictionLimit;
		friction_constraint_1.hi		= +frictionLimit;
//		friction_constraint_1.cfm		= 0.0f;
		friction_constraint_1.rhs		= ComputeRhs( friction_constraint_1.J, friction_constraint_1.B, 0.0f/*bias*/, invDeltaTime, oA, oB );
		friction_constraint_1.lambda	= 0.0f;
		friction_constraint_1.iA		= indexA;
		friction_constraint_1.iB		= indexB;


		// second friction direction
		pxSolverConstraint & friction_constraint_2 = constraints[ offset + numJacobianRows++ ];

		friction_constraint_2.J			.Set( t2, rA.Cross(t2), -t2, -(rB.Cross(t2)) );
		ComputeBMatrix( friction_constraint_2.J, oA, oB, friction_constraint_2.B );
		friction_constraint_2.JB		= ComputeJdotB( friction_constraint_2.J, friction_constraint_2.B );
		friction_constraint_2.invJB		= mxReciprocal(friction_constraint_2.JB);
		friction_constraint_2.lo		= -frictionLimit;
		friction_constraint_2.hi		= +frictionLimit;
//		friction_constraint_2.cfm		= 0.0f;
		friction_constraint_2.rhs		= ComputeRhs( friction_constraint_2.J, friction_constraint_2.B, 0.0f/*bias*/, invDeltaTime, oA, oB );
		friction_constraint_2.lambda	= 0.0f;
		friction_constraint_2.iA		= indexA;
		friction_constraint_2.iB		= indexB;
	}
	else if( gNumFrictionDirections == 1 )
	{
		const pxVec3 relativeVelocity =
			(oA->GetLinearVelocity() + rA_cross_N.Cross(oA->GetAngularVelocity()))
			-
			(oB->GetLinearVelocity() + rB_cross_N.Cross(oB->GetAngularVelocity()))
			;

		// friction direction = (-1) * (relative velocity projected onto the contact surface)
		pxVec3 u = relativeVelocity - normal * relativeVelocity.Dot(normal);
		pxReal len = u.LengthSqr();
		if( len < PX_EPSILON ) {
			goto L_End;
		}
		u /= mxSqrt(len);

		pxSolverConstraint & friction_constraint = constraints[ offset + numJacobianRows++ ];

		friction_constraint.J		.Set( -u, -(rA.Cross(u)), u, rB.Cross(u) );
		ComputeBMatrix( friction_constraint.J, oA, oB, friction_constraint.B );
		friction_constraint.JB		= ComputeJdotB( contactConstraint.J, contactConstraint.B );
		friction_constraint.invJB	= mxReciprocal(contactConstraint.JB);
		friction_constraint.lo		= -frictionLimit;
		friction_constraint.hi		= +frictionLimit;
//		friction_constraint.cfm		= 0.0f;
		friction_constraint.rhs		= ComputeRhs( friction_constraint.J, friction_constraint.B, 0.0f/*bias*/, invDeltaTime, oA, oB );
		friction_constraint.lambda	= 0.0f;
		friction_constraint.iA		= indexA;
		friction_constraint.iB		= indexB;
	}
L_End:
	return numJacobianRows;
}

//----------------------------------------------------------------

void pxConstraintSolver_PGS::SetupContactConstraints( const pxSolverInput& input )
{
	// 3 because we set 1 constraint to ensure non-penetration
	// and max. 2 constraints for two friction directions.
	const pxUInt maxConstraints = input.contacts->totalNumContacts * 3;
	mConstraints.Reserve( maxConstraints );
//	MemSet(mConstraints.ToPtr(),0,maxConstraints*sizeof(pxSolverConstraint));

	pxUInt numConstraints = 0;

	const pxUInt numManifolds = input.contacts->numManifolds;

	for(pxUInt iManifold = 0;
		iManifold < numManifolds;
		iManifold++)
	{
		const pxContactManifold* manifold = input.contacts->manifolds[ iManifold ];

		const pxUInt numPoints = manifold->numPoints;

		//@fixme: this shouldn't happen
		if( !numPoints ) {
			continue;
		}

		pxRigidBody * oA = c_cast(pxRigidBody*) manifold->oA;
		pxRigidBody * oB = c_cast(pxRigidBody*) manifold->oB;

		//Assert(oA->IsMovable());

		const pxMaterial & materialA = Physics::GetMaterial( oA->m_material );
		const pxMaterial & materialB = Physics::GetMaterial( oB->m_material );

		const pxReal friction = pxCombineFriction( materialA.friction, materialB.friction );
		const pxReal restitution = pxCombineRestitution( materialA.restitution, materialB.restitution );

		// Erin Catto:
		// In Coulomb's friction law, the static and dynamic friction forces have a magnitude
		// that is bounded by the normal force magnitude. This creates an awkward coupling
		// between the constraint multipliers. Using this relationship would complicate our
		// solver and decrease robustness. Therefore, we use a simpler friction model where
		// the friction force is bounded by a constant value:
		// -mu*mc*g <= lambda1 <= mu*mc*g,
		// -mu*mc*g <= lambda2 <= mu*mc*g.
		// Each contact point is assigned a certain amount of mass 'mc'.
		// The acceleration of gravity is g.
		// Typically a body's mass is divided uniformly between the current contact points. 
		// Our experience is that this friction model is sufficient for video games. Static
		// friction works well. Boxes rest on slopes. Dynamic friction is also realistic.
		// However, box stacking friction is unrealistic because lower boxes slide just as easily as
		// upper boxes. The normal force does not affect the strength of the friction.
		//
		const pxReal frictionMultiplier = (input.gravity * pxReal(0.5)) * (oA->GetMass() + oB->GetMass()) / (pxInt)manifold->numPoints;	//casting 'unsigned' to 'int' because signed int by float division is said to be faster

		for( pxUInt iContact = 0;
			iContact < numPoints;
			iContact++ )
		{
			const pxContactPoint & contact = manifold->points[ iContact ];

			const pxReal frictionLimit = friction * frictionMultiplier;

			numConstraints += BuildSingleContactConstraint(
				input, contact, oA, oB,
				frictionLimit, restitution,
				mConstraints.ToPtr(), numConstraints
				);
		}
	}

	mConstraints.SetNum( numConstraints );
}

//----------------------------------------------------------------

//
// returns deltaLambda
//
static FORCEINLINE
pxReal SolveSingleConstraintRow(
	pxSolverConstraint & constraint,
	pxAVector * vA	// accumulator, vector a = Sum( B * lambda ), dim(a) = number of bodies.
	)
{
	// sum = Jsp(bodyA) * vA(bodyA) + Jsp(bodyB) * vA(bodyB)
	const pxReal sum = Dot_4x4_Vec3(
		constraint.J.j0Linear,		constraint.J.j0Angular,	constraint.J.j1Linear,			constraint.J.j1Angular,
		vA[constraint.iA].linear,	vA[constraint.iA].angular,	vA[constraint.iB].linear,	vA[constraint.iB].angular
	);

	// deltaLambda = (eta - Jsp(bodyA) * vA(bodyA) - Jsp(bodyB) * vA(bodyB)) / Di
	pxReal deltaLambda = (constraint.rhs - sum) * constraint.invJB;

	const pxReal lambda0 = constraint.lambda;

	// Projection via clamping: lambda = max( lo, min( lambda0+deltaLambda, hi ).

	constraint.lambda = clampf( lambda0 + deltaLambda, constraint.lo, constraint.hi );

	deltaLambda = constraint.lambda - lambda0;

	// Keep the vA vector equal to (B*lambda) : vA += deltaLambda * B.

	// vA(bodyA) += deltaLambda * B(bodyA)
	vA[ constraint.iA ].linear.AddScaled( constraint.B.b0Linear, deltaLambda );
	vA[ constraint.iA ].angular.AddScaled( constraint.B.b0Angular, deltaLambda );
	// vA(bodyB) += deltaLambda * B(bodyB)
	vA[ constraint.iB ].linear.AddScaled( constraint.B.b1Linear, deltaLambda );
	vA[ constraint.iB ].angular.AddScaled( constraint.B.b1Angular, deltaLambda );

	return deltaLambda;
}

//----------------------------------------------------------------

void pxConstraintSolver_PGS::Solve( pxSolverInput& input, pxSolverOutput& output )
{
	Assert(input.isOk());

	mDeltaTime = input.deltaTime;

	const pxReal MIN_TIME_STEP = pxReal(1./300.);
	if( mDeltaTime < MIN_TIME_STEP ) {
		return;
	}

	mInvDeltaTime = 1.0f / mDeltaTime;

	const pxUInt nb = input.numBodies;	// number of bodies

	// number all bodies in the body list - set their tag values
	for( pxUInt iBody = 0; iBody < nb; iBody++ )
	{
		PX_OPTIMIZE("use separate VelocityAccumulators/SolverRigidBodies; fuse this loop with the other;");
		input.bodies[ iBody ].m_solverIndex = iBody;
	}

	const pxUInt oldNumConstraints = mConstraints.Num();

	mConstraints.Empty();

	// build contact constraints

	this->SetupContactConstraints( input );


	// number of constraints = total number of rows in jacobian
	// (number of constrained degrees of freedom or total constraint dimension)
	const pxUInt numConstraintRows = (pxUInt)mConstraints.Num();

	if( ! numConstraintRows ) {
		return;
	}

	//DBGOUT("Solving %u constraints\n",numConstraintRows);

	mStats.numConstraints = numConstraintRows;


	if( gUseWarmstarting )
	{
		if( numConstraintRows > oldNumConstraints ) {
			//const mxUInt numAddedConstraints = numConstraintRows - oldNumConstraints;
			for( pxUInt iConstraint = oldNumConstraints; iConstraint < numConstraintRows; iConstraint++ )
			{
				pxSolverConstraint & constraint = mConstraints[ iConstraint ];
				constraint.lambda = gInitialLambda;
			}
		}
	}


	PX_PROFILE("Solve constraints");


	// Fill in 'velocity accumulator': a = Sum( B * lambda ).
	pxAVector * vA = ALLOC2(pxAVector,nb);
	MemSet( vA, 0, sizeof(pxAVector)*nb);

	for( pxUInt iConstraint = 0; iConstraint < numConstraintRows; iConstraint++ )
	{
		pxSolverConstraint & constraint = mConstraints[ iConstraint ];

		// Erin Catto:
		// Stability for joints can be improved further by relaxing the lambdas. Before beginning the PGS iterations,
		// set lambda *= relaxation, where relaxation in [0,1].
		// I usually pick a relaxation value between 0.9 and 1.0. The relaxation helps to damp oscillations in the constraint forces.

		constraint.lambda *= gLambdaRelaxationFactor;

		// a +=	B * lambda
		vA[ constraint.iA ].linear	.AddScaled( constraint.B.b0Linear,	constraint.lambda );
		vA[ constraint.iA ].angular	.AddScaled( constraint.B.b0Angular,	constraint.lambda );
		vA[ constraint.iB ].linear	.AddScaled( constraint.B.b1Linear,	constraint.lambda );
		vA[ constraint.iB ].angular	.AddScaled( constraint.B.b1Angular,	constraint.lambda );
	}


	pxUInt iteration = 0;	// iteration counter

	// Kenny Erleben:
	// Early exit is only allowed after a certain minimum iteration count;
	// this is used to force the LCP solver to improve upon the solution in case warm starting is used.
	//
	PX_OPTIMIZE("this could be unrolled if minIterations was always 4");
	while( iteration < mSettings.minIterations )
	{
		for( pxUInt iConstraint = 0; iConstraint < numConstraintRows; iConstraint++ )
		{
			pxSolverConstraint & constraint = mConstraints[ iConstraint ];

			SolveSingleConstraintRow( constraint, vA );
		}

		++iteration;
	}

	// used to measure error/convergence, always positive
	pxReal deltaResidual = PX_SOLVER_INFINITY;

	while( (deltaResidual > mSettings.precision) && (iteration < mSettings.maxIterations) )
	{
		deltaResidual = 0.0f;

		for( pxUInt iConstraint = 0; iConstraint < numConstraintRows; iConstraint++ )
		{
			pxSolverConstraint & constraint = mConstraints[ iConstraint ];

			const pxReal deltaLambda = SolveSingleConstraintRow( constraint, vA );

			deltaResidual += (deltaLambda * deltaLambda);
		}

		++iteration;
	}

	mStats.numIterations = iteration;

	//DBGOUT("Solved %u constraints (%u iterations)\n",
	//	(UINT)mStats.numConstraints, (UINT)mStats.numIterations
	//	);

	// compute the velocity update, apply impulse to each body

	for( pxUInt iBody = 0; iBody < nb; iBody++ )
	{
		pxRigidBody* body = &input.bodies[ iBody ];

#if 1
		// Fix-point-iteration algorithm:
		// V2 = V1 + M^-1 * ( J^T * lambda + Fext * dt )

		// add stepsize * invM * fe to the body velocity
		const pxVec3 aF = body->GetInvMass() * body->GetTotalForce();
		const pxVec3 aT = body->GetInvInertiaWorld() * body->GetTotalTorque();

		pxVec3 newLinearVelocity = body->GetLinearVelocity() + (aF + vA[body->m_solverIndex].linear) * input.deltaTime;
		pxVec3 newAngularVelocity = body->GetAngularVelocity() + (aT + vA[body->m_solverIndex].angular) * input.deltaTime;

#else
		// this is slightly faster but inaccurate
		pxVec3 newLinearVelocity = body->GetLinearVelocity() + vA[body->m_solverIndex].linear * input.deltaTime;
		pxVec3 newAngularVelocity = body->GetAngularVelocity() + vA[body->m_solverIndex].angular * input.deltaTime;
#endif
		if( gClampVelocities ) {
			pxClampBodyVelocities( newLinearVelocity, newAngularVelocity, input.deltaTime );
		}
		if( gApplyDampingAfterVelocityUpdate ) {
			DampBodyVelocities( newLinearVelocity, newAngularVelocity, input.deltaTime );
		}

		body->SetLinearVelocity( newLinearVelocity );
		body->SetAngularVelocity( newAngularVelocity );

		// clear force accumulators
		body->ClearForces();
	}
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
