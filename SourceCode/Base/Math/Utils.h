/*
=============================================================================
	File:	Utils.h
	Desc:	Math helpers.
=============================================================================
*/

#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

mxNAMESPACE_BEGIN

//-------------------------------------------------------------------
//	Functions
//-------------------------------------------------------------------

template< typename T > FORCEINLINE T	Lerp( const T& a, const T& b, const T amount = 0.5f )	{ return a + (b - a) * amount; }

// bilinear interpolation
template< typename T > FORCEINLINE
T BiLerp(
	const T& p00, const T& p10,
	const T& p01, const T& p11,
	FLOAT fracX, FLOAT fracY
){
	return Lerp(
		Lerp( p00, p10, fracX ),
		Lerp( p01, p11, fracX ),
		fracY
	);
}

// Cubic interpolation.
// P - end points
// T - tangent directions at end points
// Alpha - distance along spline
mxSWIPED("Unreal Engine");
template< class T, class U > T CubicInterp( const T& P0, const T& T0, const T& P1, const T& T1, const U& A )
{
	FLOAT A3 = mxPow( A, 3 );
	FLOAT A2 = mxPow( A, 2 );

	return (T)(((2*A3)-(3*A2)+1) * P0) + ((A3-(2*A2)+A) * T0) + ((A3-A2) * T1) + (((-2*A3)+(3*A2)) * P1);
}

/**
 *  Performs Catmull-Rom interpolation between 4 values.
 *  @param _R Result type
 *  @param _A Value type
 *  @param _T Time type
 */
template<typename _R, typename _A, typename _T>
_R const math_catmull_rom( _A const& a0, _A const& a1, _A const& a2, _A const& a3, _T const& t )
{
	_T tt = t*t;
	_T ttt = t*tt;
	return _R( (a0 * ( -t     + tt+tt   - ttt      ) +
				a1 * ( 2.0f   - 5.0f*tt + ttt*3.0f ) +
				a2 * (  t     + 4.0f*tt - ttt*3.0f ) +
				a3 * (        - tt      + ttt      ) ) * 0.5f );
};


/**
 *  Smoothing function.
 *
 *  Uses critically damped spring for ease-in/ease-out smoothing. Stable
 *  at any time intervals. Based on GPG4 article.
 *
 *  @param from	Current value.
 *  @param to	Target value (may be moving).
 *	@param vel	Velocity (updated by the function, should be maintained between calls).
 *	@param smoothTime	Time in which the target should be reached, if travelling at max. speed.
 *	@param dt	Delta time.
 *	@return Updated value.
 */
template< typename T >
inline T smoothCD( const T& from, const T& to, T& vel, float smoothTime, float dt )
{
	float omega = 2.0f / smoothTime;
	float x = omega * dt;
	// approximate exp()
	float exp = 1.0f / (1.0f + x + 0.48f*x*x + 0.235f*x*x*x );
	T change = from - to;
	T temp = ( vel + omega * change ) * dt;
	vel = ( vel - omega * temp ) * exp;
	return to + ( change + temp ) * exp;
}

template<class T>
inline T cosineInterpolate(T const &start, T const &end, FLOAT const t)
{
    if (t <= 0)
    {
            return start;
    }
    else
    if (t >= 1)
    {
            return end;
    }

    T output;
    for (unsigned int i = 0; i < sizeof(T) / sizeof(float); ++i)
    {
            FLOAT mu2 = (1.0f - mxCos(t * M_PI)) / 2.0f;
            output[i] = start[i] * (1 - mu2) + end[i] * mu2;
    }

    return (output);
}

template<class T>
inline T linearInterpolate(T const &start, T const &end, FLOAT const t)
{
    if (t <= 0)
    {
            return start;
    }
    else
    if (t >= 1)
    {
            return end;
    }


    T output;
    for (unsigned int i = 0; i < sizeof(T) / sizeof(float); ++i)
    {
            output[i] = start[i] * (1 - t) + end[i] * t;
    }

    return (output);
}

template<class T>
inline T cubicInterpolate(T const &before, T const &start, T const &end, T const &after, FLOAT const t)
{
    if (t <= 0)
    {
            return start;
    }
    else
    if (t >= 1)
    {
            return end;
    }


    T output;
    for (unsigned int i = 0; i < sizeof(T) / sizeof(float); ++i)
    {
            FLOAT mu2 = t * t;
            FLOAT a0 = after[i] - end[i] - before[i] + start[i];
            FLOAT a1 = before[i] - start[i] - a0;
            FLOAT a2 = end[i] - before[i];
            FLOAT a3 = start[i];

            output[i] = a0 * t * mu2 + a1 * mu2 + a2 * t + a3;
    }

    return (output);
}

