#pragma once
_declspec(dllexport) int GetConnectionPID(int localPort, WCHAR* srcAddress, int addressFamily);
_declspec(dllexport)  int GetProcessName(int processId, WCHAR** pBuffer, int bufferSize);