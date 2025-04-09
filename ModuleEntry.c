//
// Apologies for code quality, im very sleep deprived.
//
// POC: Using KUSER_SHARED_DATA to examine how user is interacting with processes without any system service routines.
// 
// When developing malware we often use system service routines like NtQuerySystemInformation, NtQueryInformationProcess, etc. to search for specific processes and gather information about them.
// To avoid unnecessary syscalls, a more heuristic approach can be used in which you simply identify the currently focused pid, duration/frequency of focus, & frequency of user input.
// Assuming the process your searching for has a window & will at some point be focused by the user, this can be a very viable technique that also acts as a sort of delay/sleep obfuscation.
// Furthermore, since LastSystemRITEventTickCount provides us with a reasonably accurate tick count from the last user input, we can also use this concept to detect if the user is present at their computer, 
// if the user is typing/moving mouse, and we can even analyze the users past behavior and usage of programs ( provided we cache information unlike i do here ).
//
#include <Windows.h>
#include <winternl.h>

// theres no accounting for things like processes terminating. use pid as index in array lol.
static DWORD64 fml[10000000];

#define INPUT_TICK_THRESHOLD 1500 // i found 1500 to be a decent enough threshold, tick count is not very accurate.

//
// _ModuleEntry
//  Entry point
//
DWORD __stdcall _ModuleEntry() {
	// KUSER_SHARED_DATA data because cba to add struct
	PDWORD pLastSystemRITEventTickCount = (PDWORD)0x7FFE02E4;
	PDWORD pConsoleSessionForegroundProcessId = (PDWORD)0x7FFE0338;
	PDWORD64 pTickCount = (PDWORD64)0x7FFE0320;
	PDWORD pTickCountMultiplier = (PDWORD)0x7FFE0004;



	DWORD64 InitialInputTickCount = *pLastSystemRITEventTickCount;
	DWORD64 InitialForegroundFocusTickCount = 0;
	DWORD StoredForegroundPid = 0;

	while (1) {
		DWORD64 CurrentTickCount = (*pTickCount * *pTickCountMultiplier) >> 0x18;

		// handle window change focus

		if (StoredForegroundPid != *pConsoleSessionForegroundProcessId) {

			// update fml on each focus change because its easier
			if(StoredForegroundPid != 0 && InitialForegroundFocusTickCount != 0)
				fml[StoredForegroundPid] += (CurrentTickCount - InitialForegroundFocusTickCount);

			InitialForegroundFocusTickCount = CurrentTickCount;
			StoredForegroundPid = *pConsoleSessionForegroundProcessId;
			continue; // continue to update CurrentTickCount
		}

		DWORD64 TicksSinceWindowFocused = (CurrentTickCount - InitialForegroundFocusTickCount);

		if (fml[StoredForegroundPid] == 0) { // is first time focused

		}
		else { // has been focused before

		}
		

		// handle input start/stop
		DWORD64 TicksSinceLastInput = (CurrentTickCount - *pLastSystemRITEventTickCount);
		if (TicksSinceLastInput > INPUT_TICK_THRESHOLD) {
			InitialInputTickCount = *pLastSystemRITEventTickCount;
			continue; // continue to update CurrentTickCount
		}

		DWORD64 TicksSinceInitialInput = (CurrentTickCount - InitialInputTickCount);

		if (TicksSinceInitialInput > INPUT_TICK_THRESHOLD) { // is sufficient input duration for acknowledgement

		}
	}

	return 0;
}
