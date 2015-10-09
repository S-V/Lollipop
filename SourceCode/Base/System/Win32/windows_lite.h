#pragma once

#define CALLBACK								__stdcall
#define WINAPI									__stdcall
#define WINBASEAPI								extern __declspec(dllimport)
#define WINUSERAPI								extern __declspec(dllimport)
#define WINGDIAPI								extern __declspec(dllimport)
#define OPENGLAPI								extern __declspec(dllimport)
#define CONST const
#define INVALID_HANDLE_VALUE					(HANDLE)(-1)
#define PFD_DRAW_TO_WINDOW						4 
#define PFD_DOUBLEBUFFER						1 
#define PFD_SUPPORT_OPENGL						32 
#define PFD_MAIN_PLANE							0 
#define PFD_TYPE_RGBA							0

#define PM_REMOVE								1

#define MAX_PATH								260

/* WND MSG CMD */
#define WM_CREATE								1
#define WM_DESTROY								2
#define SW_SHOW									5
#define WM_SETFOCUS								7
#define WM_KILLFOCUS							8
#define WM_KEYDOWN								256
#define WM_KEYUP								257
#define WM_LBUTTONDOWN							513
#define WM_LBUTTONUP							514
#define WM_RBUTTONDOWN							516
#define WM_RBUTTONUP							517
/* WND STYLE */
#define WS_POPUP	0x80000000
#define WS_VISIBLE	0x10000000
/*System Metrics*/
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
/* FILES */
#define FILE_ATTRIBUTE_DIRECTORY		0x00000010
/* USER DATA*/
#define GWL_USERDATA					(-21)

/* M$ LIB */
#pragma comment (lib, "kernel32.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "gdi32.lib")
#pragma comment (lib, "opengl32.lib")

/* Microsoft TYPE */

typedef const void *HANDLE;

typedef unsigned long DWORD;
typedef DWORD *PDWORD,*LPDWORD;

typedef HANDLE HINSTANCE;

typedef char *PCHAR,*LPCH,*PCH,*NPSTR,*LPSTR,*PSTR;
typedef const char *LPCCH,*PCSTR,*LPCSTR;

typedef int WINBOOL,*PWINBOOL,*LPWINBOOL;
typedef WINBOOL BOOL;
typedef unsigned char BYTE;

typedef const void *PCVOID,*LPCVOID;
typedef void *PVOID,*LPVOID;
typedef char CHAR;
typedef long LONG, *PLONG;
typedef double LONGLONG,DWORDLONG;
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR;
typedef DWORD_PTR *PDWORD_PTR;
typedef unsigned short UHALF_PTR, *PUHALF_PTR;
typedef short HALF_PTR, *PHALF_PTR;
typedef unsigned long HANDLE_PTR;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef int INT;
typedef unsigned int UINT,*PUINT,*LPUINT;
typedef unsigned short WORD;
typedef WORD ATOM;
typedef HANDLE HWND;
typedef HANDLE HDC;
typedef HDC HGLRC;
typedef __int64 INT64, *PINT64;
typedef unsigned __int64 UINT64;

typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(void*);
typedef LRESULT(CALLBACK *WNDPROC)(HANDLE,UINT,WPARAM,LPARAM);

typedef struct tagPIXELFORMATDESCRIPTOR {
	WORD nSize;
	WORD nVersion;
	DWORD dwFlags;
	BYTE iPixelType;
	BYTE cColorBits;
	BYTE cRedBits;
	BYTE cRedShift;
	BYTE cGreenBits;
	BYTE cGreenShift;
	BYTE cBlueBits;
	BYTE cBlueShift;
	BYTE cAlphaBits;
	BYTE cAlphaShift;
	BYTE cAccumBits;
	BYTE cAccumRedBits;
	BYTE cAccumGreenBits;
	BYTE cAccumBlueBits;
	BYTE cAccumAlphaBits;
	BYTE cDepthBits;
	BYTE cStencilBits;
	BYTE cAuxBuffers;
	BYTE iLayerType;
	BYTE bReserved;
	DWORD dwLayerMask;
	DWORD dwVisibleMask;
	DWORD dwDamageMask;
} PIXELFORMATDESCRIPTOR,*PPIXELFORMATDESCRIPTOR,*LPPIXELFORMATDESCRIPTOR; 


typedef struct _WNDCLASSEXA {
	UINT cbSize;
	UINT style;
	WNDPROC lpfnWndProc;
	int cbClsExtra;
	int cbWndExtra;
	HINSTANCE hInstance;
	HANDLE hIcon;
	HANDLE hCursor;
	HANDLE hbrBackground;
	LPCSTR lpszMenuName;
	LPCSTR lpszClassName;
	HANDLE hIconSm;
} WNDCLASSEXA,*LPWNDCLASSEXA,*PWNDCLASSEXA;

typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT,POINTL,*PPOINT,*LPPOINT,*PPOINTL,*LPPOINTL;

