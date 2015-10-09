/*
=============================================================================
	File:	LogUtil.h
	Desc:	
=============================================================================
*/

#ifndef __MX_LOG_UTIL_H__
#define __MX_LOG_UTIL_H__

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

//--------------------------------------------------------------//
//	Logging.
//--------------------------------------------------------------//

void F_Util_ComposeLogFileName( OSFileName & logFileName );

// writes current date and time, system info, etc.
void mxUtil_StartLogging( mxOutputDevice* logger );

// writes current date and time, performance stats, etc.
void mxUtil_EndLogging( mxOutputDevice* logger );

//--------------------------------------------------------------//
//	Logging helpers.
//--------------------------------------------------------------//

inline mxTextWriter& operator << ( mxTextWriter& stream, const Vec3D& v )
{
	stream.Putf( "%.3f, %.3f, %.3f", v.x, v.y, v.z );
	return stream;
}

//--------------------------------------------------------------//
//	Serializers.
//--------------------------------------------------------------//

//inline AStreamReader & operator >> ( AStreamReader& archive, FileTime& o )
//{
//	return archive.Unpack( o );
//}
//inline AStreamWriter & operator << ( AStreamWriter& archive, const FileTime& o )
//{
//	return archive.Pack( o );
//}


//--------------------------------------------------------------//

//
//	mxLogger_FILE - is an ANSI file logger.
//
class mxLogger_FILE : public mxLogger
{
	FILE *	mFile;

public:
	mxLogger_FILE()
		: mFile( nil )
	{}
	mxLogger_FILE( const char* fileName )
		: mFile( nil )
	{
		this->Open( fileName );
	}
	~mxLogger_FILE()
	{
		this->Close();
	}
	bool Open( const char* filename, bool eraseExisting = true )
	{
		AssertPtr(filename);
		Assert(!this->IsOpen());
		mFile = ::fopen( filename, eraseExisting ? "w" : "a" );
		if( !mFile ) {
			mxErrf("Failed to open log file '%s' for writing", filename);
			return false;
		}
		return true;
	}
	bool IsOpen() const {
		return (mFile != nil);
	}
	virtual void Close() override
	{
		if( this->IsOpen() ) {
			::fflush( mFile );
			::fclose( mFile );
			mFile = nil;
		}
		Assert(!this->IsOpen());
	}
	virtual void Log( ELogLevel level, const char* message, UINT numChars ) override
	{
		Assert(IsOpen());
		if( mFile )
		{
			::fwrite( message, sizeof(message[0]), numChars, mFile );
			::fflush( mFile );
		}
	}
	virtual void VARARGS Logf( ELogLevel level, const char* fmt, ... ) override
	{
		if( mFile )
		{
			char	buffer[ MAX_STRING_CHARS ];
			MX_GET_VARARGS_ANSI( buffer, fmt );
			this->Log( level, buffer, mxStrLenAnsi(buffer) );
		}
	}
	virtual void Flush() override
	{
		if( mFile )
		{
			::fflush( mFile );
		}
	}
};

class FileLogUtil
{
public:
	FileLogUtil()
	{
		OSFileName	logFileName;
		F_Util_ComposeLogFileName( logFileName );
		this->OpenLog( logFileName.ToChars() );
	}
	explicit FileLogUtil( const char* logFileName )
	{
		this->OpenLog( logFileName );
	}
	~FileLogUtil()
	{
		this->CloseLog();
	}

private:
	mxLogger_FILE	fileLog;

private:
	void OpenLog( const char* logFileName )
	{
		bool bOK = fileLog.Open( logFileName );
		if( !bOK ) {
			mxPutf( "Failed to create log file '%s' for writing.\n", logFileName );
			return;
		}

		mxLogManager& logMgr = GetGlobalLogger();
		logMgr.Attach( &fileLog );

		mxUtil_StartLogging( &logMgr );
	}

	void CloseLog()
	{
		mxLogManager& logMgr = GetGlobalLogger();

		mxUtil_EndLogging( &logMgr );

		logMgr.Detach( &fileLog );
	}
};


mxNAMESPACE_END

#endif // ! __MX_LOG_UTIL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
