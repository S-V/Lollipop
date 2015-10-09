
#if MX_MATH_USE_ID_APPROX

//
// The magical inverse square root function from Quake, id Software.
// Note: uses a Newton-Raphson iteration with a clever first approximation ( magical constant ).
// Note: don't use if very high accuracy is needed.
// Note: this routine can actually be slower on modern CPUs because
// it operates on the integer representation of floating point numbers
// and data transfers between integer and floating-point units are relatively slow.
//
FORCEINLINE FLOAT Math::RSqrt( FLOAT x )
{
	Assert( x > 0.0f );
	DWORD i;
	FLOAT y, r;
	const FLOAT threeHalfs = 1.5f;

	y = x * 0.5f;

#if 0 // Original code from Id Software

	i = *reinterpret_cast<DWORD *>( &x );
	i = 0x5f3759df - ( i >> 1 );
	r = *reinterpret_cast<FLOAT *>( &i );

#else
	// This version is said to be slightly better.
	const DWORD magicConstant = 0x5f375a86;	// The new constant proposed by Chris Lomont.
	i = *(DWORD* )( &x );
	i = magicConstant - ( i >> 1 );
	r = *(FLOAT* )( &i );
#endif

	r = r * ( threeHalfs - r * r * y );		// 1st iteration ( repeating increases accuracy ).
	//r = r * ( threeHalfs - r * r * y );	// 2nd iteration - this can be removed.
	return r;
}

#else

FORCEINLINE FLOAT Math::RSqrt( FLOAT x )
{
	Assert( x > 0.0f );
	return InvSqrt( x );
}

#endif // MX_MATH_USE_ID_APPROX

FORCEINLINE FLOAT Math::InvSqrt16( FLOAT x )
{
	Assert( x > 0.0f );

#if MX_MATH_USE_ID_APPROX
	DWORD a = ((union _flint*)(&x))->i;
	union _flint seed;

	Assert( initialized );

	DOUBLE y = x * 0.5f;
	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
	DOUBLE r = seed.f;
	r = r * ( 1.5f - r * r * y );
	return (FLOAT) r;
#else
	return InvSqrt( x );
#endif
}

FORCEINLINE FLOAT Math::InvSqrt( FLOAT x )
{
	Assert( x > 0.0f );
#if MX_USE_ASM

	return mxInvSqrt( x );

#elif MX_MATH_USE_ID_APPROX
	// Use a look-up table and floating-point tricks.
	// This can actually be slower on modern CPUs because of LHS issues.
	DWORD a = ((union _flint*)(&x))->i;
	union _flint seed;

	Assert( initialized );

	DOUBLE y = x * 0.5f;
	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
	DOUBLE r = seed.f;
	r = r * ( 1.5f - r * r * y );
	r = r * ( 1.5f - r * r * y );
	return (FLOAT) r;
#else
	return 1.0f / ::sqrtf( x );
#endif
}

FORCEINLINE DOUBLE Math::InvSqrt64( FLOAT x )
{
	Assert( x > 0.0f );
#if MX_MATH_USE_ID_APPROX
	DWORD a = ((union _flint*)(&x))->i;
	union _flint seed;

	Assert( initialized );

	DOUBLE y = x * 0.5f;
	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
	DOUBLE r = seed.f;
	r = r * ( 1.5f - r * r * y );
	r = r * ( 1.5f - r * r * y );
	r = r * ( 1.5f - r * r * y );
	return r;
#else
	return 1.0f / ::sqrt( x );
#endif
}

FORCEINLINE FLOAT Math::Sqrt16( FLOAT x )
{
	Assert( x >= 0.0f );
#if MX_MATH_USE_ID_APPROX
	return x * InvSqrt16( x );
#else
	return ::sqrtf( x );
#endif
}

