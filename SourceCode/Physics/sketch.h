TODO:
friction
restitution(bouncyness)
roughness(perturb friction direction&magnitude to achieve more realism)


num. objects: %u
num. overlapping pairs (contact pairs): %u
num. constraints: %u
solver cache factor: %f
solver precision: %f (or) solver iterations: %u

threads
FPS
physics step size (Hz)

collision detection: %f msec (%u percent)
	broadphase: %f msec (%u percent)
	broadphase: %f msec (%u percent)
//------------------------------------------------------------------------------
	
void CameraUtil::convertZoomToFov( float zoom, float width, float height,
	float* hfov, float* vfov )
{
	assert( width > 1 );
	assert( height > 1 );
	assert( zoom > Float::MIN_VALUE );
	assert( hfov );
	assert( vfov );

	float frameAspect = width / height;
	*hfov = 2.f * Math::atan( frameAspect / zoom );
	*vfov = 2.f * Math::atan( 1.f / zoom );
}
//-----------------------------------------------------------------------
//	Render materials.
//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//	Geometry.
	//-----------------------------------------------------------------------

	//
//	pxShape_Box
//
MX_ALIGN_16(class) pxShape_Box : public pxShape {
public_internal:
	pxReal	length;	// x
	pxReal	height;	// y
	pxReal	width;	// z

public:
	pxShape_Box( pxReal length, pxReal width, pxReal height );
	~pxShape_Box();

	void GetWorldBounds( const pxTransform& xform, pxAABB &outBounds );
};

void pxShape_Box::GetWorldBounds( const pxTransform& xform, pxAABB &outBounds )
{
	const pxVec3& center = xform.GetOrigin();

	const pxVec3 x_row = xform.GetBasis().GetRow(0).absolute();
	const pxVec3 y_row = xform.GetBasis().GetRow(1).absolute();
	const pxVec3 z_row = xform.GetBasis().GetRow(2).absolute();

	const pxReal xrange = pxReal(0.5) * x_row.Dot( length );
	const pxReal yrange = pxReal(0.5) * x_row.Dot( height );
	const pxReal zrange = pxReal(0.5) * x_row.Dot( width );

	const pxVec3 extent( xrange, yrange, zrange );

	outBounds.mMin = center - extent;
	outBounds.mMax = center + extent;
}
	

k_biasFactor is the Baumgarte stabilization factor. Read e.g. Erin's first GDC presentation for an explanation. Usually 0.1 - 0.2
k_allowedPenetration is the described "skin" I mentioned above. Usually 0.01 - 0.02 (1-2 cm)

Actually I am looking into improving the friction model in Erin GS solver. At the moment I only solve the friction in the manifold center and apply an additional twist impulse to mimic the torsional friction. Another trick (also suggested on the forum here) is to align one friction direction with the relative velocity in the contact plane. 

So what would be a better friction model and friction solver? I see that I could solve a 3D mini LCP for the friction at the manifold center (2 tangents + 1 twist), but the question is if this improves the quality in a way that it justifies the exta computational effort. 

I implemented the G&B&F paper and I liked the friction and stacking results. From my personal experience Erin method works very nice when you warm-start the simulation. Ideally I would like to get rid of the warmstarting so could you explain a little bit more how friction could be improved and why the friction model has such a big impact on the GS convergence? In particular why is there a difference in feeding the old velocities rather then the new velocities? 

Finally, what do you mean by position projection? I am not aware of any position projection in the sense of a position constraints solver in this paper (e.g. Jacobsen). They suggest this at the end, but if IIRC this was only experiemental.

A simple way to improve the friction model is to make the friction force proportional to the previous frames computed contact normal force (as opposed to just a constant like the demo is if I remember correctly). 

This makes stacks look much more realistic as objects at the bottom have more friction and so are harder to push out of the stack as expected.

I don't have my home brew code handy, but off the top of my head: 

For contact constraints you should already be warming starting the solver with the previous frames contact normal impulse. Let's call this N for each contact. 

For each friction constraint (I use 2 tangents + 1 twist for each contact manifold like Dirk), you set the constraint min and max limits proportional to the sum of the previous frames contact normal impulses for the whole manifold. 

So 

// Compute sum of previous frames contact manifold normal impulses 
// (I use up to 4 contacts per manifold) 
Limit = N0 + N1 + N2 + N3 etc.. 

// For each friction constraint: 
FMax = Limit * k; 
FMin = -Limit * k; 

where k is a tuneable constant. 

This means the friction limit is a frame behind what it should really be, but it's very easy to implement and it doesn't slow down the solver iteration loop at all. 

You can also pseudo implement static and dynamic friction by changing the limit k depending on the velocity of the contact manifold. If Vel < threshold use k-static, else use k-dynamic. 

Let me know if this doesn't make sense and I could dig up the code... 
-Steve.


I'm a little confused by this - I'm not sure if what I do is the same as you, or if there is some new thing I should try out. 

Currently I set my friction impulse limit with something like: 

float maxTangentImpulse = frictionConstant * impulseAccumulator; 

where I use the accumulated impulse from the current contact calculation. Isn't this the same thing? Or is it important to use the previous frame's value, and if so why? Or is this something specific to a central friction model? 

And what do you mean by previous frame, do you mean previous solver iteration, or really previous frame? 

Thanks!


Afte reading your e-mail Dog - you are right - you are already doing what I mention. 

My physics code is actually based on a hybrid of Erin's previous paper (GDC 2005) and sequential impulses - so it's a slightly different implmentation, but mathematically equivalent. 

The difference is in my code the limits of the friction constraints are not updated in the inner loop iterations of the solver (only from the previous frames results) - so it's faster, but not quite as accurate. 

Sorry for the confusion. 
-Steve. 

PS. One improvement would be the addition of modifying the code to use a static and dynamic friction constant like in the G&B&F paper. As an example there's a good open source "JigLib" physics lib by Danny Chapman that has this implemented.



I see that this is a nice optimization since you basically decouple the friction from the normal impulse in the inner loop. On the other hand you don't have any friction at all in the first frame, right? Do you use any heuristic to get an initial first guess (e.g. Erin's first friction solution from his poster session at the GDC)? 

Quote:
PS. One improvement would be the addition of modifying the code to use a static and dynamic friction constant like in the G&B&F paper


What do you mean by this? Do you suggest to differentiate between static and dynamic friction? From the top of my head I remember that the coefficients are nearly the same, so I wonder if this is worth the effort. Is there any example where this improves the quality of the simulation? 



Anyway, or there any other ideas to improve friction? 



Cheers, 
-Dirk


The difference in coefficients can be small or very large. Take a look the tables on this page for some examples: 

http://www.roymech.co.uk/Useful_Tables/Tribology/co_of_frict.htm 

Lead on mild steel, for example, has a coefficient of 0.95 for both static and dynamic friction. Near the other end of the scale, zinc on cast iron has a static friction of 0.85 but a dynamic friction of only 0.21! And then of course a good friction model of lubricated surfaces would require even more complexities including the actual velocity.


If you use a procedural voronoi fracture then calculating the connectivity is trivial since the connectivity is simply the dual of the voronoi diagram, i.e., the delaunay triangulation. While this might be quite cool for performing runtime fractures one should just keep in mind to allow for artists to create prefractured geometry which may, or may not, have been created by a voronoi fracture.

The trick would be to somehow automatically calculate neighbours. I have worked on a project to do exactly this from within Houdini. We naively constructed a 'connectivity graph' by performing a 3D deluanay triangulation on the centroids of all the "chunks". We would then perform shock / impact propagation along those edges and break them when they exceeded certain 'strength' thresholds and so forth. 

Maybe this helps a bit?




This is not correct. The friction force in the formulation used by Bullet and ODE depends on the normal force and vise versa. The friction rows are solved at the end of each PGS iteration, and the most up-to-date normal force is used for the friction force magnitude. The next iteration, effect of the friction constraint propagates back to the normal force. So there is a two-way dependency between normal and friction forces. 

Bullet has several friction settings. The default setting keeps the two friction directions, orthogonal to the contact normal, constant during the iterations. This leads to errors, clamping against a 'box' pyramid. Objects that are sliding almost parallel to one of the friction directions will move towards the friction direction. To avoid this unwanted effect, another settings allows to adjust the friction direction every iteration. In that case, we cannot reliably use warm starting or 'accumulated impulse', so we make an error either in the direction or the magnitude of the friction constrains. Stewart Trinkle and Anitescu Potra is physically more realistic in this perspective.

