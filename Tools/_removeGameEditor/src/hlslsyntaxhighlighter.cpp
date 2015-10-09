#include "stdafx.h"

#include "hlslsyntaxhighlighter.h"

/*
-----------------------------------------------------------------------------
	HLSLSyntaxHighlighter
-----------------------------------------------------------------------------
*/
HLSLSyntaxHighlighter::HLSLSyntaxHighlighter(QTextDocument* parent)
	: QSyntaxHighlighter( parent )
{

}

HLSLSyntaxHighlighter::~HLSLSyntaxHighlighter()
{

}
void HLSLSyntaxHighlighter::highlightBlock(const QString& text)
{
#if 1
	HLSLUtil & hsls = HLSLUtil::Get();
	QList< HighlightingRule > & rules = hsls.rules;

	foreach ( HighlightingRule rule, rules )
	{
		int index = text.indexOf(rule.pattern);
		while (index >= 0)
		{
			int length = rule.pattern.matchedLength();
			setFormat(index, length, hsls.formats[rule.type]);
			index = text.indexOf(rule.pattern, index + length);
		}
	}

	if (!HLSLUtil::Get().multiLineCommentStart.isEmpty())
	{
		setCurrentBlockState(0);

		int startIndex = 0;
		if (previousBlockState() != 1)
			startIndex = text.indexOf(hsls.multiLineCommentStart);

		while (startIndex >= 0) {
			int endIndex = text.indexOf(hsls.multiLineCommentEnd, startIndex);
			int commentLength;
			if (endIndex == -1) {
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			} else {
				commentLength = endIndex - startIndex + hsls.multiLineCommentStart.length();
			}
			setFormat(startIndex, commentLength, hsls.formats[TT_Comment]);
			startIndex = text.indexOf(hsls.multiLineCommentStart, startIndex + commentLength);
		}
	}
#else

	QTextCharFormat myClassFormat;
	myClassFormat.setFontWeight(QFont::Bold);
	myClassFormat.setForeground(Qt::darkMagenta);
	QString pattern = "\\bMy[A-Za-z]+\\b";

	QRegExp expression(pattern);
	int index = text.indexOf(expression);
	while (index >= 0) {
		int length = expression.matchedLength();
		setFormat(index, length, myClassFormat);
		index = text.indexOf(expression, index + length);
	}
#endif
}

/*
-----------------------------------------------------------------------------
	HLSLUtil
-----------------------------------------------------------------------------
*/
HLSLUtil::HLSLUtil()
{
	setupFormats();
	setupComments();
	setupRules();
}

HLSLUtil::~HLSLUtil()
{

}

void HLSLUtil::setupFormats()
{
	formats.resize(TT_MAX);

	QTextCharFormat format;
	//format.setFont(QFont("Monospace", 8));

	format.setForeground(Qt::darkBlue);
	format.setFontWeight(QFont::Bold);
	formats[TT_Keyword] = format;
	format.setFontWeight(QFont::Normal);

	format.setForeground(Qt::darkCyan);
	format.setFontWeight(QFont::Bold);
	formats[TT_DataType] = format;

	format.setForeground(Qt::darkBlue);
	format.setFontWeight(QFont::Bold);
	formats[TT_BuiltinVar] = format;

	format.setForeground(Qt::darkBlue);
	format.setFontWeight(QFont::Bold);
	formats[TT_BuiltinFunction] = format;
	format.setFontWeight(QFont::Normal);

	format.setForeground(Qt::darkCyan);
	format.setFontWeight(QFont::Bold);
	formats[TT_Number] = format;

	format.setForeground(Qt::darkRed);
	format.setFontWeight(QFont::Normal);
	formats[TT_String] = format;

	format.setFontItalic(true);
	format.setForeground(Qt::gray);
	formats[TT_Comment] = format;

	format.setFontItalic(false);
	format.setForeground(Qt::darkYellow);
	formats[TT_Misc] = format;
}