FORCEINLINE FLOAT Math::Sqrt( FLOAT x )
{
	Assert( x >= 0.0f );
#if MX_MATH_USE_ID_APPROX
	return x * InvSqrt( x );
#elif 0
	MX_SWIPED("Bullet");
    FLOAT y, z, tempf;
    UINT *tfptr = ((UINT*)&tempf) + 1;
	tempf = x;
	*tfptr = (0xBFCDD90A - *tfptr)>>1; // estimate of 1/sqrt(x)
	y =  tempf;
	z =  x*FLOAT(0.5);
	y = (FLOAT(1.5)*y)-(y*y)*(y*z);
	y = (FLOAT(1.5)*y)-(y*y)*(y*z);
	y = (FLOAT(1.5)*y)-(y*y)*(y*z);
	y = (FLOAT(1.5)*y)-(y*y)*(y*z);
	y = (FLOAT(1.5)*y)-(y*y)*(y*z);
	return y*x;
#else
	return ::sqrtf( x );
#endif
}

FORCEINLINE DOUBLE Math::Sqrt64( FLOAT x )
{
	Assert( x >= 0.0f );
#if MX_MATH_USE_ID_APPROX
	return x * InvSqrt64( x );
#else
	return ::sqrt( x );
#endif
}

FORCEINLINE FLOAT Math::Sin( FLOAT a )
{
	return ::sinf( a );
}

FORCEINLINE FLOAT Math::Sin16( FLOAT a )
{
#if MX_MATH_USE_ID_APPROX
	FLOAT s;

	if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
		a -= ::floorf( a / TWO_PI ) * TWO_PI;
	}
#if 1
	if ( a < MX_PI ) {
		if ( a > MX_HALF_PI ) {
			a = MX_PI - a;
		}
	} else {
		if ( a > MX_PI + MX_HALF_PI ) {
			a = a - TWO_PI;
		} else {
			a = MX_PI - a;
		}
	}
#else
	a = MX_PI - a;
	if ( ::fabs( a ) >= MX_HALF_PI ) {
		a = ( ( a < 0.0f ) ? -MX_PI : MX_PI ) - a;
	}
#endif
	s = a * a;
	return a * ( ( ( ( ( -2.39e-08f * s + 2.7526e-06f ) * s - 1.98409e-04f ) * s + 8.3333315e-03f ) * s - 1.666666664e-01f ) * s + 1.0f );
#else
	return ::sinf( a );
#endif
}

FORCEINLINE DOUBLE Math::Sin64( FLOAT a ) {
	return ::sin( a );
}

FORCEINLINE FLOAT Math::Cos( FLOAT a ) {
	return ::cosf( a );
}

FORCEINLINE FLOAT Math::Cos16( FLOAT a )
{
#if MX_MATH_USE_ID_APPROX
	FLOAT s, d;

	if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
		a -= ::floorf( a / TWO_PI ) * TWO_PI;
	}
#if 1
	if ( a < MX_PI ) {
		if ( a > MX_HALF_PI ) {
			a = MX_PI - a;
			d = -1.0f;
		} else {
			d = 1.0f;
		}
	} else {
		if ( a > MX_PI + MX_HALF_PI ) {
			a = a - TWO_PI;
			d = 1.0f;
		} else {
			a = MX_PI - a;
			d = -1.0f;
		}
	}
#else
	a = MX_PI - a;
	if ( ::fabs( a ) >= MX_HALF_PI ) {
		a = ( ( a < 0.0f ) ? -MX_PI : MX_PI ) - a;
		d = 1.0f;
	} else {
		d = -1.0f;
	}
#endif
	s = a * a;
	return d * ( ( ( ( ( -2.605e-07f * s + 2.47609e-05f ) * s - 1.3888397e-03f ) * s + 4.16666418e-02f ) * s - 4.999999963e-01f ) * s + 1.0f );
#else
	return ::cosf( a );
#endif
}

FORCEINLINE DOUBLE Math::Cos64( FLOAT a ) {
	return ::cos( a );
}

FORCEINLINE void Math::SinCos( FLOAT a, FLOAT &s, FLOAT &c ) {
#if MX_USE_ASM
	asmSinCos( a, s, c );
#else
	s = ::sinf( a );
	c = ::cosf( a );
#endif
}

