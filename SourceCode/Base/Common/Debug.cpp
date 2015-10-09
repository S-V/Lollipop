/*
=============================================================================
	File:	Debug.cpp
	Desc:	Code for debug utils, assertions and other very useful things.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

namespace Debug
{
	static int DefaultUserAssertCallback( const char* expr, const char* file, const char* func, unsigned int line, bool* ignore )
	{
		return 1;
	}

	static UserAssertCallback userAssertCallback = DefaultUserAssertCallback;

}//Debug

UserAssertCallback SetUserAssertCallback( UserAssertCallback cb )
{
	UserAssertCallback prevCallback = 
		(Debug::userAssertCallback == Debug::DefaultUserAssertCallback) ?
		nil : Debug::userAssertCallback;

	Debug::userAssertCallback = cb ? cb : Debug::DefaultUserAssertCallback;

	return prevCallback;
}

#if MX_DEBUG
	/*
	================================
			DBGOUT
	================================
	*/
	void VARARGS DBGOUT( const char* format, ... )
	{
		if(0)
		{
			char	buffer[ MAX_STRING_CHARS ];
			MX_GET_VARARGS_ANSI( buffer, format );

			mxPrintfAnsi( buffer );
			DebugOut( buffer );
		}
		else	// indentation for nicer formatting
		{
			char	buffer[ MAX_STRING_CHARS ];
			MX_GET_VARARGS_ANSI( buffer, format );

			char	buffer2[ MAX_STRING_CHARS ];
			MX_SPRINTF_ANSI( buffer2, "------ %s", buffer );

			mxPrintfAnsi( buffer2 );
			DebugOut( buffer2 );
		}
	}

	void DBGNEWLINE()
	{
		mxPrintfAnsi( LINE_CHARS.ToChars() );
		DebugOut( LINE_CHARS.ToChars() );
	}
#endif // MX_DEBUG


#if MX_DEVELOPER
	void VARARGS DEVOUT( const char* format, ... )
	{
		if(0)
		{
			char	buffer[ MAX_STRING_CHARS ];
			MX_GET_VARARGS_ANSI( buffer, format );

			mxPut( buffer );
		}
		else	// indentation for nicer formatting
		{
			char	buffer[ MAX_STRING_CHARS ];
			MX_GET_VARARGS_ANSI( buffer, format );

			char	buffer2[ MAX_STRING_CHARS ];
			MX_SPRINTF_ANSI( buffer2, "--- %s", buffer );

			mxPut( buffer2 );
		}
	}
	void DEVNEWLINE()
	{
		mxPut( LINE_CHARS.ToChars() );
	}
#endif // MX_DEVELOPER

/*
================================
		OnAssertionFailed
================================
*/
void OnAssertionFailed( const char* expression, const char* filename, const char* function, int line, bool* ignore )
{
	if( 0 == Debug::userAssertCallback(expression,filename,function,line,ignore) )
	{
		return;
	}

	char  buffer[ MAX_STRING_CHARS ];
	mxSPrintfAnsi( buffer, NUMBER_OF(buffer), "Assertion failed:\n\n '%s'\n\n in file %s, function '%s', line %d\n", expression, filename, function, line );

#if MX_PLATFORM_WIN32 || MX_PLATFORM_WIN64
	if( ::IsDebuggerPresent() )
	{
		mxPut( buffer );
		if( ignore != nil )
		{
			mxStrCatAnsi( buffer, ("\nDo you wish to debug?\nYes - 'Debug Break', No - 'Exit', Cancel or Close - 'Don't bother me again!'\n") );
			const int result = ::MessageBoxA( NULL, buffer, ("Assertion failed"), MB_YESNOCANCEL | MB_ICONERROR );
			if( IDYES == result ) {
				DebugBreak();
				return;
			}
			else
			if( IDCANCEL == result ) {
				*ignore = true;
				return;
			}
		}
		else
		{
			mxStrCatAnsi( buffer, ("\nDo you wish to debug?") );
			const int result = ::MessageBoxA( NULL, buffer, ("Assertion failed"), MB_YESNO | MB_ICONERROR );
			if( IDYES == result ) {
				DebugBreak();
				return;
			}
		}
	}
#endif

	mxFatal( "Assertion failed" );
}

/*
================================
		OnAssertionFailedX
================================
*/
void OnAssertionFailedX( const char* expression, const char* message, const char* filename, const char* function, int line, bool* ignore )
{
	if( 0 == Debug::userAssertCallback(expression,filename,function,line,ignore) )
	{
		return;
	}

	char  buffer[ MAX_STRING_CHARS ];
	mxSPrintfAnsi( buffer, NUMBER_OF(buffer),
		"Assertion failed:\n\n '%s',\n\n '%s'\n\n in file %s, function '%s', line %d\n", message, expression, filename, function, line );

#if MX_PLATFORM_WIN32 || MX_PLATFORM_WIN64
	if( ::IsDebuggerPresent() )
	{
		mxPut( buffer );
		if( ignore != nil )
		{
			mxStrCatAnsi( buffer, ("\nDo you wish to debug?\nY - 'Debug Break', N - 'Exit', Cancel - 'Don't bother me again!'\n") );
			const int result = ::MessageBoxA( NULL, buffer, ("Assertion failed"), MB_YESNOCANCEL | MB_ICONERROR );
			if( IDYES == result ) {
				DebugBreak();
				return;
			}
			else
			if( IDCANCEL == result ) {
				*ignore = true;
				return;
			}
		}
		else
		{
			mxStrCatAnsi( buffer, "\nDo you wish to debug?" );
			const int result = ::MessageBoxA( NULL, buffer, "Assertion failed", MB_YESNO | MB_ICONERROR );
			if( IDYES == result ) {
				DebugBreak();
				return;
			}
		}
	}
#endif

	mxFatal( "Assertion failed" );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
