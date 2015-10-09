/*
=============================================================================
	File:	Color.cpp
	Desc:	Color representation.
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

#include "Color.h"

mxNAMESPACE_BEGIN

// The per-color weighting to be used for luminance calculations in RGB order.
const FLOAT LUMINANCE_VECTOR[ 3 ] =
{
//	0.2126f, 0.7152f, 0.0722f
//	0.27f, 0.67f, 0.06f
	0.3f, 0.59f, 0.11f
};

/*================================
			SColor
================================*/

//mxBEGIN_REFLECTION(SColor)
//	FIELD(SColor,A)
//	FIELD(SColor,R)
//	FIELD(SColor,G)
//	FIELD(SColor,B)
//mxEND_REFLECTION

/*================================
			FColor
================================*/

//mxBEGIN_REFLECTION(FColor)
//	FIELD(FColor,R)
//	FIELD(FColor,G)
//	FIELD(FColor,B)
//	FIELD(FColor,A)
//mxEND_REFLECTION

const FColor	FColor::BLACK				( 0.00f, 0.00f, 0.00f, 1.00f );
const FColor	FColor::DARK_GREY			( 0.25f, 0.25f, 0.25f, 1.00f );
const FColor	FColor::GRAY				( 0.50f, 0.50f, 0.50f, 1.00f );
const FColor	FColor::LIGHT_GREY			( 0.75f, 0.75f, 0.75f, 1.00f );
const FColor	FColor::WHITE				( 1.00f, 1.00f, 1.00f, 1.00f );

const FColor	FColor::RED					( 1.00f, 0.00f, 0.00f, 1.00f );
const FColor	FColor::GREEN				( 0.00f, 1.00f, 0.00f, 1.00f );
const FColor	FColor::BLUE				( 0.00f, 0.00f, 1.00f, 1.00f );
const FColor	FColor::YELLOW				( 1.00f, 1.00f, 0.00f, 1.00f );
const FColor	FColor::MAGENTA				( 1.00f, 0.00f, 1.00f, 1.00f );
const FColor	FColor::CYAN				( 0.00f, 1.00f, 1.00f, 1.00f );
const FColor	FColor::ORANGE				( 1.00f, 0.50f, 0.00f, 1.00f );	
const FColor	FColor::PURPLE				( 0.60f, 0.00f, 0.60f, 1.00f );
const FColor	FColor::PINK				( 0.73f, 0.40f, 0.48f, 1.00f );
const FColor	FColor::BROWN				( 0.40f, 0.35f, 0.08f, 1.00f );

const FColor	FColor::BEACH_SAND			( 1.00f, 0.96f, 0.62f, 1.00f );
const FColor	FColor::LIGHT_YELLOW_GREEN	( 0.48f, 0.77f, 0.46f, 1.00f );
const FColor	FColor::DARK_YELLOW_GREEN	( 0.10f, 0.48f, 0.19f, 1.00f );
const FColor	FColor::DARKBROWN			( 0.45f, 0.39f, 0.34f, 1.00f );