Another drawback is that the high and low limits are updated every iteration, so it is not a usual PGS. This means, we cannot prove convergence anymore.
Still, in practice the friction model is realistic enough for movies and games.
Thanks,
Erwin



Let me put it another way: Bullet does not solve for friction and normal forces together but first assume zero (or known) friction, solve for normal forces, then plug that in to solve for friction forces. In a way, it's exactly the same as method described in http://www.cs.ubc.ca/labs/sensorimotor/ ... sigasia08/ with only one iteration. For games/movies it's enough but maybe not in robotics. One example, when simulating a robot hand grasping a cone shape, it's important to solve for BOTH normal and friction at the same time in order to have a realistic "squeezing" effect.



Sorry, I read the thread way too quick. So what we usually do for games and what is also explained in Kenny Erleben's thesis is a velocity based time stepper. The original problem (using only holonome constraints to get you the idea is):

x' = v
v' = M^-1 * ( JT * lambda + f_ext )
C(x) = 0

So you have three equations for three unknowns (x', v', lambda), but you cannot solve it. The first two and the third equations are not related. Many ways exist, but the one usually used in games uses a symplectic Euler and solves on the velocity level instead of the position level. This is already a linearization since you now look at the tangent space of the original constraint space. So basically you have:

x2 = x1 + v2 * dt
v2 = v1 + M^-1 * ( JT * lambda + f_ext ) * dt
J * v2 = 0

M * v2 - JT * lambda * dt = M * v1 + f_ext * dt
-J* v2 = 0

[ M -JT ] [ v2 ] = b
[ -J 0 ] [ lambda' ] = 0 // lambda' = lambda * dt

This system could *not* be solved with PGS (it also has zeros on its diagonals). But it has other interesting properties, e.g. it is always sparse and for hierarchical structures you can solve it in linear time. If you now plug the second into the third equation you get the so called effective mass form which I already mentioned above:

J*M-^1*JT * lambda = -J * ( v1 + M-^1 * f_ext * dt )


This can be solved with PGS now since J*M-^1*JT is PD diagonal dominant. Usually you don't have only holonome constraints, but also contact, friction, limits, and motors. From there it is pretty simple to add these. Erwin summarized the friction differences very well and I agree that this is usally sufficient for games. I cannot comment on robotics though. Still I recommed using an impulse implementation like SI since they are very efficient and easy to implement. You can also play with different friction ideas like e.g. circular clamping very, very easily. You cannot prove convergence in these cases (at least I can't), but this works really well in practise.




As for box constrained LCP, sadly I don't have any reference for that. Doom3 uses box constrained LCP to handle friction, so you can look it up in their SDK.

The idea behind it is very simple.
Box constraints allow you dynamically assign the limits for some solution scalar based on some other solution scalar.

Here's a brief explanation:

Define:
n - contact normal
u, v - orthonormal vectors that span the collision plane.
c - mu / sqrt(2)

Coulomb model:
Ff <= mu * Fn

Define:
Fn = lambda1 * n
Ff = lambda2 * u + lambda3 * v

Make the following approximation to Coulomb model.
(2) |lambdal2 + lambda3| <= mu * lambda1

This is almost identical to the "classic LCP". The only difference is that you have four vectors instead of two. I assume that you are already familiar with this technique so I hope that you know what I mean.

Rewrite (2) like this:
(3) -c * lambda1 <= lambda2 <= c * lambda1
(4) -c * lambda1 <= lambda3 <= c * lambda1

There we have our box constraint. The bounds for lambda2 and lambda3 should be dynamically adjusted by the solver, as it finds new values for lambda1.

The results that this technique gives are identical to the ones that the "classic LCP" gives when using two vectors.










The reason Gauss-Seidel has become important is for a number of reasons:

- it can easily be extended to handle box constrained LCPs
- it allows a trade-off between accuracy and performance
- an optimized implementation can be incredibly fast
- it is easy to implement
- it can be 'warm-started' to improve accuracy
- it has low memory requirements in code and data
- games don't require high accuracy (no one has died due to low physics accuracy in a game)







I looked at the Point2PointConstraint class and have a question how the constraint is satisfied ( Point2PointConstraint::SolveConstraint( dt ) ). For my understanding two steps have to be taken: 

1.) First calculate the constraint error 
This is very obvious here. Simply calculate the relative drift and relative velocity at the pivot point. 

2.) Find correction (impulse) such that constraint error is repaired 
Here I can't follow anymore. 

Questions: 

(1) 
The Jacobian for the Pt2Pt constraint is a 3 x 12 matrix and it has the following entries: 

J = ( I -R1 -I R2 ) 

I := Identity matrix (3x3) 
R1 := Cross matrix for offset vector from CM body1 to pivot point in world space (3x3) 
R2 := Cross matrix for offset vector from CM body2 to pivot point in world space (3x3) 

How does this relate to the JacobianEntry class and what is the "normal" entry in this class? 

(2) 
We need to find an impulse such that constraint error is resolved. How is this achieved using the JacobianEntry class? Derivation? I have solved this problem by finding an impulse such that relative velocity at the pivot point is resolved, but I don't take the Jacobian into account. So how can use the Jacobian to find the repairing impulse? 

(3) 
For a hinge joint the resulting impulse would be a 5 x1 vector. How do I build a linear and angular impulse from this data? 

A sketch of the basic idea would be very helpful - especially the role of the Jacobian! 

Regards, 

-Dirk

I thought about the problem and came up with the following idea: 

The velocity update for the two bodies is: 

M * du /dt = f_ext + JT * lambda 

Here u is the generalized velocity vector for body1 and body2, JT is the transposed jacobian and lambda is a vector of langrange multipliers. 

Since we use linear dynamics we can use superposition and apply the external and constraint forces subsequently. So the velocity update because of the the constraint forces becomes: 

M * du /dt = JT * lambda 

The compatibility equation (velocity constraint) for the two bodies is: 

J * u(t) = 0 

Numerical integration of the Newton-Euler equation: 

M * ( u(t+dt) - u(t) ) / dt = JT * lambda 

<=> u(t+dt) = u(t) + M^-1*JT*lambda*dt 

Semi-implicit integration, therefore 

J * u(t+dt) = 0 

Plug integrated Euler-Newton into compatibility equation: 

J * [ u(t) + M^-1*JT*lambda*dt ] = 0 
<=> J*M^-1*JT * lambda * dt + J*v(t) = 0 

This is a linear system: A * x + b = 0 

If we look now sharply we see that J*v(t) is the constraint error and lambda * dt is the wanted impulse. We define 

A := J*M^-1*JT 
x := lambda * dt 
b := J*v(t) 

=> lambda = A^-1 * constraint error (J*v(t)) 

So the wanted linear and angular impulses for body1 and body2 are simply: 

P = JT * lambda 

Is this correct? 

-Dirk

I renamed some variables to make the source a bit more clear. 
Basically the JacobianEntry calculates and stores some values to perform constraint correction: 

for two rigidbodies, A and B it precomputes 

angular Jacobian entries J for both bodies 
Minv Jt for both bodies 
Adiag, the diagonal entry in the system matrix A = J Minv Jt 

This information can be used in a Gauss-Siedel iterative LCP solver, or to correct the constraint error on a per-body pair. 

Once I got some time I will add some more details, see the renamed variables in JacobianEntry here: 
http://www.erwincoumans.com/Bullet/Bull ... tated.html


















typedef void FSharedObjCallback( pxShared* obj );

struct pxShared
{
	pxInt	numRefs;
	FSharedObjCallback * onZeroRefCount;
};


//
//	pxSolver
//
struct pxConstraintData
{
    pxVec3 r[2];  // vector from center of mass to constraint point
    pxVec3 n;  // normal
    float dv; // desired relative velocity in normal
    float u;  // friction coefficient in tangent space
    float low, high; // low and high bound of normal impulse
};

