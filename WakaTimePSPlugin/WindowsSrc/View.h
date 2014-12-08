#ifndef __WakaTime_Plugin_View
#define __WakaTime_Plugin_View

#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <string>

#include "resource.h"
#include "INIReader.h"
#include "CommonUtility.h"

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

const std::wstring licenseAgreement(
	L"Redistribution and use in source and binary forms, with or without"
	L"modification, are permitted provided that the following conditions are met :"
	L"*Redistributions of source code must retain the above copyright"
	L"notice, this list of conditions and the following disclaimer.\r\n"
	L"* Redistributions in binary form must reproduce the above copyright"
	L"notice, this list of conditions and the following disclaimer"
	L"in the documentation and / or other materials provided"
	L"with the distribution.\r\n"
	L"* Neither the names of Wakatime or WakaTime, nor the names of its"
	L"contributors may be used to endorse or promote products derived"
	L"from this software without specific prior written permission.\r\n"
	L"THIS SOFTWARE AND DOCUMENTATION IS PROVIDED BY THE COPYRIGHT HOLDERS AND"
	L"CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT"
	L"NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR"
	L"A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER"
	L"OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,"
	L"EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,"
	L"PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR"
	L"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF"
	L"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING"
	L"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS"
	L"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.");

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hEdit = GetDlgItem(hDlg, IDC_WAKATIME_KEY);
		gConfigFileManager.ReadWakaTimeConfigFile();
		SetDlgItemTextW(hDlg, IDC_WAKATIME_KEY,
			gConfigFileManager.GetAPIKeyFromConfigFile().c_str());
		SetFocus(hEdit);
		return FALSE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			// Handle cancel by sending the close windows message.
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;
		case IDOK:
		{
			// Write user provided Key to the config file with full trust.
			HWND hEdit = GetDlgItem(hDlg, IDC_WAKATIME_KEY);
			WCHAR key[MAX_PATH];
			GetDlgItemTextW(hDlg, IDC_WAKATIME_KEY, key, MAX_PATH);
			gConfigFileManager.UpdateWakaTimeAPIKey(key);
			DestroyWindow(hDlg);
			return TRUE;
		}
		case IDC_WAKATIME_KEY:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				HWND hOk = GetDlgItem(hDlg, IDOK);
				EnableWindow(hOk, true);
				return TRUE;
			}

			return FALSE;
		}
		break;

	case WM_CLOSE:
	{
		// Close will close the dialog to enter WakaTime API key.
		DestroyWindow(hDlg);
		return TRUE;
	}
		
	case WM_DESTROY:
	{
		// Handle any processing that needs to be done before quitting.
		return TRUE;
	}
	}

	return FALSE;
}

#include "SPPlugs.h"
int ShowWakaTimeAPIKeyDialog()
{
	try
	{
		HWND hSelf = ::CreateDialogParam(currentInstance,
			MAKEINTRESOURCE(IDD_WAKATIME_API_DLG), GetActiveWindow(),
			(DLGPROC)DialogProc, (LPARAM)NULL);
		if (hSelf == NULL)
		{
			::MessageBox(NULL, TEXT("WakaTime API Key dialog could not be created!"), 
				TEXT("WakaTime Notepad++ Plugin"), MB_ICONEXCLAMATION);
			return -1;
		}

		RECT rc;
		::GetClientRect(GetActiveWindow(), &rc);
		POINT center;
		int w = rc.right - rc.left;
		int h = rc.bottom - rc.top;
		center.x = rc.left + w / 2;
		center.y = rc.top + h / 2;
		::ClientToScreen(GetActiveWindow(), &center);



		RECT dlgRect;
		::GetClientRect(hSelf, &dlgRect);
		int x = center.x - (dlgRect.right - dlgRect.left) / 2;
		int y = center.y - (dlgRect.bottom - dlgRect.top) / 2;
		::SetWindowPos(hSelf, HWND_TOP, x, y, (dlgRect.right - dlgRect.left), 
			(dlgRect.bottom - dlgRect.top), SWP_SHOWWINDOW);
		::ShowWindow(hSelf, SW_SHOW);
		return 0;
	}
	catch (...)
	{
		// Don't let the NPP runtime issue a cryptic dialog bar that our plugin 
		// has failed. Rather fail gracefully.
		::MessageBox(NULL, TEXT("WakaTime API Key dialog could not be created!"), 
			TEXT("WakaTime Notepad++ Plugin"), MB_ICONEXCLAMATION);
		return -1;
	}
}

INT_PTR CALLBACK AbtDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		SetDlgItemText(hDlg, IDC_EDIT1, licenseAgreement.c_str());
		return TRUE;
	}

	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDOK)
	{
		DestroyWindow(hDlg);
		return TRUE;
	}
	
	return FALSE;
}

int ShowWakaTimeAbtDialog(SPPluginRef plugin, int aboutID)
{
	try
	{
		HWND hSelf = ::CreateDialogParam(currentInstance,
			MAKEINTRESOURCE(IDD_ABOUT_DLG), GetActiveWindow(),
			(DLGPROC)AbtDialogProc, (LPARAM)NULL);
		
		if (hSelf == NULL)
		{
			::MessageBox(NULL, TEXT("WakaTime about dialog could not be created!"),
				TEXT("WakaTime Notepad++ Plugin"), MB_ICONEXCLAMATION);
			return -1;
		}
		
		RECT rc;
		::GetClientRect(GetActiveWindow(), &rc);
		POINT center;
		int w = rc.right - rc.left;
		int h = rc.bottom - rc.top;
		center.x = rc.left + w / 2;
		center.y = rc.top + h / 2;
		::ClientToScreen(GetActiveWindow(), &center);



		RECT dlgRect;
		::GetClientRect(hSelf, &dlgRect);
		int x = center.x - (dlgRect.right - dlgRect.left) / 2;
		int y = center.y - (dlgRect.bottom - dlgRect.top) / 2;
		::SetWindowPos(hSelf, HWND_TOP, x, y, (dlgRect.right - dlgRect.left),
			(dlgRect.bottom - dlgRect.top), SWP_SHOWWINDOW);
		
		::ShowWindow(hSelf, SW_SHOW);
		return 0;
	}
	catch (...)
	{
		// Don't let the NPP runtime issue a cryptic dialog bar that our plugin 
		// has failed. Rather fail gracefully.
		::MessageBox(NULL, TEXT("WakaTime about dialog could not be created!"),
			TEXT("WakaTime Notepad++ Plugin"), MB_ICONEXCLAMATION);
		return -1;
	}
}

#endif