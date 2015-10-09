/*
=============================================================================
	File:	Log.h
	Desc:	Logging.
=============================================================================
*/

#ifndef __MX_LOG_STREAM_H__
#define __MX_LOG_STREAM_H__

mxNAMESPACE_BEGIN

/*
=================================================================
	A very simple logging device.
	
	Todo:	Push/Pop log levels?
			Overload '<<' for vectors,matrices, etc?
			Pass string sizes to improve speed?
=================================================================
*/

//
//	ELogLevel - specifies the importance of logging information.
//
enum ELogLevel
{
//	LL_All,		// log everything

	LL_Info,	// normal logging level
	LL_Warning,
	LL_Error,

//	LL_None,	// nothing is printed to the log
};

//
//	mxOutputDevice - represents a user output stream.
//
class mxOutputDevice
{
public:

	virtual	void Log( ELogLevel level, const char* message, UINT numChars ) = 0;
	
	virtual	void VARARGS Logf( ELogLevel level, const char* fmt, ... )
	{
		char	buffer[ MAX_STRING_CHARS ];
		UINT	newLength;
		MX_GET_VARARGS_ANSI_X( buffer, fmt, newLength );

		Log( level, buffer, newLength );
	}

	virtual void Flush() {}

	virtual void Close() {}

protected:
	virtual	~mxOutputDevice() {}
};



//
//	mxTextWriter - provides formatted text output.
//
class mxTextWriter {
public:
	explicit mxTextWriter( AStreamWriter& stream );
	virtual ~mxTextWriter();

	// all text writing is redirected to this function
	virtual void Put( const char* s, UINT length );

	/// Outputs a hex address.
	mxTextWriter& operator<< (const void* p);

	/// Outputs a bool.
	mxTextWriter& operator<< (mxBool b);

	/// Outputs a signed char.
	mxTextWriter& operator<< (signed char c);

	/// Outputs an unsigned char.
	//mxTextWriter& operator<< (unsigned char c);

	/// Outputs a string.
	mxTextWriter& operator<< (const char* s);

	/// Outputs a string.
	mxTextWriter& operator<< (const signed char* s);

	/// Outputs a string.
	mxTextWriter& operator<< (const unsigned char* s);

	/// Outputs a short.
	mxTextWriter& operator<< (short s);

	/// Outputs an unsigned short
	mxTextWriter& operator<< (unsigned short s);

	/// Outputs an int
	mxTextWriter& operator<< (int i);

	/// Outputs an unsigned int.
	mxTextWriter& operator<< (unsigned int u);

	/// Outputs a float.
	mxTextWriter& operator<< (float f);

	/// Outputs a double.
	mxTextWriter& operator<< (double d);

	/// Outputs a 64 bit int.
	mxTextWriter& operator<< (INT64 i);

	/// Outputs a 64 bit unsigned int.
	mxTextWriter& operator<< (UINT64 u);

	/// Outputs a String.
	mxTextWriter& operator<< (const String& str);

	mxTextWriter& operator<< (const ConstCharPtr& str);

	/// Outputs formatted data.
	void Putf( const char *fmt, ...);

public_internal:
	AStreamWriter & GetStream() { return mStream; }

protected:
	AStreamWriter &	mStream;

private:
	PREVENT_COPY(mxTextWriter);
};


//
//	mxLogger - base class for logging devices.
//
class mxLogger : public mxOutputDevice, public mxTextWriter
{
public:

	FORCEINLINE void SetLogLevel( ELogLevel newLevel ) {
		mLogLevel = newLevel;
	}

	FORCEINLINE ELogLevel GetLogLevel() const {
		return mLogLevel;
	}

protected:
	mxLogger()
		: mxTextWriter( (AStreamWriter&)(*this) ), mLogLevel( ELogLevel::LL_Info )
	{}
	virtual ~mxLogger()
	{}

private:
	ELogLevel	mLogLevel;	// current print level (logging level threshold)
};

//
//	mxLogManager
//
class mxLogManager : public mxLogger
{
public:
	virtual	~mxLogManager()
	{}
	virtual void Attach( mxOutputDevice* logger ) = 0;
	virtual void Detach( mxOutputDevice* logger ) = 0;
	virtual bool IsRedirectingTo( mxOutputDevice* logger ) = 0;
};

mxLogManager& GetGlobalLogger();

/*
//
//	LogManipulator - controls behaviour of a logging device.
//
class LogManipulator
{
public:
	virtual mxLogger & operator () ( mxLogger& logger ) const = 0;
};

// executes the log manipulator on the logger (the stream)
//
inline mxLogger & operator << ( mxLogger& logger, const LogManipulator& manip )
{
	manip( logger );
	return logger;
}

//
//	LogLevel
//
//	Usage:
//
//	Logger::get_ref() << LogLevel(LL_Info) << "X = " << X << endl;
//
class LogLevel : public LogManipulator
{
	const ELogLevel	mLogLevel;

public:
	LogLevel( ELogLevel ll )
		: mLogLevel( ll )
	{}
	virtual mxLogger & operator () ( mxLogger& logger ) const
	{
		logger.SetLogLevel( mLogLevel );
		return logger;
	};
};

//
//	ScopedLogLevel - automatically restores saved logging level.
//
class ScopedLogLevel
{
	mxLogger &	mLogger;
	ELogLevel	mSavedLogLevel;

public:
	ScopedLogLevel( mxLogger& logger )
		: mLogger( logger ), mSavedLogLevel( logger.GetLogLevel() )
	{}
	~ScopedLogLevel()
	{
		mLogger.SetLogLevel( mSavedLogLevel );
	}
};
*/


mxNAMESPACE_END

#endif // !__MX_LOG_STREAM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