typedef struct tagMSG {
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG,*LPMSG,*PMSG;

typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		LONG HighPart;
	} c;
	struct {
		DWORD LowPart;
		LONG HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

#	define STATUS_WAIT_0		(DWORD)0x00000000L  
#	define IGNORE				0       // Ignore signal
#	define INFINITE				0xFFFFFFFF  // Infinite timeout
#	define WAIT_FAILED			(DWORD)0xFFFFFFFF
#	define WAIT_OBJECT_0		((STATUS_WAIT_0) + 0 )

extern "C"
{
	// kernel
	WINBASEAPI DWORD WINAPI GetModuleFileNameA(HINSTANCE,LPSTR,DWORD);
	WINBASEAPI HANDLE WINAPI LoadLibraryA(LPCSTR);
	WINBASEAPI HANDLE WINAPI GetProcAddress(HANDLE, LPCSTR);

	WINBASEAPI LONG WINAPI InterlockedExchange(LONG volatile *, LONG);
	WINBASEAPI LONG WINAPI InterlockedCompareExchange(LONG volatile *, LONG, LONG);

	WINBASEAPI DWORD WINAPI GetCurrentThreadId();
	WINBASEAPI LONG WINAPI InterlockedIncrement(LONG volatile *lpAddend);
	WINBASEAPI LONG WINAPI InterlockedDecrement(LONG volatile *lpAddend);

	WINBASEAPI HANDLE WINAPI CreateThread(HANDLE,DWORD,LPTHREAD_START_ROUTINE,PVOID,DWORD,PDWORD);
	WINBASEAPI BOOL WINAPI SetThreadPriority(HANDLE,INT);
	WINBASEAPI void WINAPI Sleep(DWORD);


	WINBASEAPI HANDLE WINAPI CreateEventA(HANDLE, BOOL, BOOL, HANDLE);
	WINBASEAPI BOOL WINAPI SetEvent(HANDLE);
	WINBASEAPI BOOL WINAPI ResetEvent(HANDLE);
	WINBASEAPI DWORD WINAPI WaitForSingleObject(HANDLE, DWORD);
	WINBASEAPI DWORD WINAPI WaitForMultipleObjects(DWORD, const HANDLE*, BOOL, DWORD);


	WINBASEAPI void WINAPI OutputDebugStringA(LPCSTR lpOutputString);

	//file
	WINBASEAPI HANDLE WINAPI CreateFileA(LPCSTR,DWORD,DWORD,HANDLE,DWORD,DWORD,HANDLE);
	WINBASEAPI BOOL WINAPI CloseHandle(HANDLE);

	WINBASEAPI BOOL WINAPI WriteFile(HANDLE,PCVOID,DWORD,PDWORD,HANDLE);
	WINBASEAPI BOOL WINAPI ReadFile(HANDLE,PVOID,DWORD,PDWORD,HANDLE);

	WINBASEAPI DWORD WINAPI SetFilePointer(HANDLE,LONG,PLONG,DWORD);
	WINBASEAPI DWORD WINAPI GetFileSize(HANDLE,LPDWORD);


	// user32.lib
	WINUSERAPI LRESULT WINAPI DefWindowProcA(HANDLE,UINT,WPARAM,LPARAM);
	WINUSERAPI void WINAPI PostQuitMessage(INT);

	WINUSERAPI BOOL WINAPI	TranslateMessage(const MSG*);
	WINUSERAPI BOOL WINAPI	PeekMessageA(LPMSG,HWND,UINT,UINT,UINT);
	WINUSERAPI LONG WINAPI	DispatchMessageA(const MSG*);
	WINUSERAPI HDC	WINAPI	GetDC(HWND); 
	WINUSERAPI int	WINAPI	ReleaseDC(HWND, HDC);

	WINUSERAPI INT	WINAPI	GetSystemMetrics(INT);
	WINUSERAPI INT	WINAPI	ShowCursor(BOOL bShow);

	WINUSERAPI ATOM WINAPI RegisterClassExA(CONST WNDCLASSEXA*);
	WINUSERAPI HANDLE WINAPI CreateWindowExA(DWORD,LPCSTR,LPCSTR,DWORD,INT,INT,INT,INT,HWND,HANDLE,HINSTANCE,LPVOID);
	WINUSERAPI BOOL WINAPI ShowWindow(HWND,INT);
	WINUSERAPI BOOL WINAPI UpdateWindow(HWND);

	//gdi.lib
	WINGDIAPI INT WINAPI ChoosePixelFormat(HDC,CONST PIXELFORMATDESCRIPTOR*); 
	WINGDIAPI BOOL WINAPI SetPixelFormat(HDC,INT,const PIXELFORMATDESCRIPTOR*);
	WINGDIAPI BOOL WINAPI SwapBuffers(HDC);

	//opengl32.lib
	OPENGLAPI HGLRC WINAPI wglCreateContext(HDC); 
	OPENGLAPI BOOL WINAPI wglMakeCurrent(HDC,HGLRC);
	OPENGLAPI BOOL WINAPI wglDeleteContext(HGLRC); 

	//HELPER
	WINBASEAPI BOOL WINAPI QueryPerformanceCounter(PLARGE_INTEGER);
	WINBASEAPI BOOL WINAPI QueryPerformanceFrequency(PLARGE_INTEGER);

	WINBASEAPI DWORD WINAPI GetTickCount(void);
	WINBASEAPI HANDLE WINAPI GetCurrentProcess(void);
	WINBASEAPI HANDLE WINAPI GetCurrentThread(void);

	WINBASEAPI BOOL WINAPI GetProcessAffinityMask(HANDLE hProcess, PDWORD_PTR lpProcessAffinityMask, PDWORD_PTR lpSystemAffinityMask);
	WINBASEAPI DWORD_PTR WINAPI SetThreadAffinityMask(HANDLE hThread, DWORD_PTR dwThreadAffinityMask);

}//extern "C"