template<class T>
inline T bestInterpolate(T* const vec, SizeT const size, FLOAT const t)
{
    if (size >= 4)
    {
            return cubicInterpolate(vec[size - 4], vec[size - 3], vec[size - 2], vec[size - 1], t);
    } else
    if (size == 3)
    {
            if (t > 1)
            {
                    return cosineInterpolate(vec[1], vec[2], t);
            }
            else
            {
                    return cosineInterpolate(vec[0], vec[1], t);
            }
    } else
    if (size == 2)
    {
            return cosineInterpolate(vec[0], vec[1], t);
    } else
    if (size == 1)
    {
            return vec[0];
    }

    return Vec3D(0, 0, 0);
}

/*
 * A function for creating a rotation matrix that rotates a vector called
 * "from" into another vector called "to".
 * Input : from[3], to[3] which both must be *normalized* non-zero vectors
 * Output: mtx[3][3] -- a 3x3 matrix in column-major form
 * Authors: Tomas Mueller, John Hughes
 *          "Efficiently Building a Matrix to Rotate One Vector to Another"
 *          Journal of Graphics Tools, 4(4):1-4, 1999
 */
void FromToRotation( const FLOAT from[3], const FLOAT to[3], FLOAT mtx[3][3] );

//
//	RaiseBy2toN
//
FORCEINLINE FLOAT RaiseBy2toN( FLOAT x, INT n )
{
	// isolate exponent bits
	UINT exponentBits = *((INT*)&x) & 0x7F400000;
	
	// bitshift them
	exponentBits >>= n;
	
	// blank out old bits
	*((INT*)&x) &= 0x8001FFFF;

	// copy new bits in
	*((INT*)&x) &= exponentBits;

	return x;
}

//
//	ClampChar
//
INLINE signed char ClampChar( int i )
{
	if ( i < -128 ) {
		return -128;
	}
	if ( i > 127 ) {
		return 127;
	}
	return i;
}

//
//	ClampShort
//
INLINE signed short ClampShort( int i )
{
	if ( i < -32768 ) {
		return -32768;
	}
	if ( i > 0x7fff ) {
		return 0x7fff;
	}
	return i;
}

//
//	WrapInt
//
INLINE int WrapInt( int x, int hi )
{
	if( x < 0 ) {
		return ( (x % hi) + hi ) % hi;
	} else {
		return x % hi;
	}
}

INLINE int GetPow2( unsigned long u )
{
	if ( u > 1 && (u & (u - 1)) == 0 )
	{
		for ( int n = 0; u; u >>= 1, n++ )
		{
			if ( u & 1 ) {
				return n;
			}
		}
	}
	return 0;
}

template< typename TYPE >
INLINE TYPE Factorial( TYPE x )
{
	TYPE result = 1;
	while( x > 0 )
	{
		result *= x;
		x--;
	}
	return result;
}

INLINE UINT IntFactorial( UINT x )
{
	return (x < 2) ? (1) : (x * IntFactorial(x - 1));
}

INLINE
int IntLog2( int val )
{
	int answer = 0;
	while ( val >>= 1  ) {
		answer++;
	}
	return answer;
}

INLINE
int GreatestCommonDivisor( int i1, int i2 )
{
	if( i1 > i2 )
	{
		if( i2 == 0 ) {
			return (i1);
		}
		return GreatestCommonDivisor( i2, i1 % i2 );
	}
	else
	{
		if( i1 == 0 ) {
			return (i2);
		}
		return GreatestCommonDivisor( i1, i2 % i1 );
	}
}

//
//	TetrahedronDeterminant
//
INLINE
FLOAT TetrahedronDeterminant(
			const Vec3D& v0, const Vec3D& v1,
			const Vec3D& v2, const Vec3D& v3 )
{
    return Matrix4(
			Vec4D( v0, 1 ),
	    	Vec4D( v1, 1 ),
	    	Vec4D( v2, 1 ),
	    	Vec4D( v3, 1 ) ).Determinant();
}

//
//	TetrahedronVolume
//
INLINE
FLOAT TetrahedronVolume(
			const Vec3D& v0, const Vec3D& v1,
			const Vec3D& v2, const Vec3D& v3 )
{
	return mxFabs( TetrahedronDeterminant(v0,v1,v2,v3) * 0.16666666666666666667f );	// * (1.0f / 6.0f)
}