//
// colors taken from http://prideout.net/archive/colors.php
//
const FColor	FColor::aliceblue			( 0.941f, 0.973f, 1.000f );
const FColor	FColor::antiquewhite		( 0.980f, 0.922f, 0.843f );
const FColor	FColor::aqua				( 0.000f, 1.000f, 1.000f );
const FColor	FColor::aquamarine			( 0.498f, 1.000f, 0.831f );
const FColor	FColor::azure				( 0.941f, 1.000f, 1.000f );
const FColor	FColor::beige				( 0.961f, 0.961f, 0.863f );
const FColor	FColor::bisque				( 1.000f, 0.894f, 0.769f );
const FColor	FColor::black				( 0.000f, 0.000f, 0.000f );
const FColor	FColor::blanchedalmond		( 1.000f, 0.922f, 0.804f );
const FColor	FColor::blue				( 0.000f, 0.000f, 1.000f );
const FColor	FColor::blueviolet			( 0.541f, 0.169f, 0.886f );
const FColor	FColor::brown				( 0.647f, 0.165f, 0.165f );
const FColor	FColor::burlywood			( 0.871f, 0.722f, 0.529f );
const FColor	FColor::cadetblue			( 0.373f, 0.620f, 0.627f );
const FColor	FColor::chartreuse			( 0.498f, 1.000f, 0.000f );
const FColor	FColor::chocolate			( 0.824f, 0.412f, 0.118f );
const FColor	FColor::coral				( 1.000f, 0.498f, 0.314f );
const FColor	FColor::cornflowerblue		( 0.392f, 0.584f, 0.929f );
const FColor	FColor::cornsilk			( 1.000f, 0.973f, 0.863f );
const FColor	FColor::crimson				( 0.863f, 0.078f, 0.235f );
const FColor	FColor::cyan				( 0.000f, 1.000f, 1.000f );
const FColor	FColor::darkblue			( 0.000f, 0.000f, 0.545f );
const FColor	FColor::darkcyan			( 0.000f, 0.545f, 0.545f );
const FColor	FColor::darkgoldenrod		( 0.722f, 0.525f, 0.043f );
const FColor	FColor::darkgray			( 0.663f, 0.663f, 0.663f );
const FColor	FColor::darkgreen			( 0.000f, 0.392f, 0.000f );
const FColor	FColor::darkgrey			( 0.663f, 0.663f, 0.663f );
const FColor	FColor::darkkhaki			( 0.741f, 0.718f, 0.420f );
const FColor	FColor::darkmagenta			( 0.545f, 0.000f, 0.545f );
const FColor	FColor::darkolivegreen		( 0.333f, 0.420f, 0.184f );
const FColor	FColor::darkorange			( 1.000f, 0.549f, 0.000f );
const FColor	FColor::darkorchid			( 0.600f, 0.196f, 0.800f );
const FColor	FColor::darkred				( 0.545f, 0.000f, 0.000f );
const FColor	FColor::darksalmon			( 0.914f, 0.588f, 0.478f );
const FColor	FColor::darkseagreen		( 0.561f, 0.737f, 0.561f );
const FColor	FColor::darkslateblue		( 0.282f, 0.239f, 0.545f );
const FColor	FColor::darkslategray		( 0.184f, 0.310f, 0.310f );
const FColor	FColor::darkslategrey		( 0.184f, 0.310f, 0.310f );
const FColor	FColor::darkturquoise		( 0.000f, 0.808f, 0.820f );
const FColor	FColor::darkviolet			( 0.580f, 0.000f, 0.827f );
const FColor	FColor::deeppink			( 1.000f, 0.078f, 0.576f );
const FColor	FColor::deepskyblue			( 0.000f, 0.749f, 1.000f );
const FColor	FColor::dimgray				( 0.412f, 0.412f, 0.412f );
const FColor	FColor::dimgrey				( 0.412f, 0.412f, 0.412f );
const FColor	FColor::dodgerblue			( 0.118f, 0.565f, 1.000f );
const FColor	FColor::firebrick			( 0.698f, 0.133f, 0.133f );
const FColor	FColor::floralwhite			( 1.000f, 0.980f, 0.941f );
const FColor	FColor::forestgreen			( 0.133f, 0.545f, 0.133f );
const FColor	FColor::fuchsia				( 1.000f, 0.000f, 1.000f );
const FColor	FColor::gainsboro			( 0.863f, 0.863f, 0.863f );
const FColor	FColor::ghostwhite			( 0.973f, 0.973f, 1.000f );
const FColor	FColor::gold				( 1.000f, 0.843f, 0.000f );
const FColor	FColor::goldenrod			( 0.855f, 0.647f, 0.125f );
const FColor	FColor::gray				( 0.502f, 0.502f, 0.502f );
const FColor	FColor::green				( 0.000f, 0.502f, 0.000f );
const FColor	FColor::greenyellow			( 0.678f, 1.000f, 0.184f );
const FColor	FColor::grey				( 0.502f, 0.502f, 0.502f );
const FColor	FColor::honeydew			( 0.941f, 1.000f, 0.941f );
const FColor	FColor::hotpink				( 1.000f, 0.412f, 0.706f );
const FColor	FColor::indianred			( 0.804f, 0.361f, 0.361f );
const FColor	FColor::indigo				( 0.294f, 0.000f, 0.510f );
const FColor	FColor::ivory				( 1.000f, 1.000f, 0.941f );
const FColor	FColor::khaki				( 0.941f, 0.902f, 0.549f );
const FColor	FColor::lavender			( 0.902f, 0.902f, 0.980f );
const FColor	FColor::lavenderblush		( 1.000f, 0.941f, 0.961f );
const FColor	FColor::lawngreen			( 0.486f, 0.988f, 0.000f );
const FColor	FColor::lemonchiffon		( 1.000f, 0.980f, 0.804f );
const FColor	FColor::lightblue			( 0.678f, 0.847f, 0.902f );
const FColor	FColor::lightcoral			( 0.941f, 0.502f, 0.502f );
const FColor	FColor::lightcyan			( 0.878f, 1.000f, 1.000f );
const FColor	FColor::lightgoldenrodyellow( 0.980f, 0.980f, 0.824f );
const FColor	FColor::lightgray			( 0.827f, 0.827f, 0.827f );
const FColor	FColor::lightgreen			( 0.565f, 0.933f, 0.565f );
const FColor	FColor::lightgrey			( 0.827f, 0.827f, 0.827f );
const FColor	FColor::lightpink			( 1.000f, 0.714f, 0.757f );
const FColor	FColor::lightsalmon			( 1.000f, 0.627f, 0.478f );
const FColor	FColor::lightseagreen		( 0.125f, 0.698f, 0.667f );
const FColor	FColor::lightskyblue		( 0.529f, 0.808f, 0.980f );
const FColor	FColor::lightslategray		( 0.467f, 0.533f, 0.600f );
const FColor	FColor::lightslategrey		( 0.467f, 0.533f, 0.600f );
const FColor	FColor::lightsteelblue		( 0.690f, 0.769f, 0.871f );
const FColor	FColor::lightyellow			( 1.000f, 1.000f, 0.878f );
const FColor	FColor::lime				( 0.000f, 1.000f, 0.000f );
const FColor	FColor::limegreen			( 0.196f, 0.804f, 0.196f );
const FColor	FColor::linen				( 0.980f, 0.941f, 0.902f );
const FColor	FColor::magenta				( 1.000f, 0.000f, 1.000f );
const FColor	FColor::maroon				( 0.502f, 0.000f, 0.000f );
const FColor	FColor::mediumaquamarine	( 0.400f, 0.804f, 0.667f );
const FColor	FColor::mediumblue			( 0.000f, 0.000f, 0.804f );
const FColor	FColor::mediumorchid		( 0.729f, 0.333f, 0.827f );
const FColor	FColor::mediumpurple		( 0.576f, 0.439f, 0.859f );
const FColor	FColor::mediumseagreen		( 0.235f, 0.702f, 0.443f );
const FColor	FColor::mediumslateblue		( 0.482f, 0.408f, 0.933f );
const FColor	FColor::mediumspringgreen	( 0.000f, 0.980f, 0.604f );
const FColor	FColor::mediumturquoise		( 0.282f, 0.820f, 0.800f );
const FColor	FColor::mediumvioletred		( 0.780f, 0.082f, 0.522f );
const FColor	FColor::midnightblue		( 0.098f, 0.098f, 0.439f );
const FColor	FColor::mintcream			( 0.961f, 1.000f, 0.980f );
const FColor	FColor::mistyrose			( 1.000f, 0.894f, 0.882f );
const FColor	FColor::moccasin			( 1.000f, 0.894f, 0.710f );
const FColor	FColor::navajowhite			( 1.000f, 0.871f, 0.678f );
const FColor	FColor::navy				( 0.000f, 0.000f, 0.502f );
const FColor	FColor::oldlace				( 0.992f, 0.961f, 0.902f );
const FColor	FColor::olive				( 0.502f, 0.502f, 0.000f );
const FColor	FColor::olivedrab			( 0.420f, 0.557f, 0.137f );
const FColor	FColor::orange				( 1.000f, 0.647f, 0.000f );
const FColor	FColor::orangered			( 1.000f, 0.271f, 0.000f );
const FColor	FColor::orchid				( 0.855f, 0.439f, 0.839f );
const FColor	FColor::palegoldenrod		( 0.933f, 0.910f, 0.667f );
const FColor	FColor::palegreen			( 0.596f, 0.984f, 0.596f );
const FColor	FColor::paleturquoise		( 0.686f, 0.933f, 0.933f );
const FColor	FColor::palevioletred		( 0.859f, 0.439f, 0.576f );
const FColor	FColor::papayawhip			( 1.000f, 0.937f, 0.835f );
const FColor	FColor::peachpuff			( 1.000f, 0.855f, 0.725f );
const FColor	FColor::peru				( 0.804f, 0.522f, 0.247f );
const FColor	FColor::pink				( 1.000f, 0.753f, 0.796f );
const FColor	FColor::plum				( 0.867f, 0.627f, 0.867f );
const FColor	FColor::powderblue			( 0.690f, 0.878f, 0.902f );
const FColor	FColor::purple				( 0.502f, 0.000f, 0.502f );
const FColor	FColor::red					( 1.000f, 0.000f, 0.000f );
const FColor	FColor::rosybrown			( 0.737f, 0.561f, 0.561f );
const FColor	FColor::royalblue			( 0.255f, 0.412f, 0.882f );
const FColor	FColor::saddlebrown			( 0.545f, 0.271f, 0.075f );
const FColor	FColor::salmon				( 0.980f, 0.502f, 0.447f );
const FColor	FColor::sandybrown			( 0.957f, 0.643f, 0.376f );
const FColor	FColor::seagreen			( 0.180f, 0.545f, 0.341f );
const FColor	FColor::seashell			( 1.000f, 0.961f, 0.933f );
const FColor	FColor::sienna				( 0.627f, 0.322f, 0.176f );
const FColor	FColor::silver				( 0.753f, 0.753f, 0.753f );
const FColor	FColor::skyblue				( 0.529f, 0.808f, 0.922f );
const FColor	FColor::slateblue			( 0.416f, 0.353f, 0.804f );
const FColor	FColor::slategray			( 0.439f, 0.502f, 0.565f );
const FColor	FColor::slategrey			( 0.439f, 0.502f, 0.565f );
const FColor	FColor::snow				( 1.000f, 0.980f, 0.980f );
const FColor	FColor::springgreen			( 0.000f, 1.000f, 0.498f );
const FColor	FColor::steelblue			( 0.275f, 0.510f, 0.706f );
const FColor	FColor::tan					( 0.824f, 0.706f, 0.549f );
const FColor	FColor::teal				( 0.000f, 0.502f, 0.502f );
const FColor	FColor::thistle				( 0.847f, 0.749f, 0.847f );
const FColor	FColor::tomato				( 1.000f, 0.388f, 0.278f );
const FColor	FColor::turquoise			( 0.251f, 0.878f, 0.816f );
const FColor	FColor::violet				( 0.933f, 0.510f, 0.933f );
const FColor	FColor::wheat				( 0.961f, 0.871f, 0.702f );
const FColor	FColor::white				( 1.000f, 1.000f, 1.000f );
const FColor	FColor::whitesmoke			( 0.961f, 0.961f, 0.961f );
const FColor	FColor::yellow				( 1.000f, 1.000f, 0.000f );
const FColor	FColor::yellowgreen			( 0.604f, 0.804f, 0.196f );

//BUG: is this correct?
FORCEINLINE U4 ToRGBA32( UINT8 r, UINT8 g, UINT8 b, UINT8 a )
{
	U4 c = a;
	c |= r << 24;
	c |= g << 16;
	c |= b << 8;
	return c;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
