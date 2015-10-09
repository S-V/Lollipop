/*
=============================================================================
	File:	LogUtil.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

#include <Base/Text/TextUtils.h>
#include "LogUtil.h"

mxNAMESPACE_BEGIN

void F_Util_ComposeLogFileName( OSFileName & logFileName )
{
	OSFileName	exeFileName;
	exeFileName.SetString(mxTO_ANSI(mxGetExeFileName()));
	exeFileName.StripFileExtension();

	if (0)
	{
		TStackString<32>	versionString( "[" MX_BUILD_MODE_STRING "]" );
	
		mxStripTrailingCaselessANSI(
			exeFileName.ToChars(), exeFileName.Length(),
			versionString.ToChars(), versionString.Length() );
	}

	logFileName.Format("%s.log",exeFileName.ToChars());
}

void mxUtil_StartLogging( mxOutputDevice* logger )
{

#if MX_DEVELOPER
	logger->Logf( LL_Info,  "Engine init: %u microseconds.\n", (UINT)mxGetTimeInMicroseconds() );
#endif //MX_DEVELOPER

	// log current time
	{
		CalendarTime	localTime( CalendarTime::GetCurrentLocalTime() );

		StackString	timeOfDay;
		GetTimeOfDayString( timeOfDay, localTime.hour, localTime.minute, localTime.second );

		logger->Logf( LL_Info, "=====================================================\n");
		logger->Logf( LL_Info,  "Log started on %s, %s %u%s, at %s.\n",
			CalendarTime::WeekdayToStr( localTime.weekday ),
			CalendarTime::MonthToStr( localTime.month ),
			localTime.day, GetNumberExtension( localTime.day ),
			timeOfDay.ToChars()
			);
		logger->Logf( LL_Info, "=====================================================\n\n");
	}

	// log engine version and game build time stamp

	logger->Logf( LL_Info, "Engine version: %s (%s)\n",
		mxENGINE_VERSION_STRING, (MX_POINTER_SIZE == 4) ? "32-bit" : "64-bit"
	);
	logger->Logf( LL_Info, "Build time: %s\n",mxGetBaseBuildTimestamp());
	logger->Logf( LL_Info, "Compiled with %s\n",MX_COMPILER_NAME);
	logger->Logf( LL_Info, "Machine endianness: %s\n", mxIsLittleEndian() ? "Little-endian" : "Big-endian" );
	logger->Logf( LL_Info, "Character set: %s\n", sizeof(TCHAR)==1 ? ("ANSI") : ("Unicode") );

//	logger->Logf( LL_Info,  "\n==================================\n" );




	// log system info

	mxSystemInfo		sysInfo;
	mxGetSystemInfo( sysInfo );

	logger->Logf( LL_Info,  "OS: %s\n", mxCHARS_AS_ANSI( sysInfo.OSName ) );
	extern bool IsWow64();
	logger->Logf( LL_Info,  "OS type: %s Operating System\n", IsWow64() ? "64-bit" : "32-bit" );
	logger->Logf( LL_Info,  "OS language: \"%s\"\n", mxCHARS_AS_ANSI( sysInfo.OSLanguage ) );
	logger->Logf( LL_Info,  "OS directory: \"%s\"\n", mxCHARS_AS_ANSI( sysInfo.OSDirectory ) );
	logger->Logf( LL_Info,  "System directory: \"%s\"\n", mxCHARS_AS_ANSI( sysInfo.SysDirectory ) );
	logger->Logf( LL_Info,  "Launch directory: \"%s\"\n", mxCHARS_AS_ANSI( mxGetLauchDirectory() ) );
	logger->Logf( LL_Info,  "Executable file name: \"%s\"\n", mxCHARS_AS_ANSI( mxGetExeFileName() ) );
	logger->Logf( LL_Info,  "Command line parameters: %s\n", mxCHARS_AS_ANSI( mxGetCmdLine() ) );
	logger->Logf( LL_Info,  "Computer name: %s\n", mxCHARS_AS_ANSI( mxGetComputerName() ) );
	logger->Logf( LL_Info,  "User name: %s\n", mxCHARS_AS_ANSI( mxGetUserName() ) );

//	logger->Logf( LL_Info,  "\n==================================\n" );

	const mxCpuInfo & cpuInfo = sysInfo.cpu;

	logger->Logf( LL_Info,  "CPU name: %s (%s), vendor: %s, family: %u, model: %u, stepping: %u\n",
		cpuInfo.brandName, mxGetCpuTypeName( cpuInfo.type ), cpuInfo.vendor, cpuInfo.family, cpuInfo.model, cpuInfo.stepping );

	const UINT numCores = cpuInfo.numPhysicalCores;
	logger->Logf( LL_Info,  "CPU cores: %u, clock frequency: %u MHz (estimated: ~%u MHz), page size = %u bytes\n",
				numCores, (UINT)cpuInfo.readFreqMHz, UINT(cpuInfo.estimatedFreqHz/(1000*1000)), (UINT)cpuInfo.pageSize );

	logger->Logf( LL_Info, "Cache line size: %u bytes\n", cpuInfo.cacheLineSize );
	logger->Logf( LL_Info, "L1 Data Cache: %u x %u KiB, %u-way associative\n", numCores, cpuInfo.L1_DCache_Size/numCores, cpuInfo.L1_Assoc );
	logger->Logf( LL_Info, "L1 Instruction Cache: %u x %u KiB, %u-way associative\n", numCores, cpuInfo.L1_ICache_Size/numCores, cpuInfo.L1_Assoc );

	logger->Logf( LL_Info, "L2 Cache: %u x %u KiB, %u-way associative\n", numCores, cpuInfo.L2_Cache_Size/numCores, cpuInfo.L2_Assoc );

	if( cpuInfo.L3_Cache_Size && cpuInfo.L3_Assoc )
	{
		logger->Logf( LL_Info, "L3 Cache: %u KiB, %u-way associative\n", cpuInfo.L3_Cache_Size, cpuInfo.L3_Assoc );
	}

	logger->Logf( LL_Info, "CPU features:\n");
	//if( cpuInfo.has_CPUID ) {
	//	logger->Logf( LL_Info, "CPUID supported\n");
	//}
	if( cpuInfo.has_RDTSC ) {
		logger->Logf( LL_Info, "	RDTSC\n");
	}
	if( cpuInfo.has_CMOV ) {
		logger->Logf( LL_Info, "	CMOV\n");
	}
	if( cpuInfo.has_MOVBE ) {
		logger->Logf( LL_Info, "	MOVBE\n");
	}
	if( cpuInfo.has_FPU ) {
		logger->Logf( LL_Info, "	FPU: On-Chip\n");
	}
	if( cpuInfo.has_MMX ) {
		logger->Logf( LL_Info, "	MMX\n");
	}
	if( cpuInfo.has_MMX_AMD ) {
		logger->Logf( LL_Info, "	MMX (AMD extensions)\n");
	}
	if( cpuInfo.has_3DNow ) {
		logger->Logf( LL_Info, "	3D Now!\n");
	}
	if( cpuInfo.has_3DNow_Ext ) {
		logger->Logf( LL_Info, "	3D Now! (AMD extensions)\n");
	}
	if( cpuInfo.has_SSE_1 ) {
		logger->Logf( LL_Info, "	SSE\n");
	}
	if( cpuInfo.has_SSE_2 ) {
		logger->Logf( LL_Info, "	SSE2\n");
	}
	if( cpuInfo.has_SSE_3 ) {
		logger->Logf( LL_Info, "	SSE3\n");
	}
	if( cpuInfo.has_SSE_4_1 ) {
		logger->Logf( LL_Info, "	SSE4.1\n");
	}
	if( cpuInfo.has_SSE_4_2 ) {
		logger->Logf( LL_Info, "	SSE4.2\n");
	}
	if( cpuInfo.has_SSE_4a ) {
		logger->Logf( LL_Info, "	SSE4a\n");
	}
	if( cpuInfo.has_POPCNT ) {
		logger->Logf( LL_Info, "	POPCNT\n");
	}
	if( cpuInfo.has_FMA ) {
		logger->Logf( LL_Info, "	FMA\n");
	}
	if( cpuInfo.has_AES ) {
		logger->Logf( LL_Info, "	AES\n");
	}
	if( cpuInfo.has_AVX ) {
		logger->Logf( LL_Info, "	AVX\n");
	}


	if( cpuInfo.has_HTT ) {
		logger->Logf( LL_Info, "detected: Hyper-Threading Technology\n");
	}
	if( cpuInfo.has_EM64T ) {
		logger->Logf( LL_Info, "64-bit Memory Extensions supported\n");
	}


	logger->Logf( LL_Info,  "Logical CPU cores: %u\n",
		(UINT)cpuInfo.numLogicalCores );

//	logger->Logf( LL_Info,  "\n==================================\n" );

	// log memory stats

	mxMemoryStatus initialMemoryStatus;
	mxGetExeLaunchMemoryStatus(initialMemoryStatus);
	logger->Logf( LL_Info,  "%u MB physical memory installed, %u MB available, %u MB virtual memory installed, %u percent memory in use\n",
		initialMemoryStatus.totalPhysical,
		initialMemoryStatus.availPhysical,
		initialMemoryStatus.totalVirtual,
		initialMemoryStatus.memoryLoad
	);
	logger->Logf( LL_Info,  "Page file: %u MB used, %u MB available\n",
		initialMemoryStatus.totalPageFile - initialMemoryStatus.availPageFile,
		initialMemoryStatus.availPageFile
	);


	mxPERM("determine bus speed");

//	logger->Logf( LL_Info,  "\n==================================\n" );

	// Log basic info.

	logger->Logf( LL_Info,  "Initialization took %u milliseconds.\n\n", UINT(mxGetTimeInMicroseconds()/1000) );
}

void mxUtil_EndLogging( mxOutputDevice* logger )
{
#if MX_DEBUG_MEMORY
	// Dump memory leaks if needed.
	if(!mxIsInDebugger())
	{
		//mxMemoryStatistics	globalMemStats;
		F_DumpGlobalMemoryStats( *logger );
	}
#endif

	// Dump stats if needed.

#if MX_ENABLE_PROFILING
	
	logger->Logf( LL_Info, "\n--- BEGIN METRICS --------------------\n");
	mxProfileManager::dumpAll( logger );
	logger->Logf( LL_Info, "\n--- END METRICS ----------------------\n");

#endif // MX_ENABLE_PROFILING



	// log current time
	{
		CalendarTime	localTime( CalendarTime::GetCurrentLocalTime() );

		StackString	timeOfDay;
		GetTimeOfDayString( timeOfDay, localTime.hour, localTime.minute, localTime.second );

		logger->Logf( LL_Info, "\n=====================================================\n");
		logger->Logf( LL_Info, "Log ended on %s, %s %u%s, at %s.\n",
			CalendarTime::WeekdayToStr( localTime.weekday ),
			CalendarTime::MonthToStr( localTime.month ),
			localTime.day, GetNumberExtension( localTime.day ),
			timeOfDay.ToChars()
		);

		const U8 totalRunningTime = mxGetTimeInMicroseconds();
		U4 hours, minutes, seconds;
		ConvertMicrosecondsToHoursMinutesSeconds( totalRunningTime, hours, minutes, seconds );
		logger->Logf( LL_Info, "Program running time: %u hours, %u minutes, %u seconds.\n",
			hours, minutes, seconds
		);

		logger->Logf( LL_Info, "=====================================================\n\n");
	}

}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
