/*
=============================================================================
	File:	Rectangle.h
	Desc:
=============================================================================
*/

#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__
mxREFACTOR("move to Math");
mxNAMESPACE_BEGIN

//
//	TRectArea
//
template< typename TYPE >
struct TRectArea
{
	TYPE 	TopLeftX;
	TYPE 	TopLeftY;
	TYPE 	Width;
	TYPE 	Height;

public:
	TRectArea();
	TRectArea( TYPE x, TYPE y, TYPE w, TYPE h );

	/** Set the origin and dimensions of the rectangle */
	void Set( TYPE x, TYPE y, TYPE w, TYPE h );

	/** Return true if one of the dimensions (or both) are zero */
	bool IsEmpty() const;

	TYPE Right() const;
	TYPE Bottom() const;

	bool IntersectsX( const TRectArea &r ) const;
	bool IntersectsY( const TRectArea &r ) const;
	bool Intersects ( const TRectArea &r ) const;

	bool ContainsX( TYPE x ) const;
	bool ContainsY( TYPE y ) const;
	bool Contains ( TYPE x, TYPE y ) const;

	void CalcCenter(float* px, float* py) const;

	void Add( const TRectArea &r );

	void Clip( const TRectArea &r );
  
	static TRectArea Create( TYPE x, TYPE y, TYPE w, TYPE h )
	{ return TRectArea(x, y, w, h); }
};

template< typename TYPE >
INLINE 
TRectArea<TYPE>::TRectArea()
{
}

template< typename TYPE >
INLINE 
TRectArea<TYPE>::TRectArea( TYPE x, TYPE y, TYPE w, TYPE h )             
{ 
	TopLeftX   = x; 
	TopLeftY	   = y;
	Width  = w;
	Height = h; 
}

template< typename TYPE >
INLINE 
void TRectArea<TYPE>::Set(TYPE x, TYPE y, TYPE w, TYPE h)
{ 
	TopLeftX   = x; 
	TopLeftY	   = y;
	Width  = w;
	Height = h; 
}

template< typename TYPE >
INLINE 
bool TRectArea<TYPE>::IsEmpty() const
{ 
	return Width <= 0 || Height <= 0; 
}

template< typename TYPE >
INLINE 
TYPE TRectArea<TYPE>::Right() const
{ 
	return TopLeftX + Width; 
}

template< typename TYPE >
INLINE 
TYPE TRectArea<TYPE>::Bottom() const
{ 
	return TopLeftY + Height; 
}

template< typename TYPE >
INLINE 
bool TRectArea<TYPE>::IntersectsX(const TRectArea &r) const 
{ 
	return (TopLeftX < r.Right() && Right() > r.TopLeftX); 
}

template< typename TYPE >
INLINE 
bool TRectArea<TYPE>::IntersectsY(const TRectArea &r) const 
{ 
	return (TopLeftY < r.Bottom() && Bottom() > r.TopLeftY); 
}  

template< typename TYPE >
INLINE 
bool TRectArea<TYPE>::Intersects (const TRectArea &r) const 
{	
	return IntersectsX(r) && IntersectsY(r); 
}

template< typename TYPE >
INLINE 
bool TRectArea<TYPE>::ContainsX( TYPE x)	const 
{ 
	return (x < TopLeftX+Width && x >= TopLeftX); 
}

template< typename TYPE >
INLINE 
bool TRectArea<TYPE>::ContainsY( TYPE y)	const 
{ 
	return (y < TopLeftY+Height && y >= TopLeftY); 
}

template< typename TYPE >
INLINE 
bool TRectArea<TYPE>::Contains ( TYPE x, TYPE y) const 
{ 
	return ContainsX(x) && ContainsY(y); 
}

template< typename TYPE >
INLINE
void TRectArea<TYPE>::CalcCenter( float* px, float* py ) const
{ 
	*px = (float)(TopLeftX + (Width>>1)); 
	*py = (float)(TopLeftY  + (Height>>1)); 
}