//	Math::LinearMap is a very useful function.  Given a source interval (s1, e1),
//	a target interval (s2, e2), and a source value start, returns the mapping
//	of the source value's position on the source interval to the same relative
//	position in the target interval.  For example, Math::LinearMap(-1, 1, 0, ScreenWidth, X)
//	would map from perspective space to screen space; in other words
//	LinearMap(-1, 1, 0, ScreenWidth, -1)        = 0
//	LinearMap(-1, 1, 0, ScreenWidth, -0.5)    = ScreenWidth*0.25
//	LinearMap(-1, 1, 0, ScreenWidth, 0)        = ScreenWidth*0.5
//	LinearMap(-1, 1, 0, ScreenWidth, 0.5)        = ScreenWidth*0.75
//	LinearMap(-1, 1, 0, ScreenWidth, 1)        = ScreenWidth
//
FORCEINLINE
FLOAT LinearMap( FLOAT s1, FLOAT e1, FLOAT s2, FLOAT e2, FLOAT start )
{
    return ( (start - s1) * (e2 - s2) / (e1 - s1) + s2 );
}

template< typename TYPE >
FORCEINLINE
void
TLinearMapRange(
	// source interval
	const TYPE& srcMin, const TYPE& srcMax,
	// target interval
	const TYPE& destMin, const TYPE& destMax,
	// source value
	const TYPE& srcValue,
	// unknown value
	TYPE & destValue
){
	destValue = destMin + (srcValue - srcMin) * (destMax - destMin) / (srcMax - srcMin);
}

/**
 *	Computes real quadratic roots for equ. ax^2 + bx + c = 0
 *
 *	\param		a	[in] a coeff
 *	\param		b	[in] b coeff
 *	\param		c	[in] c coeff
 *	\param		x0	[out] smallest real root
 *	\param		x1	[out] largest real root
 *	\return		number of real roots
 */
INLINE int FindRealQuadraticRoots( FLOAT a, FLOAT b, FLOAT c, FLOAT& x0, FLOAT& x1 )
{
	if( a == 0.0f )
	{
		// bx + c = 0
		if( b != 0.0f ) {
			x0 = -c / b;
			return 1;
		} else {
			return 0;
		}
	}

	// Compute D.
	FLOAT det = (b * b) - (4.0f * a * c);
	if( det < 0.0f ) {
		return 0;	// Complex roots
	}

	if( det == 0.0f )
	{
		x0 = -b / (2.0f * a);
		return 1;
	}
	else
	{
		det = mxSqrt( det );

		b = -b;
		a = 0.5f / a;

		b *= a;
		det *= a;

		x0 = b - det;
		x1 = b + det;

		if( x0 > x1) {
			swapf( x0, x1 );
		}

		return 2;
	}
}


mxSWIPED("CryEngine 3");

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

/*!
		Class TRange, can represent anything that is range between two values, mostly used for time ranges.
 */
template <class T>
class	TRange
{
public:
	T start;
	T end;

	TRange()	{ start = 0; end = 0; };
	TRange( const TRange &r ) { start = r.start; end = r.end; };
	TRange( T s,T e ) { start = s; end = e; };

	void Set( T s,T e ) { start = s; end = e; };
	void Clear() { start = 0; end = 0; };

	//! Get length of range.
	T	Length() const { return end - start; };
	//! Check if range is empty.
	bool IsEmpty()	const { return (start == 0 && end == 0); }

	//! Check if value is inside range.
	bool IsInside( T val ) { return val >= start && val <= end; };

	void ClipValue( T &val ) const
	{
		if (val < start) val = start;
		if (val > end) val = end;
	}

	//! Compare two ranges.
	bool	operator == ( const TRange &r ) const {
		return start == r.start && end == r.end;
	}
	//! Assign operator.
	TRange&	operator =( const TRange &r ) {
		start = r.start;
		end = r.end;
		return *this;
	}
	//! Interect two ranges.
	TRange	operator & ( const TRange &r ) const {
		return TRange( MAX(start,r.start),MIN(end,r.end) );
	}
	TRange&	operator &= ( const TRange &r )	{
		return (*this = (*this & r));
	}
	//! Concatent two ranges.
	TRange	operator | ( const TRange &r ) const {
		return TRange( MIN(start,r.start),MAX(end,r.end) );
	}
	TRange&	operator |= ( const TRange &r )	{
		return (*this = (*this | r));
	}
	//! Add new value to range.
	TRange	operator + ( T v ) const {
		T s = start, e = end;
		if (v < start) s = v;
		if (v > end) e = v;
		return TRange( s,e );
	}
	//! Add new value to range.
	TRange&	operator += ( T v ) const {
		if (v < start) start = v;
		if (v > end) end = v;
		return *this;
	}
};

#undef MIN
#undef MAX