/*
// To solve collisions, we set constraint data this way:
LPSPECONTACT pContact = pWorld->AddContact(); // Create contact
// specify rigid bodies
pContact->SetRigidBody(RigidBody0, RigidBody1); 
// then enable the contact 
pContact->SetEnabled(true);
LPSPECONSTRAINT pConstraint = pContact->AddConstraint();
SPEConstraintData data;
data.r[0] = r0;
data.r[1] = r1;
data.n = n;
// note that the relative velocity is defined as v0-v1 in SPE.
data.dv = -e * ( ( Velocity0(p) - Velocity1(p) ) * n );
data.u = 0.5f * ( u0 + u1); // artificial formula
data.low = 0.0f;
data.high = SPE_POSITIVE_INFINITE;
// submit data
pConstraint->SetData(data);

*/
/*
// To solve ball joint, we set constraint data this way:
LPSPECONTACT pContact = pWorld->AddContact();
// specify rigid bodies
pContact->SetRigidBody(RigidBody0, RigidBody1); 
// then enable the contact 
pContact->SetEnabled(true);
LPSPECONSTRAINT pConstraint = pContact->AddConstraint();
SPEConstraintData data;
data.r[0] = r0;
data.r[1] = r1;
data.n = n; // p1-p0
// correct positions
data.dv = (p1-p0).Length()/StepTime;
// use infinite friction to avoid sliding in tangent space
data.u = SPE_POSITIVE_INFINITE;
// need not to clamp impulse in this case
data.low = SPE_NEGATIVE_INFINITE;
data.high = SPE_POSITIVE_INFINITE;
// submit data
pConstraint->SetData(data);
*/



	/// some cached values, which allow for fast solving of simple 3d constraints
	/// (e.g. contact constraints for toi handling)
struct hkpSimpleConstraintInfo
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT_SOLVER, hkpSimpleConstraintInfo );

	struct BodyInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT_SOLVER, hkpSimpleConstraintInfo::BodyInfo );

		hkRotation m_jacT;		// the transposed jacobians, used to get the velocities
		hkMatrix3 m_invIjac;	// used to apply forces	
		//hkMatrix3 m_inertia;
		hkReal m_invMass;
		hkReal m_mass;

		hkReal getMass() const { return m_mass; }
	};

	BodyInfo m_bodyInfo[2];

		/// first column is the normal direction, the other two columns are perpendicular
	hkRotation m_base;

	/// calculated by J^-1 * invI * J
	hkMatrix3 m_invMassMatrix;

		/// = m_invMassMatrix^-1
	hkMatrix3 m_massMatrix;

		/// store the inverted determinint of the invMassMatrix without row and column 1 and 2 
	hkReal& getMass00(){ return 	m_invMassMatrix.getColumn(0)(3); }
	hkReal getMass00() const{ return 	m_invMassMatrix.getColumn(0)(3); }

		/// store the inverted determinint of the invMassMatrix without row and column 1 and 2 
	hkReal& getInvDetM12(){ return 	m_invMassMatrix.getColumn(1)(3); }
	hkReal getInvDetM12() const { return 	m_invMassMatrix.getColumn(1)(3); }

};


class pxSolverInfo {
public:
	// Time step information.
	pxReal	mDeltaTime;
	pxReal	mInvDeltaTime;
	pxUInt	mNumSteps;
	pxReal	mInvDNumSteps;

	pxReal	mTau;
	pxReal	mDamping;	// [ 0.0f .. 1.0f ]
	pxReal	mFrictionTau;
};

// Constraint solver results.
class pxSolverResults {
public:
};


 //subtract two vectors, placing result in result
  public static final void sub( Vector3 v1, Vector3 v2, Vector3 result ) {
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
  }
  public boolean lessThan(Vector3 v) {
    return (x<v.x)&&(y<v.y)&&(z<v.z);
  }

  public boolean weaklyLessThan(Vector3 v) {
	    return (x<=v.x)&&(y<=v.y)&&(z<=v.z);
	  }

  
  public boolean isZero() {
	  return x==0&&y==0&&z==0;
  }
  
  public final boolean isWeaklyGreaterThanZero() {
	  return x>=0&&y>=0&&z>=0;
  }
 public Vector3 cutOff() {
	 double b1=Math.abs(x)<1e-6?0:x;
	 double b2=Math.abs(y)<1e-6?0:y;
	 double b3=Math.abs(z)<1e-6?0:z;
	 return new Vector3(b1,b2,b3);
 }




  Vector3 n1 = contact->norm;
  Vector3 w1 =  (contact->norm ^ (contact->point - body1->pos));
  Vector3 n2 = -contact->norm;
  Vector3 w2 = -(contact->norm ^ (contact->point - body2->pos));
  float velocityProjection =
      n1 * body1->velocity + w1 * body1->angularVelocity 
    + n2 * body2->velocity + w2 * body2->angularVelocity;



В терминах ODE, ERP можно выразить как 
J * v = – bounce * (J * v0) + depth * ERP; 
выражение – (J * v0) * bounce + depth * ERP часто обозначается как просто «c», где 
J — матрица Якоби m x 6n, 
v — вектор скоростей после решения системы 6n x 1, 
bounce — скаляр (0..1), характеризующий коэффициент отскока, 
v0 — вектор скоростей до решения системы 6n x 1, 
depth - вектор, характеризующий нарушение каждой конкретной связи m x 1, 
ERP — скаляр (0..1), 
n — количество тел, 
m — количество связей. 


Использование малого ERP (0.0f—0.7f) приводит к некоторой «мягкости» твердых тел, солвер будто не спешит их расталкивать.

Увеличение же ERP до значений порядка 0.9f—1.0f приводит к значительной нестабильности системы, возможен даже её «взрыв».





Sequential Impulses With Warmstarting - метод решения системы ограничений в динамике твердых тел, предложенный Erin'ом Catto и подробно описанный в его блоге

Основной идеей SI является итеративный подход к разрешению ограничений. То есть вместо того, чтобы решать Mixed LCP "классическим" подходом, за O(n3), SI позволяет нам решать эту систему за линейное время O(n). Разумеется, решение получается лишь приближенное.

Как неоднократно заявлял сам Эрин, его подход математически эквивалентен Projected Gauss-Seidel солверу. Фактически, методы различаются лишь подходом к их программированию - если PGS работает напрямую с матрицей Якоби, каждая строка которой ограничивает одну степень свободы, то SI оперирует со значительно более интуитивно понятными терминами, такими как "изменение импульса", "плечо силы", "относительная скорость" и другие.

Под Warmstarting'ом же понимается инициализация всех импульсов системы каждый кадр их значениями с предыдущего кадра. Опять же, это классическйи подход, основанный на том, что обычно напряжения в точках взаимодействия тел от кадра к кадру меняются незначительно, и он применяется и в Projected Gauss-Seidel солвере, и в большинстве случаев значительно повышает сходимость системы.

Также Erin Catto разработал на основе метода псевдо скоростей (Pseudo Velocities), метода коррекции положений (position correction), очень важное усовершенствование к своему солверу, называемое Split Impulses (можно перевести как "раздельные импульсы"). Основной идеей является раздельное разрешение столкновений и проникновений. То есть отдельно решаются системы уравнений для импульсов, и для так называемых псевдоимпульсов, ответственных за "расталкивание" проникших друг в друга тел. Это позволяет избавиться от артефактов сильного отскока тел при большом взаимном проникновении, а так же от проблемы неправильного распределения скоростей после удара (проблема существует в классическом методе коррекции положений Баумгарте - Baumgarte position correction)



class pxConstraint
{
public:
	struct Info {
	};

	pxConstraint();
	~pxConstraint();

	pxVec3	JoL;
	pxVec3	JoA;
	pxVec3	J1L;
	pxVec3	J1A;
	pxUInt	numRows;	// constraint dimension (in range [1..6])
};


	pxVec3		mLinearVelocity;	// center of mass velocity
	pxReal		mLinearDamping;
	pxVec3		mAngularVelocity;	// angular Velocity (radians per time unit around each axis)
	pxReal		mAngularDamping;


	struct constraint_entry {
		btScalar	m_distance1;
		btScalar	m_combinedFriction;
		btScalar	m_combinedRestitution;
	};

// position of the contact point in world space
const pxVec3 position( contact.position );

// positions of contact points with respect to body PORs
const pxVec3 rel_pos0( position - o0->GetTransform().GetOrigin() );
const pxVec3 rel_pos1( position - o1->GetTransform().GetOrigin() );

// get normal, with sign adjusted for body1/body2 polarity
const pxVec3 normal( contact.normal );

