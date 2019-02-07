// TCPHelper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "tcphelper.h"

#ifndef UNICODE
#define UNICODE
#endif

// Need to link with Iphlpapi.lib and Ws2_32.lib
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

int GetConnectionPID(int srcPort, WCHAR* srcAddress, int addressFamily) 
{
	char szSrcAddress[128];
	int ret = wcstombs(szSrcAddress, srcAddress, sizeof(szSrcAddress));
	if (ret == 128) szSrcAddress[127] = '\0';

	if (addressFamily == 4) {
		// Declare and initialize variables
		PMIB_TCPTABLE2 pTcpTable;
		ULONG ulSize = 0;
		DWORD dwRetVal = 0;

		struct in_addr IpAddr;

		int i;

		pTcpTable = (MIB_TCPTABLE2 *)MALLOC(sizeof(MIB_TCPTABLE2));
		if (pTcpTable == NULL) {
			return -1;
		}

		ulSize = sizeof(MIB_TCPTABLE);
		// Make an initial call to GetTcpTable2 to
		// get the necessary size into the ulSize variable
		if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE)) ==
			ERROR_INSUFFICIENT_BUFFER) {
			FREE(pTcpTable);
			pTcpTable = (MIB_TCPTABLE2 *)MALLOC(ulSize);
			if (pTcpTable == NULL) {
				return -1;
			}
		}

		if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE)) == NO_ERROR) {
			for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
				MIB_TCPROW2& table = pTcpTable->table[i];
				if (ntohs((u_short)table.dwLocalPort) == srcPort) {
					IpAddr.S_un.S_addr = (u_long)table.dwLocalAddr;
					if (!_stricmp(inet_ntoa(IpAddr), szSrcAddress)) {
						return table.dwOwningPid;
					}
				}
			}
		}
	}
	else if (addressFamily == 6) {
		// Declare and initialize variables
		PMIB_TCP6TABLE2 pTcpTable;
		DWORD dwSize = 0;
		DWORD dwRetVal = 0;
		wchar_t ipstringbuffer[46];
		int i;

		pTcpTable = (MIB_TCP6TABLE2 *)MALLOC(sizeof(MIB_TCP6TABLE2));
		if (pTcpTable == NULL) {
			return -1;
		}

		dwSize = sizeof(MIB_TCP6TABLE);
		// Make an initial call to GetTcp6Table to
		// get the necessary size into the dwSize variable
		if ((dwRetVal = GetTcp6Table2(pTcpTable, &dwSize, TRUE)) ==
			ERROR_INSUFFICIENT_BUFFER) {
			FREE(pTcpTable);
			pTcpTable = (MIB_TCP6TABLE2 *)MALLOC(dwSize);
			if (pTcpTable == NULL) {
				return -1;
			}
		}

		// Make a second call to GetTcp6Table to get
		// the actual data we require
		if ((dwRetVal = GetTcp6Table2(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
			for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
				MIB_TCP6ROW2& table = pTcpTable->table[i];

				if (ntohs((u_short)table.dwLocalPort) == srcPort) {
					if (InetNtop(AF_INET6, &table.LocalAddr, ipstringbuffer, 46) != NULL) {
						if (!wcscmp(ipstringbuffer, srcAddress)) {
							return table.dwOwningPid;
						}
					}
				}
			}
		}
	}

	return -1;
}

//private ITcpConnectionInfo GetLocalPacketInfo(ushort localPort, IPAddress localAddress)
//{
//	switch (localAddress.AddressFamily)
//	{
//	case System.Net.Sockets.AddressFamily.InterNetwork:
//	{
//		return NetworkTables.GetTcp4Table().Where(x = > x.LocalPort == localPort && (x.LocalAddress.Equals(localAddress) || x.LocalAddress.Equals(IPAddress.Any))).FirstOrDefault();
//	}
//
//	case System.Net.Sockets.AddressFamily.InterNetworkV6:
//	{
//		return NetworkTables.GetTcp6Table().Where(x = > x.LocalPort == localPort && (x.LocalAddress.Equals(localAddress) || x.LocalAddress.Equals(IPAddress.IPv6Any))).FirstOrDefault();
//	}
//	}
//
//	return null;
//}

//public static string GetProcessName(ulong processId)
//{
//	StringBuilder buffer = new StringBuilder(1024);
//	IntPtr hprocess = Kernel32.OpenProcess(Kernel32.ProcessAccessFlags.QueryLimitedInformation, false, (uint)processId);
//	if (hprocess != IntPtr.Zero)
//	{
//		try
//		{
//			int size = buffer.Capacity;
//			if (Kernel32.QueryFullProcessImageName(hprocess, 0, buffer, ref size))
//			{
//				return buffer.ToString();
//			}
//		}
//		finally
//		{
//			Kernel32.CloseHandle(hprocess);
//		}
//	}
//	return string.Empty;
//}

int GetProcessName(int processId, WCHAR** pBuffer, int bufferSize) {
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, processId);
	if (hProcess) {
		try {
			DWORD dwSize = bufferSize;
			BOOL bRet = ::QueryFullProcessImageNameW(hProcess, 0, *pBuffer, &dwSize);
			return bRet ? 0 : -1;
		}
		catch (...) {

		}
	}
	return -1;
}