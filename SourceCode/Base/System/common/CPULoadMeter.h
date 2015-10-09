/*
=============================================================================
	File:	CPULoadMeter.cpp
	Desc:
=============================================================================
*/

#ifndef MxCPULoadMeter_h 
#define MxCPULoadMeter_h 

#if !defined(_KERNEL32_)
#define WINBASEAPI DECLSPEC_IMPORT
#else
#define WINBASEAPI
#endif

#define OUT

#ifdef WIN32
	#ifndef WINAPI
	#define WINAPI __stdcall
	#endif
#else
#	error Unimpl
#endif

mxNAMESPACE_BEGIN

mxSWIPED("muscle: https://public.msli.com/lcs/muscle/");

/** This class knows how to measure the total load on the host computer's CPU.
  * Note that the internal implementation of this class is OS-specific, and so
  * it will only work properly on the OS's for which an implementation has been
  * provided (currently Windows, MacOS/X, and Linux).  Under other OS's, 
  * GetCPULoad() will always just return a negative value.
  *
  * To use this class, just instantiate a CPULoadMeter object, and then call
  * GetCPULoad() every so often (e.g. whenever you want to update your CPU load display)
  */
class CPULoadMeter
{
public:
   /** Default constructor */
   CPULoadMeter();

   /** Destructor. */
   ~CPULoadMeter();

   /** Returns the percentage CPU load, measured since the last time this method was called.
     * @note Currently this method is implemented only for Linux, OS/X, and Windows.
     *       For other operating systems, this method will always return a negative value.
     * @returns 0.0f if the CPU was idle, 1.0f if the CPU was fully loaded, or something
     *          in between.  Returns a negative value if the CPU time could not be measured.
     */
   float GetCPULoad();

private:
   float CalculateCPULoad(UINT64 idleTicks, UINT64 totalTicks); 

   UINT64 _previousTotalTicks;
   UINT64 _previousIdleTicks;

#ifdef WIN32
# if defined(MUSCLE_USING_NEW_MICROSOFT_COMPILER)
// we will use the statically linked version
# else
#  define USE_KERNEL32_DLL_FOR_GETSYSTEMTIMES 1
# endif
#endif

#ifdef USE_KERNEL32_DLL_FOR_GETSYSTEMTIMES
   //typedef WINBASEAPI BOOL WINAPI (*GetSystemTimesProc) (OUT LPFILETIME t1, OUT LPFILETIME t2, OUT LPFILETIME t3);
   typedef BOOL (*GetSystemTimesProc) (LPFILETIME t1, LPFILETIME t2, LPFILETIME t3);
   GetSystemTimesProc _getSystemTimesProc;
   HMODULE _winKernelLib;
#endif
};

mxNAMESPACE_END

#endif