FORCEINLINE void Math::SinCos16( FLOAT a, FLOAT &s, FLOAT &c )
{
#if MX_MATH_USE_ID_APPROX
	FLOAT t, d;

	if ( ( a < 0.0f ) || ( a >= Math::TWO_PI ) ) {
		a -= ::floorf( a / Math::TWO_PI ) * Math::TWO_PI;
	}
#if 1
	if ( a < MX_PI ) {
		if ( a > MX_HALF_PI ) {
			a = MX_PI - a;
			d = -1.0f;
		} else {
			d = 1.0f;
		}
	} else {
		if ( a > MX_PI + MX_HALF_PI ) {
			a = a - TWO_PI;
			d = 1.0f;
		} else {
			a = MX_PI - a;
			d = -1.0f;
		}
	}
#else
	a = MX_PI - a;
	if ( fabs( a ) >= MX_HALF_PI ) {
		a = ( ( a < 0.0f ) ? -MX_PI : MX_PI ) - a;
		d = 1.0f;
	} else {
		d = -1.0f;
	}
#endif
	t = a * a;
	s = a * ( ( ( ( ( -2.39e-08f * t + 2.7526e-06f ) * t - 1.98409e-04f ) * t + 8.3333315e-03f ) * t - 1.666666664e-01f ) * t + 1.0f );
	c = d * ( ( ( ( ( -2.605e-07f * t + 2.47609e-05f ) * t - 1.3888397e-03f ) * t + 4.16666418e-02f ) * t - 4.999999963e-01f ) * t + 1.0f );
#else
	SinCos( a, s, c );
#endif
}

FORCEINLINE void Math::SinCos64( FLOAT a, DOUBLE &s, DOUBLE &c ) {
#if MX_USE_ASM
	asmSinCos64( a, s, c );
#else
	s = sin( a );
	c = cos( a );
#endif
}

FORCEINLINE FLOAT Math::Tan( FLOAT a ) {
	return ::tanf( a );
}

FORCEINLINE FLOAT Math::Tan16( FLOAT a )
{
#if MX_MATH_USE_ID_APPROX
	FLOAT s;
	bool reciprocal;

	if ( ( a < 0.0f ) || ( a >= MX_PI ) ) {
		a -= ::floorf( a / MX_PI ) * MX_PI;
	}
#if 1
	if ( a < MX_HALF_PI ) {
		if ( a > ONEFOURTH_PI ) {
			a = MX_HALF_PI - a;
			reciprocal = true;
		} else {
			reciprocal = false;
		}
	} else {
		if ( a > MX_HALF_PI + ONEFOURTH_PI ) {
			a = a - MX_PI;
			reciprocal = false;
		} else {
			a = MX_HALF_PI - a;
			reciprocal = true;
		}
	}
#else
	a = MX_HALF_PI - a;
	if ( fabs( a ) >= ONEFOURTH_PI ) {
		a = ( ( a < 0.0f ) ? -MX_HALF_PI : MX_HALF_PI ) - a;
		reciprocal = false;
	} else {
		reciprocal = true;
	}
#endif
	s = a * a;
	s = a * ( ( ( ( ( ( 9.5168091e-03f * s + 2.900525e-03f ) * s + 2.45650893e-02f ) * s + 5.33740603e-02f ) * s + 1.333923995e-01f ) * s + 3.333314036e-01f ) * s + 1.0f );
	if ( reciprocal ) {
		return 1.0f / s;
	} else {
		return s;
	}
#else
	return ::tanf( a );
#endif
}

FORCEINLINE DOUBLE Math::Tan64( FLOAT a ) {
	return ::tan( a );
}

FORCEINLINE FLOAT Math::ASin( FLOAT a )
{
	if ( a <= -1.0f ) {
		return -MX_HALF_PI;
	}
	if ( a >= 1.0f ) {
		return MX_HALF_PI;
	}
	return ::asinf( a );
}

FORCEINLINE FLOAT Math::ASin16( FLOAT a )
{
#if MX_MATH_USE_ID_APPROX
	if ( FLOATSIGNBITSET( a ) ) {
		if ( a <= -1.0f ) {
			return -MX_HALF_PI;
		}
		a = ::fabs( a );
		return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a ) - MX_HALF_PI;
	} else {
		if ( a >= 1.0f ) {
			return MX_HALF_PI;
		}
		return MX_HALF_PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
	}
#else
	return ASin( a );
#endif
}

