/*
=============================================================================
	File:	Platform.h
	Desc:	System-level unportable services,
			preprocessor definitions for platform, compiler, etc.
=============================================================================
*/

#ifndef __MX_PLATFORM_H__
#define __MX_PLATFORM_H__

/*
=======================================================================

	Platform-specific services.

=======================================================================
*/
mxNAMESPACE_BEGIN

	// Functions used to initialize/shutdown platform layer.

	void	mxPlatform_Init();
	void	mxPlatform_Shutdown();


	enum EPlatform
	{
		Platform_Unknown = 0,

		Platform_Win32,
		Platform_Win64,
		Platform_Linux,

		Platform_Mac,
		Platform_IPhone,

		Platform_PS3,
		Platform_XBox360,
		Platform_Wii,
		Platform_GameCube,
	};

	enum ECompilerId
	{
		Compiler_Unknown = 0,
		Compiler_MSVCpp,
		Compiler_Intel, 
		Compiler_GCC,
		Compiler_Metrowerks,
		Compiler_Codeplay,
		Compiler_Borland,
	};

	enum ECPUArchType
	{
		Architecture_Unknown = 0,
		Architecture_IA32,
		Architecture_IA64,
		Architecture_ARM,
		Architecture_PPC,
	};

	// byte sex
	enum EEndianness
	{
		BigEndian = 4321,
		LittleEndian = 1234,	// Lowest address contains the least significant byte.
		//PDP_Endian = 3412
	};

mxNAMESPACE_END



//
// Identify the current platform.
//

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN32_WCE)
#	define MX_PLATFORM_WIN32	1
#	define MX_PLATFORM_WIN64	0
#	define MX_PLATFORM_LINUX	0
#elif defined(_WIN64) || defined(WIN64)
#	define MX_PLATFORM_WIN32	0
#	define MX_PLATFORM_WIN64	1
#	define MX_PLATFORM_LINUX	0
#else
#	error Unsupported platform !
#endif

//
// Find the current compiler.
//

#if defined(_MSC_VER)
#	define MX_COMPILER_MSVC
#elif defined(__INTEL_COMPILER)
#	define MX_COMPILER_INTEL
#elif defined(__MINGW32__)
#	define MX_COMPILER_GCC
#elif defined(__MWERKS__)
#	define MX_COMPILER_MWERKS
#else
#	error Unknown compiler !
#endif

//
// Find CPU architecture type.
//

#if defined(_MSC_VER) && defined(_M_IX86)

#	define MX_ARCH_IA32					1
#	define MX_CPU_X86					1
#	define MX_LITTLE_ENDIAN				1
#	define MX_BIG_ENDIAN				0
#	define MX_POINTER_SIZE				4

#elif defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) ||  defined(__s390__) || defined(__s390x__)

#	define MX_ARCH_IA64					1
#	define MX_CPU_X86					1
#	define MX_LITTLE_ENDIAN				1
#	define MX_BIG_ENDIAN				0
#	define MX_POINTER_SIZE				8

#else

#	error "Unknown processor architecture!"

#endif

//-------------------------------------------------------------------------------
// Include platform-specific headers.
//-------------------------------------------------------------------------------

#if (MX_PLATFORM_WIN32) && defined(MX_COMPILER_MSVC)
	#include <Base/System/Win32/Win32_MVCpp.h>
#elif (MX_PLATFORM_WIN64) && defined(MX_COMPILER_MSVC)
	#include <Base/System/Win64/Win64_MVCpp.h>
#elif (MX_PLATFORM_WIN32) && defined(MX_COMPILER_INTEL)
	#include <Base/System/Win32/Win32_IntelCC.h>
#elif (MX_PLATFORM_LINUX) && defined(MX_COMPILER_GCC)
	#include <Base/System/Linux/Linux_GCC.h>
#else
#	error Unsupported compiler/platform !
#endif

/*
=======================================================================
	
	Non-portable system services.

=======================================================================
*/

mxNAMESPACE_BEGIN

enum ECoreMsgCode
{
	Msg_Unknown = 0,	// no error
	Msg_InvalidParams,	// method called with invalid parameter(s)
	Msg_InvalidOp,		// method was called at a time when an operation is not possible
	Msg_OutOfMemory,	// failed to allocate some memory
	Msg_Internal,		// internal error
	Msg_Assertion,		// an assertion failed
	Msg_Info,			// an informational message
	Msg_Warning,		// a warning message
};

//---------------------------------------------------------------------
//		Debug output and error handling
//---------------------------------------------------------------------

				// Prints a message to the text console.
void			mxPut( const char* str );
void VARARGS	mxPutf( const char* fmt, ... ) mxCHECK_PRINTF_ARGS;

				// Shows a message box.
