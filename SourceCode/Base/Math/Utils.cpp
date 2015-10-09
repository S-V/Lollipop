/*
=============================================================================
	File:	Utils.cpp
	Desc:	Math helpers.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN


void FromToRotation( const FLOAT from[3], const FLOAT to[3], FLOAT mtx[3][3] )
{

#define EPSILON 0.000001f

#define CROSS(dest, v1, v2){                 \
          dest[0] = v1[1] * v2[2] - v1[2] * v2[1]; \
          dest[1] = v1[2] * v2[0] - v1[0] * v2[2]; \
          dest[2] = v1[0] * v2[1] - v1[1] * v2[0];}

#define DOT(v1, v2) (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2])

#define SUB(dest, v1, v2){       \
          dest[0] = v1[0] - v2[0]; \
          dest[1] = v1[1] - v2[1]; \
          dest[2] = v1[2] - v2[2];}

	FLOAT c[3];
	FLOAT e, h, f;

	CROSS(c, from, to);
	e = DOT(from, to);
	f = (e < 0.0f)? -e:e;
	if (f > 1.0f - EPSILON)     /* "from" and "to"-vector almost parallel */
	{
		FLOAT u[3], v[3]; /* temporary storage vectors */
		FLOAT x[3];       /* vector most nearly orthogonal to "from" */
		FLOAT c1, c2, c3; /* coefficients for later use */
		int i, j;

		x[0] = (from[0] > 0.0f)? from[0] : -from[0];
		x[1] = (from[1] > 0.0f)? from[1] : -from[1];
		x[2] = (from[2] > 0.0f)? from[2] : -from[2];

		if (x[0] < x[1])
		{
			if (x[0] < x[2])
			{
				x[0] = 1.0f; x[1] = x[2] = 0.0f;
			}
			else
			{
				x[2] = 1.0f; x[0] = x[1] = 0.0f;
			}
		}
		else
		{
			if (x[1] < x[2])
			{
				x[1] = 1.0f; x[0] = x[2] = 0.0f;
			}
			else
			{
				x[2] = 1.0f; x[0] = x[1] = 0.0f;
			}
		}

		u[0] = x[0] - from[0]; u[1] = x[1] - from[1]; u[2] = x[2] - from[2];
		v[0] = x[0] - to[0];   v[1] = x[1] - to[1];   v[2] = x[2] - to[2];

		c1 = 2.0f / DOT(u, u);
		c2 = 2.0f / DOT(v, v);
		c3 = c1 * c2  * DOT(u, v);

		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				mtx[i][j] =  - c1 * u[i] * u[j]
				- c2 * v[i] * v[j]
				+ c3 * v[i] * u[j];
			}
			mtx[i][i] += 1.0f;
		}
	}
	else  /* the most common case, unless "from"="to", or "from"=-"to" */
	{
#if 1
		/* unoptimized version - a good compiler will optimize this. */
		/* h = (1.0f - e)/DOT(v, v); old code */
		h = 1.0f/(1.0f + e);      /* optimization by Gottfried Chen */
		mtx[0][0] = e + h * c[0] * c[0];
		mtx[0][1] = h * c[0] * c[1] - c[2];
		mtx[0][2] = h * c[0] * c[2] + c[1];

		mtx[1][0] = h * c[0] * c[1] + c[2];
		mtx[1][1] = e + h * c[1] * c[1];
		mtx[1][2] = h * c[1] * c[2] - c[0];

		mtx[2][0] = h * c[0] * c[2] - c[1];
		mtx[2][1] = h * c[1] * c[2] + c[0];
		mtx[2][2] = e + h * c[2] * c[2];
#else
		/* ...otherwise use this hand optimized version (9 mults less) */
		FLOAT hvx, hvz, hvxy, hvxz, hvyz;
		/* h = (1.0f - e)/DOT(c, c); old code */
		h = 1.0f/(1.0f + e);      /* optimization by Gottfried Chen */
		hvx = h * c[0];
		hvz = h * c[2];
		hvxy = hvx * c[1];
		hvxz = hvx * c[2];
		hvyz = hvz * c[1];
		mtx[0][0] = e + hvx * c[0];
		mtx[0][1] = hvxy - c[2];
		mtx[0][2] = hvxz + c[1];

		mtx[1][0] = hvxy + c[2];
		mtx[1][1] = e + h * c[1] * c[1];
		mtx[1][2] = hvyz - c[0];

		mtx[2][0] = hvxz - c[1];
		mtx[2][1] = hvyz + c[0];
		mtx[2][2] = e + hvz * c[2];
#endif
	}

#undef DOT
#undef CROSS
#undef SUB
#undef EPSILON
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
