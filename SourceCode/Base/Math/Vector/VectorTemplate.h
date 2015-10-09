/*
=============================================================================
	File:	VectorTemplate.h
	Desc:	Templated vector classes.
=============================================================================
*/

#ifndef __MATH_VECTOR_TEMPLATE_H__
#define __MATH_VECTOR_TEMPLATE_H__

mxNAMESPACE_BEGIN

//@todo: Use Eigen.

mxSWIPED("Irrlicht");

template <class T>
class vector2d
{
public:
	//! Default constructor (null vector)
	vector2d() : x(0), y(0) {}
	//! Constructor with two different values
	vector2d(T nx, T ny) : x(nx), y(ny) {}
	//! Constructor with the same value for both members
	explicit vector2d(T n) : x(n), y(n) {}
	//! Copy constructor
	vector2d(const vector2d<T>& other) : x(other.X), y(other.Y) {}

	// operators

	vector2d<T> operator-() const { return vector2d<T>(-x, -y); }

	vector2d<T>& operator=(const vector2d<T>& other) { x = other.X; y = other.Y; return *this; }

	vector2d<T> operator+(const vector2d<T>& other) const { return vector2d<T>(x + other.X, y + other.Y); }
	vector2d<T>& operator+=(const vector2d<T>& other) { x+=other.X; y+=other.Y; return *this; }
	vector2d<T> operator+(const T v) const { return vector2d<T>(x + v, y + v); }
	vector2d<T>& operator+=(const T v) { x+=v; y+=v; return *this; }

	vector2d<T> operator-(const vector2d<T>& other) const { return vector2d<T>(x - other.X, y - other.Y); }
	vector2d<T>& operator-=(const vector2d<T>& other) { x-=other.X; y-=other.Y; return *this; }
	vector2d<T> operator-(const T v) const { return vector2d<T>(x - v, y - v); }
	vector2d<T>& operator-=(const T v) { x-=v; y-=v; return *this; }

	vector2d<T> operator*(const vector2d<T>& other) const { return vector2d<T>(x * other.X, y * other.Y); }
	vector2d<T>& operator*=(const vector2d<T>& other) { x*=other.X; y*=other.Y; return *this; }
	vector2d<T> operator*(const T v) const { return vector2d<T>(x * v, y * v); }
	vector2d<T>& operator*=(const T v) { x*=v; y*=v; return *this; }

	vector2d<T> operator/(const vector2d<T>& other) const { return vector2d<T>(x / other.X, y / other.Y); }
	vector2d<T>& operator/=(const vector2d<T>& other) { x/=other.X; y/=other.Y; return *this; }
	vector2d<T> operator/(const T v) const { return vector2d<T>(x / v, y / v); }
	vector2d<T>& operator/=(const T v) { x/=v; y/=v; return *this; }

	//! sort in order x, Y. Equality with rounding tolerance.
	bool operator<=(const vector2d<T>&other) const
	{
		return 	(x<other.X || core::equals(x, other.X)) ||
				(core::equals(x, other.X) && (y<other.Y || core::equals(y, other.Y)));
	}

	//! sort in order x, Y. Equality with rounding tolerance.
	bool operator>=(const vector2d<T>&other) const
	{
		return 	(x>other.X || core::equals(x, other.X)) ||
				(core::equals(x, other.X) && (y>other.Y || core::equals(y, other.Y)));
	}

	//! sort in order x, Y. Difference must be above rounding tolerance.
	bool operator<(const vector2d<T>&other) const
	{
		return 	(x<other.X && !core::equals(x, other.X)) ||
				(core::equals(x, other.X) && y<other.Y && !core::equals(y, other.Y));
	}

	//! sort in order x, Y. Difference must be above rounding tolerance.
	bool operator>(const vector2d<T>&other) const
	{
		return 	(x>other.X && !core::equals(x, other.X)) ||
				(core::equals(x, other.X) && y>other.Y && !core::equals(y, other.Y));
	}

	bool operator==(const vector2d<T>& other) const { return equals(other); }
	bool operator!=(const vector2d<T>& other) const { return !equals(other); }

	// functions

	//! Checks if this vector equals the other one.
	/** Takes floating point rounding errors into account.
	\param other Vector to compare with.
	\return True if the two vector are (almost) equal, else false. */
	bool equals(const vector2d<T>& other) const
	{
		return core::equals(x, other.X) && core::equals(y, other.Y);
	}

	vector2d<T>& set(T nx, T ny) {x=nx; y=ny; return *this; }
	vector2d<T>& set(const vector2d<T>& p) { x=p.X; y=p.Y; return *this; }

	//! Gets the length of the vector.
	/** \return The length of the vector. */
	T getLength() const { return core::squareroot( x*x + y*y ); }

	//! Get the squared length of this vector
	/** This is useful because it is much faster than getLength().
	\return The squared length of the vector. */
	T getLengthSQ() const { return x*x + y*y; }

	//! Get the dot product of this vector with another.
	/** \param other Other vector to take dot product with.
	\return The dot product of the two vectors. */
	T dotProduct(const vector2d<T>& other) const
	{
		return x*other.X + y*other.Y;
	}