void VARARGS	mxMsgBoxf( PCSTR fmt, ... ) mxCHECK_PRINTF_ARGS;

				// Prints a message to the debug console.
void			DebugOut( const char* message );

				// Shows a message box with a warning message.
void			mxWarn( const char* str );
void VARARGS	mxWarnf( const char* fmt, ... ) mxCHECK_PRINTF_ARGS;

				// Shows a error message box; should be used for media or logic errors.
				// Doesn't terminate execution.
				// Causes a breakpoint exception to occur in debug builds.
void			mxErr( const char* str );
void VARARGS	mxErrf( const char* fmt, ... ) mxCHECK_PRINTF_ARGS;

				// Fatal error - terminate execution immediately;
				// does not return;
				// may write something into the error log.
				// Causes a breakpoint exception to occur in debug builds.
void			mxFatal( const char* str );
void VARARGS	mxFatalf( const char* fmt, ... ) mxCHECK_PRINTF_ARGS;


bool mxYesNoMsgBoxf( const char* msg, const char* caption );

//---------------------------------------------------------------------
//		General system info
//---------------------------------------------------------------------

//---------------------------------------------------------------------------
//	ENDIANNESS
//---------------------------------------------------------------------------
// Extra comments by Kenny Hoff:
// Determines the byte ordering of the current machine (little or big endian)
// by setting an integer value to 1 (so least significant bit is now 1); take
// the address of the int and cast to a byte pointer (treat integer as an
// array of four bytes); check the value of the first byte (must be 0 or 1).
// If the value is 1, then the first byte least significant byte and this
// implies LITTLE endian. If the value is 0, the first byte is the most
// significant byte, BIG endian. Examples:
//      integer 1 on BIG endian: 00000000 00000000 00000000 00000001
//   integer 1 on LITTLE endian: 00000001 00000000 00000000 00000000
// Intel/AMD chips tend to be little endian, MAC PPC's and Suns are big endian.
//---------------------------------------------------------------------------
FORCEINLINE bool mxIsLittleEndian()
{
	INT32 i = 1;
	return *((UINT8*) & i) != 0;
}

FORCEINLINE bool mxIsBigEndian()
{
	return !mxIsLittleEndian();
}

//---------------------------------------------------------------------
//		Platform information
//---------------------------------------------------------------------

EPlatform mxCurrentPlatform();
const char* mxGetPlatformName( EPlatform platformType );

//---------------------------------------------------------------------
//		CPU information
//---------------------------------------------------------------------

enum ECpuType
{
	CPU_Unknown = 0,	// Any unrecognized processor.
	CPU_x86_32,
	CPU_x86_64,
};

const char* mxGetCpuTypeName( ECpuType CpuType );

enum ECpuVendor
{
	CpuVendor_Unknown,
	CpuVendor_AMD,
	CpuVendor_Intel,
};

enum ECpuFamily
{
	CpuFamily_Unknown		= 0,
	CpuFamily_Pentium		= 5,
	CpuFamily_PentiumII		= 6,
	CpuFamily_PentiumIII	= 6,
	CpuFamily_PentiumIV		= 15,
};

enum CpuCacheType
{
	CpuCache_Unified,
	CpuCache_Instruction,
	CpuCache_Data,
	CpuCache_Trace,

	NumCpuCacheTypes
};

struct CpuCacheInfo
{
	DWORD			size;
	UWORD			lineSize;
	UBYTE			level;
	UBYTE			associativity;	// 0xFF if fully associative
	CpuCacheType	type;
};

// Processor information.
//
struct mxCpuInfo
{
	char	brandName[49];	// Processor Brand String.
	char	vendor[13];		// CPU vendor string.
	INT		brandId;		// CPU brand index.
	INT		family;			// ECpuFamily.
	INT		model;			// CPU model.
	INT		stepping;		// Stepping ID.

	INT		extendedFamily;
	INT		extendedModel;

	ECpuType	type;

	UINT	numLogicalCores;// number of CPU cores
	UINT	numPhysicalCores;

	UINT	pageSize;		// CPU page size, in bytes
	UINT	cacheLineSize;	// cache line size, in bytes

	// cache sizes in 1K units
	UINT	L1_ICache_Size, L1_DCache_Size;
	UINT	L2_Cache_Size, L3_Cache_Size;

	// cache associativity
	UINT	L1_Assoc, L2_Assoc, L3_Assoc;

	// CPU clock frequency
	UINT32	readFreqMHz;	// value read from OS registry, in megahertz
	UINT64	estimatedFreqHz;// roughly estimated CPU clock speed, in hertz

