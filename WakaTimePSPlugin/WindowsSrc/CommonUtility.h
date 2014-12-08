#ifndef __WakaTime_Plugin_Common_Utility
#define __WakaTime_Plugin_Common_Utility

#include "stdafx.h"
#include <string>

// The instance of Notepad++ for which the dll is loaded. This is declared
// extern here so that whoever includes this file gets this variable. But 
// the variable itself is declared in dllmain.cpp.
extern HINSTANCE currentInstance;

class EditRecordTimer
{
private:
	~EditRecordTimer(){}
	static FILETIME lastUpdatedTimeStamp; 
	static const long long SPAN = 2 * 60 * 1000; // 2 Minute interval
public:
	static bool HasEnoughTimeElapsedToRecordEdit()
	{
		FILETIME thisInstant;
		GetSystemTimeAsFileTime(&thisInstant);
		// Somewhere in SO there was this comment that FILETIME is laid out in 
		// memory as a long long and reinterpreting it as long long.
		long long delta = *(reinterpret_cast<long long*>(&thisInstant)) - 
							*(reinterpret_cast<long long*>(&lastUpdatedTimeStamp));
		return (delta / 1000 > SPAN);
	}

	static void UpdateTimeStampToBringToCurrent()
	{
		GetSystemTimeAsFileTime(&lastUpdatedTimeStamp);
	}
};

class ManageWakaTimeConfigFile
{
private:
	const std::wstring SECTION = L"settings";
	const std::wstring API_KEY = L"api_key";
	std::wstring m_FileName;
public:
	bool ReadWakaTimeConfigFile();
	std::wstring GetAPIKeyFromConfigFile();
	bool UpdateWakaTimeAPIKey(std::wstring key);
	void CreateWakaTimeConfigFile(std::wstring key);
};

class CommonUtility
{
private:
	static std::wstring sWakaTimeCmdPath;
	static const std::wstring pythoncmd;
public:
	static std::wstring GetCurrentNPPDocument();
	static std::wstring GetNPPConfigDirectory();
	static void OnCurrentNPPDocumentSaved();
	static void OnNewNPPDocumentCreated();
	static void OnNPPDocumentModified();
	static std::wstring GetPythonPath();
	static std::wstring GetCommandPrefix();
};

// One instance of config file manager. 
extern ManageWakaTimeConfigFile gConfigFileManager;

#include "SPAccess.h"
#include "PIUSuites.h"
HINSTANCE GetDLLInstance(SPPluginRef plugin);

extern "C" BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD ul_reason_for_call,
	LPVOID /*lpReserved*/);

#endif