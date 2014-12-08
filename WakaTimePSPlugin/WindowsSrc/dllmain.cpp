// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "CommonUtility.h"

HINSTANCE currentInstance;

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD ul_reason_for_call,
	LPVOID /*lpReserved*/)
{

	ul_reason_for_call;

#ifdef NEEDS_CRT_INIT
	if (ul_reason_for_call == DLL_PROCESS_ATTACH || ul_reason_for_call == DLL_THREAD_ATTACH)
	{
		__security_init_cookie();
		__crt_dll_initialize();
	}
#endif

	currentInstance = static_cast<HINSTANCE>(hModule);
	return  true;
}