**WindowsHelpers**

WindowsHelpers is a Visual Studio 2017 solution. Open it in Visual Studio and compile to produce executables. You may specify different configurations. Build 32-bit or 64-bit versions, Release or Debug.

Note that on 64-bit platform we can run successfully a 32-bit executable. If this executable needs additional dlls they should also be 32-bit. A 64-bit executable requires 64-bit dlls. You cannot mix and match.

On 32-bit platform, obviously, all binaries should be 32-bit.

**Projects**

1. **LowRide**. This one builds 32- or 64-bit LowRide.exe.

It launches specified executable in Medium integrity mode even if running in High integrity mode itself.

Example of usage:

**>lowride c:\windows\system32\notepad.exe**

Use quotes to specify a path with blanks.

1. **TCPHelper**. Builds 32- or 64-bit TCPHelper.dll.

This dll provides two functions which can be called from golang code. To see how it can be done take a look at tcphelper.go file in godivert repo.

**Godivert** needs **GetConnectionPID** to function properly.

**GetProcessName** is useful, but supplemental. Both functions may be called from other places in the Ghostery Desktop project.

For example, using these functions we may test if a user machine has other processes running on port 8080 or 4443, and adjust ports accordingly.

1. **TCPHelperDriver**. This one builds 32- or 64-bit TCPHelperDriver.exe

This executable uses same-bit TCPHelper.dll and calls its functions.

The dll should be placed in the same directory as the executable.

Without command-line parameters TCPHelperDriver finds PID for 127.0.0.1:8080 and IPv4 connections. Different parameters can be specified on command line, like this:

**>TCPHelperDriver 4443 &quot;127.0.0.1&quot; 6**

(6 for IPv6 connections). Currently the reason for **TCPHelperDriver** is to be able conveniently debug content of **TCPHelper.dll**.