	// CPU features
	bool	has_CPUID;
	bool	has_RDTSC;
	bool	has_FPU;	// On-Chip FPU
	bool	has_MMX;	// Multimedia Extensions
	bool	has_MMX_AMD;// AMD Extensions to the MMX instruction set
	bool	has_3DNow;	// 3DNow!
	bool	has_3DNow_Ext;// AMD Extensions to the 3DNow! instruction set
	bool	has_SSE_1;	// Streaming SIMD Extensions
	bool	has_SSE_2;	// Streaming SIMD Extensions 2
	bool	has_SSE_3;	// Streaming SIMD Extensions 3 aka Prescott's New Instructions
	bool	has_SSE_4_1;// Streaming SIMD Extensions 4.1
	bool	has_SSE_4_2;// Streaming SIMD Extensions 4.2
	bool	has_SSE_4a;	// AMD's SSE4a extensions
	bool	has_HTT;	// Hyper-Threading Technology
	bool	has_CMOV;	// Conditional Move and fast floating point comparison (FCOMI) instructions
	bool	has_EM64T;	// 64-bit Memory Extensions (64-bit registers, address spaces, RIP-relative addressing mode)
	bool	has_MOVBE;	// MOVBE (this instruction is unique to Intel Atom processor)
	bool	has_FMA;	// 256-bit FMA (Intel)
	bool	has_POPCNT;	// POPCNT
	bool	has_AES;	// AES support (Intel)
	bool	has_AVX;	// 256-bit AVX (Intel)
};

SizeT	mxGetNumCpuCores();	// shortcut for convenience

//---------------------------------------------------------------------
//		FPU
//---------------------------------------------------------------------

enum FPU_Exceptions_e
{
	FPU_EXCEPTION_INVALID_OPERATION		= 1,
	FPU_EXCEPTION_DENORMALIZED_OPERAND	= 2,
	FPU_EXCEPTION_DIVIDE_BY_ZERO		= 4,
	FPU_EXCEPTION_NUMERIC_OVERFLOW		= 8,
	FPU_EXCEPTION_NUMERIC_UNDERFLOW		= 16,
	FPU_EXCEPTION_INEXACT_RESULT		= 32
};

enum FPU_Precision_e
{
	FPU_PRECISION_SINGLE			= 0,
	FPU_PRECISION_DOUBLE			= 1,
	FPU_PRECISION_DOUBLE_EXTENDED	= 2
};

enum FPU_Rounding_e
{
	FPU_ROUNDING_TO_NEAREST		= 0,
	FPU_ROUNDING_DOWN			= 1,
	FPU_ROUNDING_UP				= 2,
	FPU_ROUNDING_TO_ZERO		= 3
};

//
// FP_Class - Floating-point class of a number.
//
enum FP_Class
{
	FP_SNAN,		// Signaling NaN.
	FP_QNAN,		// Quiet NaN.
	FP_NINF,		// Negative infinity ( –INF ).
	FP_NN,			// Negative normalized non-zero.
	FP_ND,			// Negative denormalized.
	FP_NZ,			// Negative zero ( –0 ).
	FP_PZ,			// Positive 0 ( +0 ).
	FP_PD,			// Positive denormalized.
	FP_PN,			// Positive normalized non-zero.
	FP_PINF,		// Positive infinity ( +INF ).
};

FP_Class	GetFPClass( const double x );
FASTBOOL	IsNaN( const double x );
FASTBOOL	IsFinite( const double x );

//
//	FPU_Control - can be used to enabe / disable FPU exceptions.
//
struct FPU_Control
{
	enum EDebugMask
	{
		FPU_DEBUG_ZERODIVIDE = BIT(0),
		FPU_DEBUG_INVALID	= BIT(1),
		FPU_DEBUG_OVERFLOW	= BIT(2),
		FPU_DEBUG_UNDERFLOW	= BIT(3),
		FPU_DEBUG_DENORMAL	= BIT(4),
	};

	// Pass in a mask of FPU exceptions that you want to break on.
	void EnableFpuDebug( EDebugMask mask );

	// Disables FPU exception handling.
	void DisableFpuDebug();

	// TODO: add precision control, rounding control, etc
};

//
//	SystemInfo
//
struct mxSystemInfo
{
	EPlatform		platform;
	const mxChar *	OSName;
	const mxChar *	OSDirectory;
	const mxChar *	SysDirectory;
	const mxChar *	OSLanguage;

	mxCpuInfo		cpu;
};

void mxGetSystemInfo( mxSystemInfo &OutInfo );

//---------------------------------------------------------------------
//		Memory usage statistics
//---------------------------------------------------------------------

// Returns the total amount of system (physical) RAM, in megabytes.
SizeT mxGetSystemRam();

struct mxMemoryStatus
{
	// Number between 1 and 100 that specifies
	// the approximate percentage of physical memory  that is in use.
	UINT 	memoryLoad;