FORCEINLINE DOUBLE Math::ASin64( FLOAT a ) {
	if ( a <= -1.0f ) {
		return -MX_HALF_PI;
	}
	if ( a >= 1.0f ) {
		return MX_HALF_PI;
	}
	return ::asin( a );
}

FORCEINLINE FLOAT Math::ACos( FLOAT a ) {
	if ( a <= -1.0f ) {
		return MX_PI;
	}
	if ( a >= 1.0f ) {
		return 0.0f;
	}
	return ::acosf( a );
}

FORCEINLINE FLOAT Math::ACos16( FLOAT a )
{
#if MX_MATH_USE_ID_APPROX
	if ( FLOATSIGNBITSET( a ) ) {
		if ( a <= -1.0f ) {
			return MX_PI;
		}
		a = fabs( a );
		return MX_PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
	} else {
		if ( a >= 1.0f ) {
			return 0.0f;
		}
		return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
	}
#else
	return ACos( a );
#endif
}

FORCEINLINE DOUBLE Math::ACos64( FLOAT a ) {
	if ( a <= -1.0f ) {
		return MX_PI;
	}
	if ( a >= 1.0f ) {
		return 0.0f;
	}
	return ::acos( a );
}

FORCEINLINE FLOAT Math::ATan( FLOAT a ) {
	return ::atanf( a );
}

FORCEINLINE FLOAT Math::ATan16( FLOAT a )
{
#if MX_MATH_USE_ID_APPROX
	FLOAT s;

	if ( ::fabs( a ) > 1.0f ) {
		a = 1.0f / a;
		s = a * a;
		s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
				* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
		if ( FLOATSIGNBITSET( a ) ) {
			return s - MX_HALF_PI;
		} else {
			return s + MX_HALF_PI;
		}
	} else {
		s = a * a;
		return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
	}
#else
	return ::atanf( a );
#endif
}

FORCEINLINE DOUBLE Math::ATan64( FLOAT a ) {
	return ::atan( a );
}

FORCEINLINE FLOAT Math::ATan( FLOAT y, FLOAT x ) {
	return ::atan2f( y, x );
}

FORCEINLINE FLOAT Math::ATan16( FLOAT y, FLOAT x )
{
	FLOAT a, s;

	if ( Fabs( y ) > Fabs( x ) ) {
		a = x / y;
		s = a * a;
		s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
				* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
		if ( FLOATSIGNBITSET( a ) ) {
			return s - MX_HALF_PI;
		} else {
			return s + MX_HALF_PI;
		}
	} else {
		a = y / x;
		s = a * a;
		return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
	}
}

FORCEINLINE DOUBLE Math::ATan64( FLOAT y, FLOAT x ) {
	return ::atan2( y, x );
}

FORCEINLINE FLOAT Math::Pow( FLOAT x, FLOAT y ) {
	return ::powf( x, y );
}

FORCEINLINE FLOAT Math::Pow16( FLOAT x, FLOAT y ) {
	return Exp16( y * Log16( x ) );
}

FORCEINLINE DOUBLE Math::Pow64( FLOAT x, FLOAT y ) {
	return ::pow( x, y );
}

FORCEINLINE FLOAT Math::Exp( FLOAT f ) {
	return ::expf( f );
}

FORCEINLINE FLOAT Math::Exp16( FLOAT f ) {
	INT i, s, e, m, exponent;
	FLOAT x, x2, y, p, q;

	x = f * 1.44269504088896340f;		// multiply with ( 1 / log( 2 ) )
#if 1
	i = *reinterpret_cast<INT *>(&x);
	s = ( i >> IEEE_FLT_SIGN_BIT );
	e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
	i = ( ( m >> ( IEEE_FLT_MANTISSA_BITS - e ) ) & ~( e >> 31 ) ) ^ s;
#else
	i = (INT) x;
	if ( x < 0.0f ) {
		i--;
	}
#endif
	exponent = ( i + IEEE_FLT_EXPONENT_BIAS ) << IEEE_FLT_MANTISSA_BITS;
	y = *reinterpret_cast<FLOAT *>(&exponent);
	x -= (FLOAT) i;
	if ( x >= 0.5f ) {
		x -= 0.5f;
		y *= 1.4142135623730950488f;	// multiply with sqrt( 2 )
	}
	x2 = x * x;
	p = x * ( 7.2152891511493f + x2 * 0.0576900723731f );
	q = 20.8189237930062f + x2;
	x = y * ( q + p ) / ( q - p );
	return x;
}

