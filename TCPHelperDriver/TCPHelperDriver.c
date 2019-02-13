// TCPHelperDriver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <libloaderapi.h>
#include <tchar.h>

int _tmain(int argc, TCHAR *argv[])
{
	int port = 8080;
	WCHAR pAddr[128];
	wcscpy_s(pAddr, _countof(pAddr), L"127.0.0.1");
	int addrFamily = 4; //(4 for IPv4 or 6 for IPv6)
	if (argc > 1) {
		port = _ttoi(argv[1]);
	}
	if (argc > 2) {
#ifdef UNICODE
		wcscpy_s(pAddr, _countof(pAddr), argv[2]);
#else
		size_t numConverted;
		pAddr = mbstowcs_s(&numConverted, pAddr, _countof(pAddr), argv[2], _TRUNCATE);
#endif
	}
	if (argc > 3) {
		addrFamily = argv[3];
	}
	//Build configuration places driver in the same directory as dll
	HMODULE hDll = LoadLibrary(_T("TCPHelper.dll"));

	//We are testing functions from TCPHelper here
	// int GetConnectionPID(int localPort, WCHAR* srcAddress, int addressFamily);
	// int GetProcessName(int processId, WCHAR** pBuffer, int bufferSize);
	typedef int(*GETCONNECTIONPID)();
	typedef int(*GETPROCESSNAME)();

	GETCONNECTIONPID GetConnectionPID = (GETCONNECTIONPID)GetProcAddress(hDll, "GetConnectionPID");
	if (!GetConnectionPID) {
		_tprintf(_T("\nFailed to find function GetConnectionPID"));
	}
	GETPROCESSNAME GetProcessName = (GETPROCESSNAME)GetProcAddress(hDll, "GetProcessName");
	if (!GetProcessName) {
		_tprintf(_T("\nFailed to find function GetProcessName"));
	}

	//Calling GetConnectionPID
	int pid = GetConnectionPID(port, pAddr, addrFamily);
	if (pid > 0) {
		_tprintf(_T("\nThe PID is %d"), pid);
		//Calling GetProcessName
		WCHAR exePath[MAX_PATH + 1];
		WCHAR* pExePath = exePath;
		int exePathSize = _countof(exePath);

		int retVal = GetProcessName(pid, &pExePath, exePathSize);
		if (!retVal) {
			_tprintf(_T("\nThe process path is %s"), exePath);
		}
		else {
			_tprintf(_T("\nDid not find the process path"));
		}
	}
	else {
		_tprintf(_T("\nDid not find the process Id"));
	}

	int stop = 0;
}