j.J0.linear = normal;
j.J0.angular = normal.Cross( rel_pos0 );
j.J1.linear = -normal;
j.J1.angular = -(normal.Cross( rel_pos1 ));

const pxReal velocityProjection = 
  j.J0.linear.Dot( o0->GetLinearVelocity() )
+ j.J0.angular.Dot( o0->GetAngularVelocity() )
+ j.J1.linear.Dot( o1->GetLinearVelocity() )
+ j.J1.angular.Dot( o1->GetAngularVelocity() );

const pxReal dv;  // desired relative velocity in normal direction
dv = 0.0f;//-e * dot(n,(v0-v1))

const pxReal combinedFriction;
combinedFriction = 0.5f;//combinedFriction = 0.5f * ( u0 + u1);

// set LCP limits for normal
data.low = 0.0f;
data.high = SPE_POSITIVE_INFINITE;



Solving a 1D constraint in rigid body dynamics is straigt forward. Actually this is what you do all the time in the iterative solvers. You have

w = a * x - b >= 0 and x >= 0 and w * x = 0

In rigid body dynamics the effective mass matrix (J*W*JT)^-1 is PD (and SPD in the presence of friction). This means that a > 0 (Practical hint: You should assert this in your code!)

So you have x = b / a (with a > 0 as explained above)

Case 1 (b >= 0): In this case x >= 0 and therefore w = 0
Case 2 (b < 0): In this case x < 0 and therefore we clamp x = 0 -> w > 0


You have examples directly in the beginning of the web book I mentioned. Also Box2D has the block-solver in the contact code and revolute joint with limits. Note that Erin shifts variables to account for the clamping against the accumulated impulse which is a small additional gotcha.


HTH,



Assume you have a distance constraint between two particles. Also assume that we only can push the particles apart.

C(x1, x2) = |x2 - x1| - L = 0
dC/dt = (x2 - x1) / |x2 - x1| * (v2 - v1) 

Define n = (x2 - x1) / |x2 - x1| -> dC/dt = n * (v2 - v1) 


We apply impulses P1 and P2:

(1) v1' = v1 + P1 / m1
(2) v2' = v2 + P2 / m2

such that the post-velocities v1' and v2' should satisfy the velocity constraint dC/dt:

(3) n * (v2' - v1') = 0