FORCEINLINE DOUBLE Math::Exp64( FLOAT f ) {
	return ::exp( f );
}

FORCEINLINE FLOAT Math::Log( FLOAT f ) {
	return ::logf( f );
}

FORCEINLINE FLOAT Math::Log16( FLOAT f ) {
	INT i, exponent;
	FLOAT y, y2;

	i = *reinterpret_cast<INT *>(&f);
	exponent = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	i -= ( exponent + 1 ) << IEEE_FLT_MANTISSA_BITS;	// get value in the range [.5, 1>
	y = *reinterpret_cast<FLOAT *>(&i);
	y *= 1.4142135623730950488f;						// multiply with sqrt( 2 )
	y = ( y - 1.0f ) / ( y + 1.0f );
	y2 = y * y;
	y = y * ( 2.000000000046727f + y2 * ( 0.666666635059382f + y2 * ( 0.4000059794795f + y2 * ( 0.28525381498f + y2 * 0.2376245609f ) ) ) );
	y += 0.693147180559945f * ( (FLOAT)exponent + 0.5f );
	return y;
}

FORCEINLINE DOUBLE Math::Log64( FLOAT f ) {
	return ::log( f );
}

FORCEINLINE FLOAT Math::Log10( FLOAT f ) {
	return ::log10( f );
}

FORCEINLINE INT Math::IPow( INT x, INT y ) {
	INT r; for( r = x; y > 1; y-- ) { r *= x; } return r;
}

