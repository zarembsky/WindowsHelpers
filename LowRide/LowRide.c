// LowRide.cpp : This file contains full implementation code.
// It launches specified executable in Medium integrity level
// while running in High mode itself.
// Program is self-contained and does not rely on the presence 
// of other resources (dlls) on user machine. 
// The default configuration is multibyte 32-bit exe, which runs on 64-bit Windows too.

#include <Windows.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <securitybaseapi.h>
#include <winnt.h>
#include <sddl.h>

#include <sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>

#include <stdio.h>
#include <tchar.h>

BOOL CreateLowProcess(LPCTSTR proxyPath)
{
	BOOL                  fRet;
	HANDLE                hToken = NULL;
	HANDLE                hNewToken = NULL;
	PSID                  pIntegritySid = NULL;
	TOKEN_MANDATORY_LABEL TIL = { 0 };
	PROCESS_INFORMATION   ProcInfo = { 0 };
	STARTUPINFO           StartupInfo = { 0 };

	TCHAR wszProcessName[MAX_PATH];
	_tcscpy_s(wszProcessName, _countof(wszProcessName), proxyPath);

	// Windows Vista defines four integrity levels : 
	// Low(SID : S - 1 - 16 - 4096) 
	// Medium(SID : S - 1 - 16 - 8192) 
	// High(SID : S - 1 - 16 - 12288) 
	// System(SID : S - 1 - 16 - 16384)

	// Medium integrity SID
	TCHAR wszIntegritySid[20] = _T("S-1-16-8192");

	fRet = OpenProcessToken(GetCurrentProcess(),
		TOKEN_DUPLICATE |
		TOKEN_ADJUST_DEFAULT |
		TOKEN_QUERY |
		TOKEN_ASSIGN_PRIMARY,
		&hToken);

	if (!fRet)
	{
		goto CleanExit;
	}

	fRet = DuplicateTokenEx(hToken,
		0,
		NULL,
		SecurityImpersonation,
		TokenPrimary,
		&hNewToken);

	if (!fRet)
	{
		goto CleanExit;
	}

	fRet = ConvertStringSidToSid(wszIntegritySid, &pIntegritySid);

	if (!fRet)
	{
		goto CleanExit;
	}

	TIL.Label.Attributes = SE_GROUP_INTEGRITY;
	TIL.Label.Sid = pIntegritySid;

	//
	// Set the process integrity level
	//

	fRet = SetTokenInformation(hNewToken,
		TokenIntegrityLevel,
		&TIL,
		sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(pIntegritySid));

	if (!fRet)
	{
		goto CleanExit;
	}

	//
	// Create the new process at Low integrity
	//

	fRet = CreateProcessAsUser(hNewToken,
		NULL,
		wszProcessName,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&StartupInfo,
		&ProcInfo);

CleanExit:

	if (ProcInfo.hProcess != NULL)
	{
		CloseHandle(ProcInfo.hProcess);
	}

	if (ProcInfo.hThread != NULL)
	{
		CloseHandle(ProcInfo.hThread);
	}

	LocalFree(pIntegritySid);

	if (hNewToken != NULL)
	{
		CloseHandle(hNewToken);
	}

	if (hToken != NULL)
	{
		CloseHandle(hToken);
	}

	return fRet;
}

int _tmain(int argc, TCHAR *argv[])
{
	if (argc == 1) {
		_tprintf(_T("Usage: >LowRide <path to exe>. \nFor paths with blanks use double quotes.\nSuccessful run returns 1, unsuccessful - 0"));
		return 2;
	}
	else {
		return CreateLowProcess(argv[1]);
	}
}