We also know the impulse direction (results from the d'Alembert Principle):

(4) P1 = -lambda * n 
(5) P2 = lambda * n

5 equations for 5 unknowns(v1', v2', P1, P2, lambda). Now we solve for lambda using a more compact vector notation:

v' = v + P / M
J * v' = 0
P = JT * lambda

<=> J * M^-1 * JT * lambda = -J * v 

The above equation finds an impulse lambda such that the relative velocites along n are cancelled out. Now let's form an LCP from this:

v_rel' = J * M^-1 * JT * lambda + J * v >= 0 _and_
lambda >= 0 _and_
v_rel' * lambda = 0

If the particles are drifting apart lambda becomes less than zero and we ignore the impulse. If the particles are approaching each other we apply a positive impulse lambda such that v_rel' becomes zero.

v_rel' is the relative velocity after we apply the impulse. J * v is the relative velocity before we apply the impulse. If the relative velocity is greater zero lambda becomes zero. Otherwise we compute a lambda such that the relative velocity after applying the impulse becomes zero.

Note that LCPs and sequential impulses are mathemetical equivalent. It is basically just the physical interpretation what a projected Gauss-Seidel does when solving a MLCP. Namely it applies "impulses sequentially".


HTH,
-Dirk




Right, you can discard w.

When you have found lambda (z in your notation) you have to do the following to apply the impulse:

P = JT * lambda

Where P is a column vector of linear and angular impulses defined like this: P = ( P1 L1 P2 L2 )_t
JT is the transposed Jacobion matrix 
lambda is the vector of impulse strenths.

Note that P1 and P2 are applied in the mass center and we therefore need to find the angular impulses L1 and L2 explicitly.

The gradients in the Jacobian define the direction of the impulse. So you basically have to scale the gradients with lambda. I suggest to write this down for the 2D example above using two simultaneous contacts.

C = ( x2 + r2 - x1 - r1 ) * n
dC/dt = ( v2 + cross( omega2, r2 ) - v1 - cross( omega1, r1 ) ) * n
J = [ -n -cross( r1, n ) n cross( r2, n ) ]


Here a final gotcha of a real world problem: 
For contacts as you might remember we clip against the accumulated- and not the incremental impulse. You need to shift variables to account for this. Erin Catto solves this quite elegantly in Box2D.

In other words we don't require z > 0, but sum( z_i ) > 0

It might be interesting to look at this.


HTH,
-Dirk




You could formulate the problem maybe differently. Basically you have (assuming an impulse solver):

M * dv = f_ext * dt + JT * lambda // Newton's law in impulse form
J * ( v + dv ) = 0 // Velocity constraint

Rearranging terms:

M * dv - JT * lambda = f_ext * dt
-J * dv = J * v

This can be written in matrix form:

( M -JT ) = ( f_ext * dt )
( -J 0 ) = ( J * v )


This is basically the same system, but not in J*W*JT form. Actually this formulation is usually sparse as opposed to the effiective mass formulation. Look at Baraff's linear time paper. I think it also discusses PD'ness of the problem.


-Dirk








Given that you already have the contact normal, and applied normal impulse, this simplified friction model (in Bullet) works pretty well in practice:
Code:
{
   btVector3 frictionDir1 = vel - cp.m_normalWorldOnB * rel_vel;
   btScalar lat_rel_vel = frictionDir1.length2();
   if (lat_rel_vel > SIMD_EPSILON)//0.0f)
   {
      //use projected normal direction as first friction direction and cross product of normal and frictionDir1 as second friction direction
      frictionDir1 /= btSqrt(lat_rel_vel);
      addFrictionConstraint(frictionDir1,solverBodyIdA,solverBodyIdB,frictionIndex,cp,rel_pos1,rel_pos2,colObj0,colObj1, relaxation);
      btVector3 frictionDir2 = frictionDir1.cross(cp.m_normalWorldOnB);
      frictionDir2.normalize();
      addFrictionConstraint(frictionDir2,solverBodyIdA,solverBodyIdB,frictionIndex,cp,rel_pos1,rel_pos2,colObj0,colObj1, relaxation);
   } else
   {
      //calculate two orthogonal vectors to normal direction
      //re-calculate friction direction every frame, todo: check if this is really needed
      btVector3   frictionDir1,frictionDir2;
      btPlaneSpace1(cp.m_normalWorldOnB,frictionDir1,frictionDir2);
      addFrictionConstraint(frictionDir1,solverBodyIdA,solverBodyIdB,frictionIndex,cp,rel_pos1,rel_pos2,colObj0,colObj1, relaxation);
      addFrictionConstraint(frictionDir2,solverBodyIdA,solverBodyIdB,frictionIndex,cp,rel_pos1,rel_pos2,colObj0,colObj1, relaxation);
   }

}
//Optimization for the iterative solver: avoid calculating constant terms involving inertia, normal, relative position
void internalApplyImpulse(const btVector3& linearComponent, const btVector3& angularComponent,btScalar impulseMagnitude)
{
   if (m_inverseMass != btScalar(0.))
   {
      m_linearVelocity += linearComponent*impulseMagnitude;
      if (m_angularFactor)
      {
         m_angularVelocity += angularComponent*impulseMagnitude*m_angularFactor;
      }
   }
}
//the regular applyImpulse
void applyCentralImpulse(const btVector3& impulse)
{
   m_linearVelocity += impulse * m_inverseMass;
}
   
void applyTorqueImpulse(const btVector3& torque)
{
      m_angularVelocity += m_invInertiaTensorWorld * torque;
}
void applyImpulse(const btVector3& impulse, const btVector3& rel_pos) 
{
   if (m_inverseMass != btScalar(0.))
   {
      applyCentralImpulse(impulse);
      if (m_angularFactor)
      {
         applyTorqueImpulse(rel_pos.cross(impulse)*m_angularFactor);
      }
   }
}


And so I tried it!

It basically tried the SOR-LCP-1 () function in Kenny Erleben's PhD thesis with w=1 which would (I guess) turn it into the Gauss-Seidel LCP solver. A fixed iteration limit of 10 (no stopping criteria checking) and no contact caching as that would need some additional serious work and I usually break contacts a lot in my engine (lots of random motion). the lo chosen was 0.0 always, and hi chosen was the impulse every contact would get in the absence of all other contacts. I also tried a hi of 100.0 for all as well(very large impulse for my current objects.) For warmstarting, I tried half of each predicted impulse in the absence of all other contacts. Contacts moving away from surface always got a warmstart of 0.0.

works fine for like 7 boxes but as soon as 17 or 18 boxes get stacked, the physics engine starts to slow down. However when I tried a naive 'split-impulse' like approach, I wouldn't get that kind of slowdown until like 60 to 70 boxes all crammed on top of each other.

Also I saw objects sinking into each other two (stack of 3).. is this an indicator of the need of a "First Order World" error correction as stated in Kenny Erleben's thesis?


Ok I hope someone reads the following:

Wow, I arrived at a nice stable looking solution. Sorry to say it wasn't PGS, maybe if I had forced my lazy self a bit more to read the literature on the subject I would've definitely made a full fledged solver and system for it  (with all the shiny stuff like contact graphs and contact caching.)

Anyway, I basically went through all the impulses in the scene and applied them separately, and did this for all the contacts 10 times (ran through all contacts, then ran again and so on...), applied friction along the way and also since I'm too lazy to make a full fledged shock prorogation routine with a full fledged system constructing a full fledged contact graph, I just did the prorogation on regular objects facing immovable objects (mass = infinite). Basically just ran through contacts facing mass=inf as the last step to make sure NO object is going to move towards immovable mass before position update.

Also I used a LinearVelocityMagnitude + AngularVelocityMagnitude < threshold to stop objects from rattling around on the floor. They settled quite nicely.

Overall, I loved it, even though stacks at levels above the object on the floor, would try to sink it to the object at ground level (in other words 'failed'). Obviously REAL shock prorogation would avoid that, even though it would introduce other problems such as TOO-stable stacks.

YAY, I'm happy now!

And thanks for keeping this forum alive, I don't know of any other place that people are either willing to help or have the knowledge to.



If you want a simple method, that works quite well and looks really good (but is not neccesarily physically accurate) I would recommend that you check out the paper by Thomas Jakobsen referenced above. It utilizes Verlet integration (much simpler than it sounds) and simple relaxation methods for constraints - you basically run the sim, and then fix up positions each frame to satisfy the constraints and eliminate penetrating collisions. Works great for chaining rigid bodies together and doing cloth simulations, and is much simpler (and more efficient) than full blown solving systems of differential equations solution.


Ok, i'll try to briefly explain, what is going on with all that LCPs. I'll focus on constraint-based approach, which means that all joints defined via constraint function C(x1, R1, x2, R2), and if constraint is satisfied, constraint function is equal to zero: C = 0. To be more precise, we will look onto velocity constraints, dC/dt.

So, by the chain rule:
dC/dt = dC/dx * dx/dt = dC/dx * v;
Differentiation of one vector function (C) by another vector function (x) yelds a Jacobian matrix, so we can rewrite:
dC/dt = [J] * v;
where vector v is vector of velocities on current time step:
vt = vt-1 + dt * [M]-1 * F(e) + dt * [M]-1 * Fc;
vector vt-1 - velocities from previous timestep, [M] is mass-matrix (i will further denote [M]-1 as [W]), F(e) - vector of external forces, Fc - vector of constraint-forces(forces, needed to keep velocities in that constraint legal -- this vector we should find).

To find Fc we need to define it somehow. To do this, we need to look at rows of [J]. They contain gradients of scalar components of constrint function, Ci. The gradient is direction of the highest rate of increasing of scalar function, and as constraint is satisfied only if C = 0 -- gradient is totally illegal direction of movement. So, we need constraint force to act in this direction (as we want it to constraint only illegal movement and not to influence legal), so, the definition of constraint force is:
Fc = [J]T * lambda;
lambda is vector of Lagrange multipliers, signed scalars -- coefficients in the linear combination of those gradients.

Now we need to reformulate system via substituting Fc and into vt and then into [J] * v to find lambda:
dC/dt = [J] * ( vt-1 + dt * [W] * F(e) + dt * [W] * ([J]T * lambda) );
to find lambda, we should regroup elements, like this:
[J] * [W] * [J]T * (dt * lambda) = dC/dt - [J] * (vt-1 + dt * [W] * F(e))
We get system of equations similar to A*x = b, and in fact, we're solving not for just lambdas, but for (dt * lambda) which can be treated as constraint-impulses.

Together with complementarity constraints (like lambdai > 0 or something), it forms linear complementarity problem (LCP), and you need to solve this in order to satisfy all the constraints in system.

One more thing: as i said before, satisfying velocity constraints leads to positional drift, so let me briefly describe Baumgarte stabilization. I have kept dC/dt in the right hand side of equations to introduce Baumgarte stabilization. I prefer this way of thinking about Baumgarte: we have constraint error, so dC/dt is not zero anymore, and we actually need to substitute constraint error dC over time stepsize dt to fix that error. However, this approach have several drawbacks, and the most significant is that it affects the momentum of a body. Little example: when two bodies collide with zero restitution coefficient, one on top of another -- they should immediately go to rest. But if we have constraint violation (interpenetration, due to discretization), we will recieve penalty force and top body will pop a bit. This can be solved via pseudo-velocities post-stabilization.

Let me show you, how does it work, on example fo quite simple joint - ball-socket.
To force bodies share one point (which ball-socket actually is), we must define positional constraint function at first:
C = x1 + [R]1 * p1 - (x2 + [R]2 * p2) = 0;
Let me explain, what it all actually means. We have here coordinates of point on body1, given via its position(x1), orientation([R]1) and vector(p1) from center of masses to given point in localspace(when body is at initial state). Same is for body2, and to satisfy ball-socket constraint, these shared points must be equal.

Now we need to derive dC/dt:
dC/dt = v1 - [R]1 * p1 x w1 - (v2 - [R]2 * p2 x w2);
where w is angular velocity (and 'x' is for cross-product).
With derivation of x1,2 shouldn't be any trouble, whereas [R]1 * p1 derivation is a bit trickier (since [R] is rotation matrix, it is orthogonal => [R]T[R] = [I]):
d[R]/dt * p = d[R]/dt * [R]T * [R] * p;
But d[R]/dt * [R]T is angular velocity tensor, so we can replace it with cross product with angular velocity:
d[R]/dt * [R]T * [R] * p = w x [R] * p;
We need to retrieve a Jacobian matrix in form [J]*v, so w x [R] * p = -[R] * p x w, and cross product can be converted to matrix multiplication:
-[R] * p x w = crossprod_matrix(-[R] * p) * w;

So, we finally can write our dC/dt equation:
dC/dt = [I] * v1 + crossprod_matrix(-[R]1 * p1) * w1 + (-[I]) * v2 + crossprod_matrix([R]2 * p2) * w2;
So, Jacobian matrices for this constraint is:
[I] - identity matrix,
crossprod_matrix(-[R]1 * p1),
-[I],
crossprod_matrix([R]2 * p2)
and therefore, we can write and solve LCP (actually, for ball-socket joint we don't need complementarity constraints, so it reduces to simple system of linear equations).


On average impulses: 
In order to avoid the awful tilting which happens through sequential applying impulses (s. Kenny) I use an averaged impulse in the collision phase. Instead of using the arithmetic mean of the contact points I use a weighted sum scaled by the relative normal velocity - only considering points with penetrating velocity. John Schultz at GameDev.net uses something similar. Instead of using the relative normal velocity he scales by the normal impulse (length) at each contact point. 
Both schemes give good result for well defined stacking, but have problems for twisted and offset stacks. Also note that you totally miss the friction when the upper box only rotates around the contact normal. The averaging will cancel out the "Torsion Momentum". This is work in progress here...



Bullet implements a Projected Gauss-Seidel (PGS) in a special form called Sequential Impulses (SI). This solves indeed an MLCP. All methods you describe use Lagrange multipliers (either implicit or explicit). E.g. in Jan Benders Methods the K-Matrix is nothing else but the effective mass. You can easily show the collision matrix K and effective mass Me = (J*M^-1*JT)^-1 with C = (x2 + r2 - x1 - r1) * n and J = ( -n -r1 x n n r2 x n ) are equivalent.

Bender basically solves:

// First sweep
J*M^-1*J * lambda = -C/dt

// Second sweep
J*M^-1*JT * lambda = -J*v


Bullet solves (ignoring split impulses or similar projection methods which are also supported)

// Combined sweep using Baumgarte stabilization
J*M^-1*JT * lambda = -beta * C / dt - J * v


The videos look great. Well done! What do you use for collision detection?


HTH,
-Dirk











Should be: const og3DVector vError = -( v1 - v2 ) - fBias * m_vPosDiff / dt


I am 100% sure. Whether the points are coincident or not doesn't matter. You only remove the relative velocity at the anchor points. 

The formula goes like this:
J * M^-1 JT * lambda = -0.1 * C / dt - J * v
P = JT * lambda


Check Erin's code and presentations if you don't trust me. Without position correction you iteratively seek impulses that remove the relative velocities at the anchor points. Since you suffer from drift you put back some stabilization term (called Baumgarte stabilization). Using the Baumgarte term you can bring the anchors to what ever distance you like. 

Here is some demo I programmed lately if you want to check that this works in praxis (iterate through the demos a bit until you come to the joints): 


Just to back up Dirk:

Here's the term plus baumgarte from my engine for collisions:

( 1 + getRestitution() ) * ( deltaVel.dot3(getNormal()) ) + depth / deltaTime * baumgarteUnitFraction

A small tip Numsgil. This way you will gain energy for coefficient of restitution e = 1 since you basically additionally add the bias. You can look at the demo with the 5 balls in my demo above. I use somethink like this:

plVector3 v_rel;
plComputeRelativeVelocityAt( v_rel, pBody1, r1, pBody2, r2 );
plReal vn_rel = plVec3Dot( v_rel, n );

const plReal kMinVelocity = 1.0f;
if ( vn_rel < -kMinVelocity )
{
plReal ve = restitution * vn_rel;
if ( ve < out.mBias )
{
out.mBias = ve;
}
}


Cheers,
-Dirk




Пеппер про novodex где-то был... но там особо инфы немного (там про итерационный PGS и что-то еще)... Понятие Width_skin кроме physX, есть и в Havok, у Erin Catto в box2d, у меня тоже есть). У Catto это выглядит так: Vbias = b/dt * max(0, D - Dslop), Vbias-скорость выталкивания,  D -глубина контакта, Dslop - это и есть Width_skin. Т.е. Width_skin  - это кламп пенетрации.  Разумеется, это мое понимание)).