//! CRange if just TRange for floats..
typedef TRange<float> FloatRange;




mxSWIPED("ICE");
// From Redon's thesis:
// [a,b] + [c,d] = [a+c, b+d]
// [a,b] - [c,d] = [a-d, b-c]
// [a,b] * [c,d] = [min(ac, ad, bc, bd), max(ac, ad, bc, bd)]
// 1 / [a,b] = [1/b, 1/a] if a>0 or b<0
// [a,b] / [c,d] = [a,b] * (1, [c,d]) if c>0 or d<0
// [a,b] <= [c,d] if b<=c

class Interval
{
public:
	INLINE				Interval()										{}
	INLINE				Interval(const Interval& it) : a(it.a), b(it.b)	{}
	INLINE				Interval(float f) : a(f), b(f)					{}
	INLINE				Interval(float _a, float _b) : a(_a), b(_b)		{}
	INLINE				~Interval()										{}

	INLINE	float		Width()								const		{ return b - a;									}
	INLINE	float		MidPoint()							const		{ return (a+b)*0.5f;							}

	// Arithmetic operators

	//! Operator for Interval Plus = Interval + Interval.
	INLINE	Interval	operator+(const Interval& it)		const		{ return Interval(a + it.a, b + it.b);			}

	//! Operator for Interval Plus = Interval + float.
	INLINE	Interval	operator+(float f)					const		{ return Interval(a + f, b + f);				}

	//! Operator for Interval Minus = Interval - Interval.
	INLINE	Interval	operator-(const Interval& it)		const		{ return Interval(a - it.b, b - it.a);			}

	//! Operator for Interval Minus = Interval - float.
	INLINE	Interval	operator-(float f)					const		{ return Interval(a - f, b - f);				}

	//! Operator for Interval Mul = Interval * Interval.
	INLINE	Interval	operator*(const Interval& it)		const
							{
								float ac = a*it.a;
								float ad = a*it.b;
								float bc = b*it.a;
								float bd = b*it.b;
#ifdef INTERVAL_USE_FCOMI
								float Min = FCMin4(ac, ad, bc, bd);
								float Max = FCMax4(ac, ad, bc, bd);
#else
								float Min = ac;
								if(ad<Min)	Min = ad;
								if(bc<Min)	Min = bc;
								if(bd<Min)	Min = bd;

								float Max = ac;
								if(ad>Max)	Max = ad;
								if(bc>Max)	Max = bc;
								if(bd>Max)	Max = bd;
#endif
								return Interval(Min, Max);
							}

	//! Operator for Interval Scale = Interval * float.
	INLINE	Interval	operator*(float s)					const
							{
								float Min = a*s;
								float Max = b*s;
								if(Min>Max)	TSwap(Min,Max);

								return Interval(Min, Max);
							}

	//! Operator for Interval Scale = float * Interval.
	INLINE friend	Interval operator*(float s, const Interval& it)
							{
								float Min = it.a*s;
								float Max = it.b*s;
								if(Min>Max)	TSwap(Min,Max);

								return Interval(Min, Max);
							}

	//! Operator for Interval Scale = float / Interval.
	INLINE	friend	Interval operator/(float s, const Interval& it)			{ return Interval(s/it.b, s/it.a);					}

	//! Operator for Point Scale = Point / float.
//		INLINE	Point			operator/(float s)					const		{ s = 1.0f / s; return Point(x * s, y * s, z * s);	}

	//! Operator for Interval Div = Interval / Interval.
	INLINE	Interval		operator/(const Interval& it)		const		{ return (*this) * (1.0f / it);						}



	//! Operator for Interval += Interval.
	INLINE	Interval&		operator+=(const Interval& it)					{ a += it.a; b += it.b;			return *this;		}

	//! Operator for Interval += float.
	INLINE	Interval&		operator+=(float f)								{ a += f; b += f;				return *this;		}

	//! Operator for Interval -= Interval.
	INLINE	Interval&		operator-=(const Interval& it)					{ a -= it.b; b -= it.a;			return *this;		}

	//! Operator for Interval -= float.
	INLINE	Interval&		operator-=(float f)								{ a -= f; b -= f;				return *this;		}

	//! Operator for Interval *= Interval.
	INLINE	Interval&		operator*=(const Interval& it)
							{
								float ac = a*it.a;
								float ad = a*it.b;
								float bc = b*it.a;
								float bd = b*it.b;
#ifdef INTERVAL_USE_FCOMI
								a = FCMin4(ac, ad, bc, bd);
								b = FCMax4(ac, ad, bc, bd);
#else
								a = ac;
								if(ad<a)	a = ad;
								if(bc<a)	a = bc;
								if(bd<a)	a = bd;

								b = ac;
								if(ad>b)	b = ad;
								if(bc>b)	b = bc;
								if(bd>b)	b = bd;
#endif
								return *this;
							}