void HLSLUtil::setupRules()
{
	HighlightingRule rule;

	rule.type = TT_Keyword;
	rule.pattern = QRegExp("\\b(if|else|for|while|do|struct|break|continue|discard|return|technique|pass|sampler_state"
		"|cbuffer|register"
		"|in|out|inout"
		"|compile|true|false|packed|typedef)\\b");
	rules.append(rule);

	rule.type = TT_DataType;
	rule.pattern = QRegExp(
		"\\b("
		"void"
		
		"|float|float[1-4]|float[1-4]x[1-4]"
		"|half|half[1-4]|half[1-4]x[1-4]|fixed|fixed[1-4]|fixed[1-4]x[1-4]"

		"|int|int[1-4]|int[1-4]x[1-4]"
		"|uint|uint[1-4]|uint[1-4]x[1-4]"
		
		"|bool|bool[1-4]|bool[1-4]x[1-4]"
		
		"|SamplerState|Texture2D"

		"|sampler[1-3]D|samplerCUBE|samplerRECT|texture|string|uniform|varying|static|const"

		")\\b");
	rules.append(rule);

	rule.type = TT_BuiltinVar;
	rule.pattern = QRegExp(
		"\\b("

		"POSITION"

		//"gl_(Position|PointSize|ClipVertex|FragCoord|FragFacing|FragColor|"
		//"FragData|FragDepth|Color|SecondaryColor|Normal|Vertex|MultiTexCoord[0-7]|FogColor|"
		//"MaxLights|MaxClipPlanes|MaxTextureUnits|MaxTextureCoords|MaxVertexAttributes|"
		//"MaxVertexUniformComponents|MaxVaryingFloats|MaxVertexTextureImageUnits|MaxCombinedTextureImageUnits|"
		//"MaxTextureImageUnits|MaxFragmentUniformComponents|MaxDrawBuffers|ModelViewMatrix|ProjectionMatrix|"
		//"ModelViewProjectionMatrix|TextureMatrix|NormalMatrix|ModelViewMatrixInverse|ProjectionMatrixInverse|"
		//"ModelViewProjectionMatrixInverse|TextureMatrixInverse|ModelViewMatrixTranspose|ProjectionMatrixTranspose|"
		//"ModelViewProjectionMatrixTranspose|TextureMatrixTranspose|ModelViewMatrixInverseTranspose|ProjectionMatrixInverseTranspose|"
		//"ModelViewProjectionMatrixInverseTranspose|TextureMatrixInverseTranspose|NormScale|DepthRangeParameters|DepthRange|"
		//"ClipPlane|PointParameters|Point|MaterialParameters|FrontMaterial|BackMaterial|LightSourceParameters|LightSource|"
		//"LightModelParameters|LightModel|LightModelProducts|FrontLightModelProduct|BackLightModelProduct|LightProducts|"
		//"FrontLightProduct|BackLightProduct|TextureEnvColor|EyePlaneS|EyePlaneT|EyePlaneR|EyePlaneQ|ObjectPlaneS|ObjectPlaneT|"
		//"ObjectPlaneR|ObjectPlaneQ|FogParameters|Fog|FrontColor|BackColor|FrontSecondaryColor|BackSecondaryColor|TexCoord|FogFragCoord|Color|"
		//"SecondaryColor)|"
		//"WorldViewProjection(Inverse)?(Transpose)?|ModelView(Projection)?(Inverse)?(Transpose)?|View(Inverse)?(Transpose)?|"
		//"World(Inverse)?(Transpose)?|Projection(Inverse)?(Transpose)?|Time|ViewportSize|"
		//"MinFilter|MagFilter|WrapS|WrapT|BorderColor|"
		
		//"|COLOR[0-1]?|TEXCOORD[0-7]?|NORMAL[0-7]?"

		//|"VertexProgram|FragmentProgram|DepthTestEnable|CullFaceEnable|register\\(c[1-2]+\\)

		"|SV_Position|SV_Target[0-7]?|SV_VertexID"
		"|TexCoord[0-7]?|Normal[0-7]?"

		")\\b"
	);
	rules.append(rule);

	rule.type = TT_BuiltinFunction;
	rule.pattern = QRegExp(
		"\\b(abs|acos|all|any|asin|atan|atan2|ceil|clamp|cos|cosh|cross|degrees|determinant|dot|floor|length|lerp|"
		"max|min|mul|normalize|radians|reflect|refract|round|saturate|sin|sinh|tan|tanh|transpose|"
		"tex1D(proj)?|tex2D(proj)?|texRECT(proj)?|tex3D(proj)?|texCUBE(proj)?|"
		"offsettex2D|offsettexRECT|offsettex2DScaleBias|offsettexRECTScaleBias|tex1D_dp3|tex2D_dp3x2|"
		"texRECT_dp3x2|tex3D_dp3x3|texCUBE_dp3x3|texCUBE_reflect_dp3x3|texCUBE_reflect_eye_dp3x3|tex_dp3x2_depth|"
		"(un)?pack_4(u)?byte|(un)?pack_2ushort|(un)?pack_2half)\\b");
	rules.append(rule);


	rule.type = TT_Number;
	rule.pattern = QRegExp("\\b[-+]?\\d*\\.?\\d+([eE][-+]?\\d+)?[fFhHxX]?\\b");
	rules.append(rule);

	rule.type = TT_String;
	rule.pattern = QRegExp("\".*\"");
	rules.append(rule);

	rule.type = TT_Misc;
	rule.pattern = QRegExp("#.*(//|$)");
	rules.append(rule);

	rule.type = TT_Comment;
	rule.pattern = QRegExp("//.*$");
	rules.append(rule);
}

void HLSLUtil::setupComments()
{
	multiLineCommentStart = "/*";
	multiLineCommentEnd = "*/";
}