I'm building a physics engine and i had some sort of "pseudo-verlet" thing going and i wanted to upgrade it to "real" verlet. So i found an article and set to work.

Edit: I've fixed it and it works like a charm, but i have two questions about the following code:
Is the impulse application code correct, and if not what should it be?
How do i change the position property so that setting it preserves the current velocity of the body?

Here is the code:
public Vector2 Position
{
    get { return _position; }
    set { _position = value;}
}
public void Update(float timestepLength)
{
    if (!this._isStatic)
    {
        Vector2 velocity =  Vector2.Subtract(_position, _lastPosition);
        Vector2 velocityChange = Vector2.Subtract(velocity, Vector2.Subtract(_lastPosition, _twoStepsAgoPosition));
        Vector2 nextPos = _position + (_position - _lastPosition) + _acceleration * (timestepLength * timestepLength);
        _twoStepsAgoPosition = _lastPosition;
        _lastPosition = _position;
        _position = nextPos;
        _acceleration = Vector2.Multiply(velocityChange, timestepLength);
    }
}

public void ApplyForce(Vector2 force)
{
    if (!this._isStatic)
        _lastPosition -= force;
}
public void ApplyImpulse(Vector2 impulse)
{
    if (!this._isStatic)
        _acceleration +=-1 * impulse;
}

as a reference to others... the verlet paper you are probably refering to is this: advanced character physics made by the team who created hitman and was one of the first to have ragdoll based physics

anyhow... the original code they used was: 
 void ParticleSystem::Verlet() {
       for(int i=0; i<NUM_PARTICLES; i++) {
             Vector3& x = m_x[i];
             Vector3 temp = x;
             Vector3& oldx = m_oldx[i];
             Vector3& a = m_a[i];
             x += x-oldx+a*fTimeStep*fTimeStep;
             oldx = temp;
       }
 }

and you are right that your code does a similar thing.

The one thing which always blew up my simulations was the use of a too big timestep. Also, with this verlet integration, make sure that the timestep you use is constant throughout the game. (e.g 30 frames/sec for instance (so timestep is 1/30)) and doesn't fluctuate. If it does you should use the time corrected verlet integration which accounts for this

EDIT:

answer to question2: to move your position (without changing velocity/acceleration) just update the position to the new position, and then as an extra step add the delta of this movment(so newPosition-oldPosition) to the oldposs, so this gets updated accordingly. 

answer to question1: Impulse is the force applied to an object over a period of time. So your solution is not correct. The impulse would be that over X timesteps(or frames), you call your applyForce function with a fixed force.



		const pxVec3 x_axis = mAxis.GetRow(0);
		const pxVec3 y_axis = mAxis.GetRow(1);
		const pxVec3 z_axis = mAxis.GetRow(2);
		
		
inline int 
minimum(int a, int b)
{
  int mask = (a - b) >> 31;
  return ((a & m) | (b & ~m));
}

private bool TestSphereAABB(CD_AxisAlignedBoundingBox aABB, ref Contact contact)
        {
            // Find point (p) on AABB closest to Sphere centre
            Vector3 p = aABB.ClosestPtPointAABB(Centre);
 
            // Sphere and AABB intersect if the (squared) distance from sphere centre to point (p)
            // is less than the (squared) sphere radius
            Vector3 v = p - Centre;
 
            if (Vector3.Dot(v, v) <= Radius * Radius)
            {
                contact.location_Distance = aABB.DistPointAABB(Centre);
 
                // Calculate normal using sphere centre a closest point on AABB
                contact.normal = Centre - p;
 
                if (contact.normal != Vector3.SetZero)
                {
                    contact.normal.Normalize();
                }
 
                contact.radius_Projection = Radius;
 
                return true;
            }
 
            // No intersection
            return false;
        }

		       private bool TestSphereAABB(CD_AxisAlignedBoundingBox aABB, ref Contact contact)
        {
            // Find point (p) on AABB closest to Sphere centre
            Vector3 p = aABB.ClosestPtPointAABB(Centre);

            // Sphere and AABB intersect if the (squared) distance from sphere centre to point (p)
            // is less than the (squared) sphere radius
            Vector3 v = p - Centre;
            
            float distance_Squared = Vector3.Dot(v, v);

            if (distance_Squared <= Radius * Radius)
            {
                if (v != Vector3.SetZero)
                {
                    v.Normalize();
                }

                contact.normal = v;
                contact.penetration = aABB.DistPointAABB(Centre) - Radius;

                return true;
            }

            // No intersection
            return false;
        }
		
		
private bool TestSphereOBB(CD_OrientedBoundingBox oBB, ref Contact contact)
        {
            // Find point (p) on OBB closest to Sphere centre
            Vector3 p = oBB.ClosestPtPointOBB(Centre);

            // Sphere and OBB intersect if the (squared) distance from sphere centre to point (p)
            // is less than the (squared) sphere radius
            Vector3 v = p - Centre;

            // Vector3.Dot(v, v) gives the square distance to point p
            float distance_Squared = Vector3.Dot(v, v);
            
            if (distance_Squared <= Radius * Radius)
            {
                if (v != Vector3.SetZero)
                {
                    v.Normalize();
                }

                contact.normal = v;
                contact.penetration = (float)Math.Sqrt(oBB.SqDistancePoint(Centre)) - Radius;

                return true;
            }

            // No intersection
            return false;
        }
		
		If I have the usual sliding response collision resolution of:

position_Projected += contact.normal * contact.penetration;

then everything works fine. However, in both cases the collision normal is in the opposite direction to what it should be.

How can I achieve a correct penetration depth with the normal reversed (-v)?

Also, am I making things too complicated when trying to work out the penetration depth?

Thank you.

EDIT: Solved it, I just need to use:

Radius - (float)Math.Sqrt(distance_Squared);

for all cases. Please excuse my brain.