FORCEINLINE INT Math::ILog2( FLOAT f ) {
	return ( ( (*reinterpret_cast<INT *>(&f)) >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
}

FORCEINLINE INT Math::ILog2( INT i ) {
	return ILog2( (FLOAT)i );
}

FORCEINLINE INT Math::BitsForFloat( FLOAT f ) {
	return ILog2( f ) + 1;
}

FORCEINLINE INT Math::BitsForInteger( INT i ) {
	return ILog2( (FLOAT)i ) + 1;
}

FORCEINLINE INT Math::MaskForFloatSign( FLOAT f ) {
	return ( (*reinterpret_cast<INT *>(&f)) >> 31 );
}

FORCEINLINE INT Math::MaskForIntegerSign( INT i ) {
//	return ( i >> (sizeof(INT) * CHAR_BIT - 1) );
	return ( i >> 31 );
}

FORCEINLINE FLOAT Math::Fabs( FLOAT f )
{
#if MX_MATH_USE_ID_APPROX
	INT tmp = *reinterpret_cast<INT *>( &f );
	tmp &= 0x7FFFFFFF;	// Clears the sign bit.
	return *reinterpret_cast<FLOAT *>( &tmp );
#else
	return ::fabs( f );
#endif
}

FORCEINLINE FLOAT Math::Floor( FLOAT f ) {
	return ::floorf( f );
}

FORCEINLINE FLOAT Math::Ceil( FLOAT f ) {
	return ::ceilf( f );
}

FORCEINLINE INT Math::Rint( FLOAT f ) {
	return FtoiFast( ::floorf( f + 0.5f ) );
}

FORCEINLINE FLOAT Math::IntToF( INT i ) {
	mxOPTIMIZE("the conversion is slow, LHS issues, not recommended");
	return (FLOAT) i;
}

FORCEINLINE INT Math::Ftoi( FLOAT f ) {
#if MX_USE_ASM
	return asmRound( f );
#elif MX_MATH_USE_ID_APPROX
	_flint n;
	_flint bias;

	bias.i = ( f > 0.0f )
			?
			(23 + 127) << 23
			:
			((23 + 127) << 23) + (1 << 22);

	n.f = f;

	n.f += bias.f;
	n.i -= bias.i;

	return n.i;
#elif 1

	UINT e = (0x7F + 31) - ((* (UINT*) &f & 0x7F800000) >> 23);
	UINT m = 0x80000000 | (* (UINT*) &f << 8);
	return INT((m >> e) & -(e < 32));

#else
	// If a converted result is larger than the maximum signed doubleword integer the result is undefined.
	return (INT) f;	// float to int conversion using truncation
#endif
}

FORCEINLINE INT Math::FtoiFast( FLOAT f )
{
#if MX_USE_ASM
	return asmRound( f );
#elif MX_MATH_USE_ID_APPROX						// round chop (C/C++ standard)
	INT i, s, e, m, shift;
	i = *reinterpret_cast<INT *>(&f);
	s = i >> IEEE_FLT_SIGN_BIT;
	e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
	shift = e - IEEE_FLT_MANTISSA_BITS;
	return ( ( ( ( m >> -shift ) | ( m << shift ) ) & ~( e >> 31 ) ) ^ s ) - s;
#else
	// If a converted result is larger than the maximum signed doubleword integer the result is undefined.
	return (INT) f;
#endif
}

FORCEINLINE FLOAT Math::UByteToF( UBYTE c )
{
	return Uint8ToFloat32[ c ];
}

FORCEINLINE UBYTE Math::FtoB( FLOAT f )
{
#if MX_USE_ASM
	return asmF32toU8( f );
#elif MX_MATH_USE_ID_APPROX
	INT i;
	i = (INT) f;
	// clamp to range [0..255]
	if ( i < 0 ) {
		return 0;
	} else if ( i > 255 ) {
		return 255;
	}
	return i;
#elif 0
	// f is assumed to be in range [0..256)
  FLOAT x = f + 256.0f;
  return ((*(INT*)&x) & 0x7FFFFF) >> 15;
#else
	return (UBYTE)f;
#endif
}

FORCEINLINE FLOAT Math::Reciprocal( FLOAT x )
{
	//Assert( x != 0.0f );
#if MX_USE_ASM
	return mxReciprocal( x );
#elif MX_MATH_USE_ID_APPROX
	INT i = 2 * F32_VALUE_1 - SIR( x );
	FLOAT r = FR( i );
	return r * ( 2.0f - x * r );
#else
	return ( 1.0f / x );
#endif
}

FORCEINLINE FLOAT Math::FMod( FLOAT f, FLOAT m )
{
	return ::fmod( f, m );
}

FORCEINLINE CHAR Math::ClampChar( INT i ) {
	if ( i < -128 ) {
		return -128;
	}
	if ( i > 127 ) {
		return 127;
	}
	return i;
}

FORCEINLINE INT16 Math::ClampShort( INT i ) {
	if ( i < -32768 ) {
		return -32768;
	}
	if ( i > 32767 ) {
		return 32767;
	}
	return i;
}

FORCEINLINE INT Math::ClampInt( INT min, INT max, INT value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}

FORCEINLINE FLOAT Math::ClampFloat( FLOAT min, FLOAT max, FLOAT value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}

FORCEINLINE FLOAT Math::AngleNormalize360( FLOAT angle ) {
	if ( ( angle >= 360.0f ) || ( angle < 0.0f ) ) {
		angle -= ::floor( angle / 360.0f ) * 360.0f;
	}
	return angle;
}

FORCEINLINE FLOAT Math::AngleNormalize180( FLOAT angle ) {
	angle = AngleNormalize360( angle );
	if ( angle > 180.0f ) {
		angle -= 360.0f;
	}
	return angle;
}

FORCEINLINE FLOAT Math::AngleDelta( FLOAT angle1, FLOAT angle2 ) {
	return AngleNormalize180( angle1 - angle2 );
}

FORCEINLINE INT Math::FloatHash( const FLOAT *array, const INT numFloats ) {
	INT i, hash = 0;
	const INT *ptr;

	ptr = reinterpret_cast<const INT *>( array );
	for ( i = 0; i < numFloats; i++ ) {
		hash ^= ptr[i];
	}
	return hash;
}

INLINE DOUBLE Math::PerlinNoise( INT x, INT y, INT random )
{
	DOUBLE result;

	INT n = x + (y * 57) + (random * 131);
	n = (n << 13) ^ n;

	result = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) * 0.000000000931322574615478515625f);

	return result;
}