	//! Operator for Interval /= Interval.
	INLINE	Interval&		operator /= (const Interval& it)
							{
								*this *= 1.0f / it;
								return *this;
							}

	//! Operator for "Interval A = Interval B"
	INLINE	void			operator = (const Interval& interval)
							{
								a = interval.a;
								b = interval.b;
							}

	// Logical operators

	//! Operator for "if(Interval<=Interval)"
	INLINE	bool			operator<=(const Interval& it)		const		{ return b <= it.a;	}
	//! Operator for "if(Interval<Interval)"
	INLINE	bool			operator<(const Interval& it)		const		{ return b < it.a;	}
	//! Operator for "if(Interval>=Interval)"
	INLINE	bool			operator>=(const Interval& it)		const		{ return a >= it.b;	}
	//! Operator for "if(Interval>Interval)"
	INLINE	bool			operator>(const Interval& it)		const		{ return a > it.b;	}

	float	a, b;
};

// http://www.gamasutra.com/features/19991018/Gomez_4.htm (retrieved 2007/04/27)
// [1] J. Arvo. A simple method for box-sphere intersection testing. In A. Glassner, editor, Graphics Gems, pp. 335-339, Academic Press, Boston, MA, 1990.
//
inline FASTBOOL AabbSphereIntersection( const AABB& aabb, const Sphere& sphere )
{
	FLOAT s;
	FLOAT d = 0.0f; 

	// find the square of the distance from the sphere to the box
	for( UINT i = 0; i < 3; i++ )
	{
		if( sphere.Center[i] < aabb.mPoints[0][i] )
		{
			s = sphere.Center[i] - aabb.mPoints[0][i];
			d += s * s; 
		}
		else if( sphere.Center[i] > aabb.mPoints[1][i] )
		{
			s = sphere.Center[i] - aabb.mPoints[1][i];
			d += s * s; 
		}
	}
	return ( d <= squaref( sphere.Radius ) );
}

inline bool rayAABBIntersection( const Vec3D &rayOrig, const Vec3D &rayDir, 
								const Vec3D &mins, const Vec3D &maxs )
{
	// SLAB based optimized ray/AABB intersection routine
	// Idea taken from http://ompf.org/ray/

	float l1 = (mins.x - rayOrig.x) / rayDir.x;
	float l2 = (maxs.x - rayOrig.x) / rayDir.x;
	float lmin = minf( l1, l2 );
	float lmax = maxf( l1, l2 );

	l1 = (mins.y - rayOrig.y) / rayDir.y;
	l2 = (maxs.y - rayOrig.y) / rayDir.y;
	lmin = maxf( minf( l1, l2 ), lmin );
	lmax = minf( maxf( l1, l2 ), lmax );

	l1 = (mins.z - rayOrig.z) / rayDir.z;
	l2 = (maxs.z - rayOrig.z) / rayDir.z;
	lmin = maxf( minf( l1, l2 ), lmin );
	lmax = minf( maxf( l1, l2 ), lmax );

	if( (lmax >= 0.0f) & (lmax >= lmin) )
	{
		// Consider length
		const Vec3D rayDest = rayOrig + rayDir;
		Vec3D rayMins( minf( rayDest.x, rayOrig.x), minf( rayDest.y, rayOrig.y ), minf( rayDest.z, rayOrig.z ) );
		Vec3D rayMaxs( maxf( rayDest.x, rayOrig.x), maxf( rayDest.y, rayOrig.y ), maxf( rayDest.z, rayOrig.z ) );
		return 
			(rayMins.x < maxs.x) && (rayMaxs.x > mins.x) &&
			(rayMins.y < maxs.y) && (rayMaxs.y > mins.y) &&
			(rayMins.z < maxs.z) && (rayMaxs.z > mins.z);
	}
	else
		return false;
}

FORCEINLINE
void MulTexCoords( Vec2D& uv, const Vec2D& scale )
{
	uv.x *= scale.x;
	uv.y *= scale.y;
}

// pyramid scaling function
template< typename TYPE >
TYPE PYR( const TYPE& n ) {
	return (n*(n+1)/2);
}

// computes the cubic root of the given argument
FORCEINLINE
FLOAT CubeRoot( FLOAT x ) {
	return mxExp( FLOAT(1.0/3.0) * mxLog(x) );
}

mxNAMESPACE_END

#endif /* !__MATH_UTILS_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