Vector ClosestPointOnAABB(Vector Point, CAABBox xBox)
{
    Vector xClosestPoint;
    xClosestPoint.x = (Point.x < xBox.Min.x)? xBox.Min.x : (Point.x > xBox.Max.x)? xBox.Max.x : Point.x;
    xClosestPoint.y = (Point.y < xBox.Min.y)? xBox.Min.y : (Point.y > xBox.Max.y)? xBox.Max.y : Point.y;
    xClosestPoint.z = (Point.z < xBox.Min.z)? xBox.Min.z : (Point.z > xBox.Max.z)? xBox.Max.z : Point.z;

    return xClosestPoint;
}

bool Intersect(CSphere xSphere, CAABBox xBox, Vector& xNcoll, float& fDcoll)
{
    if(xBox.Contains(xSphere.Centre))
    {
         // Do special code.
         // here, for now don't do a collision, until the centre is
         // outside teh box
         fDcoll = 0.0f;
         xNcoll = Vector(0, 0, 0);
         return true;
    }

    // get closest point on box from sphere centre
    Vector xClosest = ClosestPointOnAABB(xSphere.Centre, xBox);
    
    // find the separation
    Vector xDiff = xSphere.Centre - xClosest;

    // check if points are far enough
    float fDistSquared = xDiff.GetLengthSquared();

    if (fDistSquared > xSphere.RadiusSquared())
    {
        return false;
    }

    float fDist = sqrt(fDistSquared);

    // collision depth
    fDcoll = xSphere.GetRadius() - fDist;
   
    // normal of collision (going towards the sphere centre)
    xNcoll = xDiff  / fDist;

    return true;    
}

// the 'footprint' of the box when projected along an axis.
void oboxInterval(const Vector& axis, const OBox& box, float& min, float& max)
{
    // extent of the box on axis  
    float hx = fabs(dotProduct(box.dir[0], axis)) * box.halfsize[0];
    float hy = fabs(dotProduct(box.dir[1], axis)) * box.halfsize[1];
    float hz = fabs(dotProduct(box.dir[2], axis)) * box.halfsize[2];
    float r  = hx + hy + hz;

    // position of the box on axis
    float p  = dotProduct(box.centre, axis);
  
    // interval of the box on axis.
    min = p - r;
    max = p + r;
}

bool intervalsIntersect(const Vector& axis, const OBox& a, const OBox& b)
{
    // this may not be necessary.
    // but if the axis of projection is degenerate (length < 1.0E-4f), ignore test.
#define CHECK_DEGENERATE_AXES (1)
#ifdef CHECK_DEGENERATE_AXES
    float a2 = dotProduct(axis, axis);
    if(a2 < 1.0E-8f) return true;
#endif

    float amin, amax;
    float bmin, bmax;

    // calculate footprint of the objects along the axis
    oboxInterval(axis, a, amin, amax);
    oboxInterval(axis, b, bmin, bmax);

    // check if projected intervals overlap.
    return(amin <= bmax) && (amax >= bmin);
}

bool OBoxOBoxIntersect(const OBox& a, const OBox& b)
{
    for(int i = 0; i < 3; i ++)
    {
        // test projections along boxes major axes.
        if(!intervalsIntersect(a.dir[i], a, b)) return false;
        if(!intervalsIntersect(b.dir[i], a, b)) return false;

        for(int j = 0; j < 3; j ++)
        {
            // polygon edge.
            Vector cross_axis = crossProduct(a.dir[i], b.dir[j]);

            // test projections along the cross_axis.
            if(!intervalsIntersect(cross_axis, a, b)) return false;
        }
    }
    return true;
}

// by Olivier Renault.
// it doesn't check if the sphere centre is inside teh box.
pxVec3 ClosestPointOnAABB(const pxVec3& Point, const pxAABB& xBox)
{
    pxVec3 xClosestPoint;
    xClosestPoint.setX( (Point.getX() < xBox.mMin.getX())? xBox.mMin.getX() : (Point.getX() > xBox.mMax.getX())? xBox.mMax.getX() : Point.getX() );
    xClosestPoint.setY( (Point.getY() < xBox.mMin.getY())? xBox.mMin.getY() : (Point.getY() > xBox.mMax.getY())? xBox.mMax.getY() : Point.getY() );
    xClosestPoint.setZ( (Point.getZ() < xBox.mMin.getZ())? xBox.mMin.getZ() : (Point.getZ() > xBox.mMax.getZ())? xBox.mMax.getZ() : Point.getZ() );
    return xClosestPoint;
}
bool Intersect(const pxSphere& xSphere, const pxAABB& xBox, pxVec3& xNcoll, float& fDcoll)
{
    if(xBox.Contains(xSphere.GetOrigin()))
    {
         // Do special code.
         // here, for now don't do a collision, until the centre is
         // outside teh box
         fDcoll = 0.0f;
         xNcoll = pxVec3(0, 1, 0);
         return true;
    }

    // get closest point on box from sphere centre
    pxVec3 xClosest = ClosestPointOnAABB(xSphere.GetOrigin(), xBox);

    // find the separation
    pxVec3 xDiff = xSphere.GetOrigin() - xClosest;

    // check if points are far enough
	float fDistSquared = xDiff.length2();

    if (fDistSquared > squaref(xSphere.GetRadius()))
    {
        return false;
    }

	float fDist = mxSqrt(fDistSquared);

    // collision depth
    fDcoll = xSphere.GetRadius() - fDist;
   
    // normal of collision (going towards the sphere centre)
    xNcoll = xDiff  / fDist;

    return true;    
}

void Quat_To_Matrix1(Quaternion& quat,Matrix& matrix)
{
  const float x = quat[0];
  const float y = quat[1];
  const float z = quat[2];
  const float w = quat[3];

  matrix[0][0] = w*w + x*x - y*y - z*z;
  matrix[0][1] = 2*x*y + 2*w*z;
  matrix[0][2] = 2*x*z - 2*w*y;
  matrix[0][3] = 0.0f;

  matrix[1][0] = 2*x*y-2*w*z;
  matrix[1][1] = w*w - x*x + y*y - z*z;
  matrix[1][2] = 2*y*z + 2*w*x;
  matrix[1][3] = 0.0f;

  matrix[2][0] = 2*x*z + 2*w*y;
  matrix[2][1] = 2*y*z - 2*w*x;
  matrix[2][2] = w*w - x*x - y*y + z*z;
  matrix[2][3] = 0.0f;

  matrix[3][0] = 0.0f;
  matrix[3][1] = 0.0f;
  matrix[3][2] = 0.0f;
  matrix[3][3] = w*w + x*x + y*y + z*z;
}


void Quat_To_Matrix2(Quaternion& quat,Matrix& matrix)
{
  const float x = quat[0];
  const float y = quat[1];
  const float z = quat[2];
  const float w = quat[3];

  float _w = w*w;
  float _x = x*x;
  float _y = y*y;
  float _z = z*z;
  
  matrix[0][0] = _w + _x - _y - _z;
  matrix[0][1] = 2*x*y + 2*w*z;
  matrix[0][2] = 2*x*z - 2*w*y;
  matrix[0][3] = 0.0f;

  matrix[1][0] = 2*x*y-2*w*z;
  matrix[1][1] = _w - _x + _y - _z;
  matrix[1][2] = 2*y*z + 2*w*x;
  matrix[1][3] = 0.0f;

  matrix[2][0] = 2*x*z + 2*w*y;
  matrix[2][1] = 2*y*z - 2*w*x;
  matrix[2][2] = _w - _x - _y + _z;
  matrix[2][3] = 0.0f;

  matrix[3][0] = 0.0f;
  matrix[3][1] = 0.0f;
  matrix[3][2] = 0.0f;
  matrix[3][3] = _w + _x + _y + _z;
}






	// compute M^1 - create (6*nb,6*nb) inverse mass matrix `invM'
	// and fill it with mass parameters

	pxMatX	invMassMatrix(
		(pxReal*)StackAlloc(Square(numBodiesBy6) * sizeof(pxReal)),
		numBodiesBy6, numBodiesBy6
	);
	MemSet( invMassMatrix.GetPtr(), 0, invMassMatrix.GetDataSize() );

	for( UINT iBody = 0; iBody < input.numBodies; iBody++ )
	{
		const pxRigidBody & body = input.bodies[ iBody ];
		pxReal ** p = (pxReal**) &( invMassMatrix( iBody * 6, iBody * 6 ) );

		p[0][0] = p[1][1] = p[2][2] = body.GetInvMass();

		p = (pxReal**) &( invMassMatrix( iBody * 6 + 3, iBody * 6 + 3 ) );
		p[0][0] = body.GetInvInertiaWorld()[0][0];
		p[0][1] = body.GetInvInertiaWorld()[0][1];
		p[0][2] = body.GetInvInertiaWorld()[0][2];
		p[1][0] = body.GetInvInertiaWorld()[1][0];
		p[1][1] = body.GetInvInertiaWorld()[1][1];
		p[1][2] = body.GetInvInertiaWorld()[1][2];
		p[2][0] = body.GetInvInertiaWorld()[2][0];
		p[2][1] = body.GetInvInertiaWorld()[2][1];
		p[2][2] = body.GetInvInertiaWorld()[2][2];
	}

	for(int i=0; i<invMassMatrix.GetNumRows();i++){
		for(int j=0; j<invMassMatrix.GetNumColumns();j++){
			//printf("A[%i][%i]=%f");
			printf("%.3f ",invMassMatrix(i,j));
		}
		printf("\n");
	}
	
	for( UINT i = 0; i < n6; i++ )
	{
//		invMassMatrix[row][i] * J[
	}

	for( UINT row = 0; row < n6; row++ )
	{
		for( UINT column = 0; column < numConstraintRows; column++ )
		{
			B[row][column] = ;
		}
	}


	for( UINT iBody = 0; iBody < input.numBodies; iBody++ )
	{
		const pxRigidBody * body = input.bodies[ iBody ];

		const UINT i6 = iBody * 6;

		B.GetPtr();
	}
	B.Multiply( invMassMatrix, J );
	
	
	

