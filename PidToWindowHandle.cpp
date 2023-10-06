#include "PidToWindowHandle.hpp"

static HWND s_winHandleRef = NULL;

namespace NsCascadeProcWin {

	// [https://stackoverflow.com/a/20730976/13951505]
	BOOL CALLBACK windowEnumProc(const HWND p_winHandle, const LPARAM p_lParam) {
		DWORD procId;
		GetWindowThreadProcessId(p_winHandle, &procId);

		// If-else guard clause!:
		if (procId != p_lParam)
			return TRUE;

		s_winHandleRef = p_winHandle;
		return FALSE;
	}

	BOOL getWindowHandleFromPid(const DWORD p_procId, HWND& p_winHandle) {
		const BOOL status = EnumWindows([](const HWND p_winHandle, const LPARAM p_lParam) {
			DWORD procId;
			GetWindowThreadProcessId(p_winHandle, &procId);

			// If-else guard clause!:
			if (procId != p_lParam)
				return TRUE;

			s_winHandleRef = p_winHandle;
			return FALSE;
		}, p_procId);

		p_winHandle = s_winHandleRef;
		return status;
	}

};