	//! Gets distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Distance from other point. */
	T getDistanceFrom(const vector2d<T>& other) const
	{
		return vector2d<T>(x - other.X, y - other.Y).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Squared distance from other point. */
	T getDistanceFromSQ(const vector2d<T>& other) const
	{
		return vector2d<T>(x - other.X, y - other.Y).getLengthSQ();
	}

	//! rotates the point anticlockwise around a center by an amount of degrees.
	/** \param degrees Amount of degrees to rotate by, anticlockwise.
	\param center Rotation center.
	\return This vector after transformation. */
	vector2d<T>& rotateBy(F8 degrees, const vector2d<T>& center=vector2d<T>())
	{
		degrees *= DEGTORAD64;
		const F8 cs = cos(degrees);
		const F8 sn = sin(degrees);

		x -= center.X;
		y -= center.Y;

		set((T)(x*cs - y*sn), (T)(x*sn + y*cs));

		x += center.X;
		y += center.Y;
		return *this;
	}

	//! Normalize the vector.
	/** The null vector is left untouched.
	\return Reference to this vector, after normalization. */
	vector2d<T>& normalize()
	{
		F4 length = (F4)(x*x + y*y);
		if (core::equals(length, 0.f))
			return *this;
		length = core::reciprocal_squareroot ( length );
		x = (T)(x * length);
		y = (T)(y * length);
		return *this;
	}

	//! Calculates the angle of this vector in degrees in the trigonometric sense.
	/** 0 is to the right (3 o'clock), values increase counter-clockwise.
	This method has been suggested by Pr3t3nd3r.
	\return Returns a value between 0 and 360. */
	F8 getAngleTrig() const
	{
		if (y == 0)
			return x < 0 ? 180 : 0;
		else
		if (x == 0)
			return y < 0 ? 270 : 90;

		if ( y > 0)
			if (x > 0)
				return atan((irr::F8)y/(irr::F8)x) * RADTODEG64;
			else
				return 180.0-atan((irr::F8)y/-(irr::F8)x) * RADTODEG64;
		else
			if (x > 0)
				return 360.0-atan(-(irr::F8)y/(irr::F8)x) * RADTODEG64;
			else
				return 180.0+atan(-(irr::F8)y/-(irr::F8)x) * RADTODEG64;
	}

	//! Calculates the angle of this vector in degrees in the counter trigonometric sense.
	/** 0 is to the right (3 o'clock), values increase clockwise.
	\return Returns a value between 0 and 360. */
	inline F8 getAngle() const
	{
		if (y == 0) // corrected thanks to a suggestion by Jox
			return x < 0 ? 180 : 0;
		else if (x == 0)
			return y < 0 ? 90 : 270;

		// don't use getLength here to avoid precision loss with s32 vectors
		F8 tmp = y / sqrt((F8)(x*x + y*y));
		tmp = atan( core::squareroot(1 - tmp*tmp) / tmp) * RADTODEG64;

		if (x>0 && y>0)
			return tmp + 270;
		else
		if (x>0 && y<0)
			return tmp + 90;
		else
		if (x<0 && y<0)
			return 90 - tmp;
		else
		if (x<0 && y>0)
			return 270 - tmp;

		return tmp;
	}

	//! Calculates the angle between this vector and another one in degree.
	/** \param b Other vector to test with.
	\return Returns a value between 0 and 90. */
	inline F8 getAngleWith(const vector2d<T>& b) const
	{
		F8 tmp = x*b.X + y*b.Y;

		if (tmp == 0.0)
			return 90.0;

		tmp = tmp / core::squareroot((F8)((x*x + y*y) * (b.X*b.X + b.Y*b.Y)));
		if (tmp < 0.0)
			tmp = -tmp;

		return atan(sqrt(1 - tmp*tmp) / tmp) * RADTODEG64;
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const vector2d<T>& begin, const vector2d<T>& end) const
	{
		if (begin.X != end.X)
		{
			return ((begin.X <= x && x <= end.X) ||
				(begin.X >= x && x >= end.X));
		}
		else
		{
			return ((begin.Y <= y && y <= end.Y) ||
				(begin.Y >= y && y >= end.Y));
		}
	}

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	vector2d<T> getInterpolated(const vector2d<T>& other, F8 d) const
	{
		F8 inv = 1.0f - d;
		return vector2d<T>((T)(other.X*inv + x*d), (T)(other.Y*inv + y*d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	vector2d<T> getInterpolated_quadratic(const vector2d<T>& v2, const vector2d<T>& v3, F8 d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const F8 inv = 1.0f - d;
		const F8 mul0 = inv * inv;
		const F8 mul1 = 2.0f * d * inv;
		const F8 mul2 = d * d;

		return vector2d<T> ( (T)(x * mul0 + v2.X * mul1 + v3.X * mul2),
					(T)(y * mul0 + v2.Y * mul1 + v3.Y * mul2));
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	vector2d<T>& interpolate(const vector2d<T>& a, const vector2d<T>& b, F8 d)
	{
		x = (T)((F8)b.X + ( ( a.X - b.X ) * d ));
		y = (T)((F8)b.Y + ( ( a.Y - b.Y ) * d ));
		return *this;
	}

	//! X coordinate of vector.
	T x;

	//! Y coordinate of vector.
	T y;
};

mxNAMESPACE_END

#endif /* !__MATH_VECTOR_TEMPLATE_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