// returns the number of contact points
typedef pxUInt FCollide( pxCollideable* objA, pxCollideable* objB, pxCollisionResult &result );

template< class TCollider, const bool REVERSE >
uint TCollide(pxCollideable* objA, pxCollideable* objB, pxCollisionResult &result)
{
	if( !REVERSE ) {
		return TCollider::Collide( objA, objB, result );
	} else {
		return TCollider::Collide( objB, objA, result );
	}
}



template< typename KEY, typename VALUE >
class THashTable
{
private:
public:
	typedef THashTable< KEY, VALUE > THIS_TYPE;

	THashTable( UINT tableSize = 512 );
	~THashTable();

private:	PREVENT_COPY( THIS_TYPE );
};
		if( heads[hash] ) {
			heads[hash].next = newPair;
		}
		hashTable[index] = newPair;
		
		
template< typename KEY, typename VALUE >
class TPairHash
{
private:
	struct Node {
		KEY		key;
		VALUE	value;
		Node *	prev;
		Node *	next;
	};

	Node *	heads;
	UINT	mask;	// mask = tableSize - 1, tableSize is a power of two
	UINT	numEntries;

private:
	Node* NewNode()
	{
		++numEntries;
	}
public:
	typedef TPairHash< KEY, VALUE > THIS_TYPE;

	TPairHash( UINT tableSize = 512 )
	{
		Assert(IsPowerOfTwo(tableSize));
		heads = (Node*) pxNew( tableSize * sizeof(Node) );
		MemSet( heads, 0, tableSize * sizeof(Node) );
		mask = tableSize - 1;
		numEntries = 0;
	}
	~TPairHash()
	{
		pxFree(heads);
		heads = nil;
		mask = 0;
		numEntries = 0;
	}
	FORCEINLINE UINT CalcHash( const KEY* keyA, const KEY* keyB ) const
	{
		return (GetHash( keyA, keyB ) & mask);
	}
	VALUE* AddPair( const KEY* keyA, const KEY* keyB )
	{
		UINT hash = CalcHash( keyA, keyB );
		return _AddPair( keyA, keyB, hash );
	}
	VALUE* AddUniquePair( const KEY* keyA, const KEY* keyB )
	{
		UINT hash = CalcHash( keyA, keyB );
		VALUE* pair = _FindPair( keyA, keyB, hash );
		if( pair ) {
			return pair;
		}
		UINT hash = GetHash( keyA, keyB ) & mask;
		return _AddPair( keyA, keyB, hash );
	}
	VALUE* FindPair( const KEY* keyA, const KEY* keyB )
	{
		UINT hash = CalcHash( keyA, keyB );
		return _FindPair( keyA, keyB, hash );
	}
	bool RemovePair( const KEY* keyA, const KEY* keyB )
	{
	}

private:
	VALUE* _AddPair( const KEY* keyA, const KEY* keyB, UINT hash )
	{
	}
	VALUE* _FindPair( const KEY* keyA, const KEY* keyB, UINT hash )
	{
		while( heads[hash] )
		{
		}
	}

private:	PREVENT_COPY( THIS_TYPE );
};

//GetHash(oA,oB)
//Equals(oA,oB)
//TPairHash< pxCollideable*, pxCollisionAgent* >;



<member name="F:BEPUphysics.Constraints.Constraint.softness">
            <summary>
            Softness of the joint; higher values reduce the rigidity and stiffness of the joint.  0 is perfectly rigid.
            If instability problems are observed, increasing this value can help.
            </summary>
        </member>
        <member name="F:BEPUphysics.Constraints.Constraint.biasFactor">
            <summary>
            Position correction factor, values range from 0-1.  A value of zero and other extremely low values cause noticable position drift, while values close to 1 can cause 'explosions.'
            Recommended value: .2f.
            </summary>
        </member>
<member name="M:BEPUphysics.Constraints.Constraint.calculateJacobians">
            <summary>
            Calculates the jacobians used in other calculations.
            Called by preStep(float dt).
            </summary>
        </member>
        <member name="M:BEPUphysics.Constraints.Constraint.calculateMassMatrix">
            <summary>
            Calculates the effective mass matrices, J * (M^-1) * J^T, for the constraints involved.
            Called by preStep(float dt).
            </summary>
        </member>
        <member name="M:BEPUphysics.Constraints.Constraint.calculateError">
            <summary>
            Calculates the position errors.
            Called by preStep(float dt).
            </summary>
        </member>
        <member name="M:BEPUphysics.Constraints.Constraint.collectInvolvedEntities">
            <summary>
            Adds entities associated with the solver item to the involved entities list.
            Ensure that sortInvolvedEntities() is called at the end of the function.
            This allows the non-batched multithreading system to lock properly.
            </summary>
        </member>
        <member name="M:BEPUphysics.Constraints.Constraint.checkForEarlyOutIterations(Microsoft.Xna.Framework.Vector3)">
            <summary>
            Manages the early-out functionality of the solver; if too many solvers in sequence are tiny, it ceases to attempt to solve them.
            Called from applyImpulse(float dt).
		 <param name="impulse">This frame's calculated impulse.</param>
        </member>
        <member name="M:BEPUphysics.Constraints.Constraint.calculateBias(System.Single)">
            <summary>
            Calculates the position correction impulse.
            Called by preStep(float dt).
            </summary>
            <param name="dt">Time in seconds since the last frame.</param>
			
"M:BEPUphysics.Constraints.DistanceRangeConstraint.#ctor(BEPUphysics.Entities.Entity,BEPUphysics.Entities.Entity,Microsoft.Xna.Framework.Vector3,Microsoft.Xna.Framework.Vector3,System.Single,System.Single,System.Single,System.Single,System.Single)">
            <summary>
            Constructs a spherical joint.
            </summary>
            <param name="connectionA">First body connected to the spring.
            Set to nil to connect with the space.</param>
            <param name="connectionB">Second body connected to the spring.
            Set to nil to connect with the space.</param>
            <param name="anchorA">Connection to the spring from the first connected body in world space.</param>
            <param name="anchorB"> Connection to the spring from the second connected body in world space.</param>
            <param name="minimumLength">Minimum distance maintained between the anchors.</param>
            <param name="maximumLength">Maximum distance allowed between the anchors.</param>
            <param name="softness">Softness of the joint; higher values reduce the rigidity and stiffness of the joint.  0 is perfectly rigid.
            If instability problems are observed, increasing this value can help.</param>
            <param name="biasFactor">Position correction factor; values must be from 0-1.  0 and very low values result in position drift, while values close to 1 can cause 'explosions.'
            Suggested value is .2f.</param>
            <param name="forceMax">Maximum corrective force allowed before breaking.</param>
        </member>
		