	SizeT 	totalPhysical;	// Total size of physical memory, in megabytes.
	SizeT 	availPhysical;	// Size of physical memory available, in megabytes.
	SizeT 	totalPageFile;	// Size of commited memory limit, in megabytes.
	SizeT 	availPageFile;

	// Total size of the user mode portion
	// of the virtual address space of the calling process, inmega bytes.
	SizeT 	totalVirtual;
	
	// Size of unreserved and uncommitted memory in the user mode portion
	// of the virtual address space of the calling process, in megabytes.
	SizeT 	availVirtual;
};

// Returns memory stats.
void mxGetCurrentMemoryStatus( mxMemoryStatus & OutStats );
void mxGetExeLaunchMemoryStatus( mxMemoryStatus & OutStats );

// This function is provided for convenience.
void mxGetMemoryInfo( SizeT &TotalBytes, SizeT &AvailableBytes );

//---------------------------------------------------------------------
//		DLL-related functions.
//---------------------------------------------------------------------

void * mxDLLOpen( const char* moduleName );
void   mxDLLClose( void* module );
void * mxDLLGetSymbol( void* module, const char* symbolName );

//---------------------------------------------------------------------
//		System time
//---------------------------------------------------------------------

// Returns the number of milliseconds elapsed since the last reset.
// NOTE: Should only be used for profiling, not for game timing!
UINT	mxGetMilliseconds();

// These functions return the amount of time elapsed since the application started.
mxUInt64 mxGetTimeInMicroseconds();
mxFloat64 mxGetTimeInSeconds();

// These functions can be used for accurate performance testing,
// but not for in-game timing.
//
UINT	mxGetClockTicks();	// Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days.
UINT	mxGetClockTicksPerSecond();	// CPU frequency in Hertz

mxReal	mxGetClockTicks_F();
mxReal	mxGetClockTicksPerSecond_F();

//
//	CalendarTime
//
class CalendarTime {
public:
    enum Month 
    {
        January = 1,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December,
    };
    enum Weekday
    {
        Sunday = 0,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
    };

	static const char * MonthToStr( Month e );
	static const char * WeekdayToStr( Weekday e );

public:
	UINT		year;		/* years since 1900 */
    Month		month;		/* months since January - [1,12] */
    Weekday		weekday;	/* days since Sunday - [0,6] */
    UINT		day;		/* day of the month - [1,31] */
    UINT		hour;		/* hours since midnight - [0,23] */
    UINT		minute;		/* minutes after the hour - [0,59] */
    UINT		second;		/* seconds after the minute - [0,59] */
    UINT		milliSecond;

public:
	// Obtains the current system time. This does not depend on the current time zone.
	static CalendarTime	GetCurrentSystemTime();

	// Obtains the current local time (with time-zone adjustment).
	static CalendarTime	GetCurrentLocalTime();
};

// Returns the exact local time.
//
void mxGetTime( UINT & year, UINT & month, UINT & dayOfWeek,
			 UINT & day, UINT & hour, UINT & minute,
			 UINT & second, UINT & milliseconds );



//---------------------------------------------------------------------
//		Miscellaneous
//---------------------------------------------------------------------


mxTODO("these should return ConstWChar")

// Returns the absolute file path name of the application executable.
// (including the trailing slash, e.g. "D:/MyCoolGame/Bin/")
const mxChar* mxGetLauchDirectory();

// Returns the fully qualified filename for the currently running executable.
const mxChar* mxGetExeFileName();

const mxChar* mxGetCmdLine();

const mxChar* mxGetComputerName();
const mxChar* mxGetUserName();

// Sleep for a specified amount of seconds.
void	mxSleep( FLOAT seconds );

//Causes the current thread to sleep for usec microseconds.
void	mxSleepMicroseconds( ULONG usec );

//Causes the current thread to sleep for msecs milliseconds.
void	mxSleepMilliseconds( ULONG msecs );

// Returns the percentage of current CPU load.
UINT	mxGetCpuUsage();

//
//	Emits a simple sound through the speakers.
//	(This can be used for debugging or waking you up when the whole thing compiles...)
//
//	Param[In]: delayMs - duration of the sound, in milliseconds.
//	Param[In]: frequencyInHertz - frequency of the sound, in hertz
//
void	mxBeep( UINT delayMs, UINT frequencyInHertz = 600 );

//
// Mouse cursor control.
//
void	mxSetMouseCursorVisible( bool bVisible );	// show/hide mouse cursor
void	mxSetMouseCursorPosition( UINT x, UINT y ); // coordinates are absolute, not relative!

void	mxGetCurrentDeckstopResolution( UINT &ScreenWidth, UINT &ScreenHeight );

void	mxWaitForKeyPress();

// Causes a crash to occur. Use only for testing & debugging!
void	mxCrash();


mxNAMESPACE_END

#endif // !__MX_PLATFORM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