template< typename TYPE >
INLINE
void TRectArea<TYPE>::Add(const TRectArea &r)
{
	if (TopLeftX > r.TopLeftX)			{ Width += (TopLeftX - r.TopLeftX); TopLeftX = r.TopLeftX; }
	if (Right() < r.Right())	{ Width += r.Right() - Right(); }
	if (TopLeftY > r.TopLeftY)			{ Height += (TopLeftY - r.TopLeftY); TopLeftY = r.TopLeftY; }
	if (Bottom() < r.Bottom())	{ Height += r.Bottom() - Bottom(); }
}

template< typename TYPE >
INLINE
void TRectArea<TYPE>::Clip(const TRectArea &r)
{
	TYPE dx;
	TYPE dy;

	// Clip X
	if (TopLeftX >= r.TopLeftX)
	{
		if (TopLeftX > r.Right())
			Width = 0;
		else
		{
			dx = r.Right() - Right();

			if (dx < 0)
				Width += dx;
		}
	}
	else
	{
		if (Right() < r.TopLeftX)
			Width = 0;
		else
		{
			Width -= (r.TopLeftX - TopLeftX);
			TopLeftX  =  r.TopLeftX;

			dx = r.Right() - Right();

			if (dx < 0)
				Width += dx;
		}
	}

	// Clip Y

	if (TopLeftY >= r.TopLeftY)
	{
		if (TopLeftY > r.Bottom())
			Height = 0;
		else
		{
			dy = r.Bottom() - Bottom();

			if (dy < 0)
				Height += dy;
		}
	}
	else
	{
		if (Bottom() < r.TopLeftY)
			Height = 0;
		else
		{
			Height -= (r.TopLeftY - TopLeftY);
			TopLeftY  =  r.TopLeftY;

			dy = r.Bottom() - Bottom();

			if (dy < 0)
				Height += dy;
		}
	}
}

INLINE
void ClipRECT( RECT* pRect, const RECT* pVP )
{
	TRectArea<long>	rectAux;
	TRectArea<long>	rectVP;

	rectAux.Set(pRect->left,pRect->top,pRect->right-pRect->left, pRect->bottom-pRect->top);

	rectVP.Set(pVP->left, pVP->top,pVP->right-pVP->left,pVP->bottom-pVP->top);

	rectAux.Clip(rectVP);

	pRect->left = rectAux.TopLeftX;
	pRect->right = rectAux.Right();

	pRect->top = rectAux.TopLeftY;
	pRect->bottom = rectAux.Bottom();
}

//
//	RectF
//
struct RectF
{
	FLOAT 	left;
	FLOAT 	top;
	FLOAT 	right;
	FLOAT 	bottom;

public:
	RectF()
	{}
	RectF( FLOAT Left, FLOAT Top, FLOAT Right, FLOAT Bottom )
		: left(Left), top(Top), right(Right), bottom(Bottom)
	{}
	bool ContainsPoint( FLOAT x, FLOAT y ) const
	{
		return x >= left
			&& x <= right
			&& y >= top
			&& y <= bottom;
	}
};

/** Geometric intersection of two rectanglar regions.
 *
 * @remarks Calculates the geometric intersection of two rectangular
 * regions.  Rectangle coordinates must be ([0-N], [0-N]), such that
 * (0,0) is in the upper left hand corner.
 *
 * If the two input rectangles do not intersect, then the result will be
 * a degenerate rectangle, i.e. left >= right or top >= bottom, or both.
 */
INLINE RectF RectIntersection( const RectF& lhs, const RectF& rhs )
{
	return RectF(
		lhs.left   > rhs.left   ? lhs.left   : rhs.left,		// left
		lhs.top    > rhs.top    ? lhs.top    : rhs.top,			// top
		lhs.right  < rhs.right  ? lhs.right  : rhs.right,		// right
		lhs.bottom < rhs.bottom ? lhs.bottom : rhs.bottom		// bottom
	);
}

mxNAMESPACE_END

#endif // !__RECTANGLE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
