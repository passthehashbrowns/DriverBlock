#include <stdio.h>
#include <windows.h>
#include <string>
#include <string.h>
#include <cstring>
#include <tdh.h>
#include <psapi.h>
#include <evntcons.h>
#include <tlhelp32.h>
#include <vector>
#include <winsvc.h>

//#include "driverhooker.h"

//this loads createservicea
//#define PATTERN "\x48\x8b\xc4\x44\x89\x48\x20\x4c\x89\x40\x18\x48\x89\x50\x10\x48\x89\x48\x08"

//#define PATTERN "\x48\x8b\xc4\x44\x89\x48\x20\x4c\x89\x40\x18\x48\x89\x50\x10\x48\x89\x48\x08\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xec\xe0\x00\x00\x00"

#define PATTERN "\x6a\x48\x68\x18\x4f\x07\x75\xe8\x48\xb0\xfd\xff\x33\xff\x89\x7d\xb8\x89\x7d\xd8\x89\x7d\xd0\x89\x7d\xdc\x89\x7d\xd4\x33\xc0\x66\x89\x45\xe4"

#pragma comment (lib, "psapi.lib")
#pragma comment (lib, "tdh.lib")
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "crypt32.lib")

CHAR* cRule;
LPVOID lpCallbackOffset;
CHAR   OriginalBytes[50] = {};

int Start();

unsigned char trampoline[] = { 0x49, 0xbb, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x41, 0xff, 0xe3 };

int __stdcall CreateServiceAHook(SC_HANDLE hSCManager,
	LPCSTR   lpServiceName,
	LPCSTR   lpDisplayName,
	DWORD     dwDesiredAccess,
	DWORD     dwServiceType,
	DWORD     dwStartType,
	DWORD     dwErrorControl,
	LPCSTR   lpBinaryPathName,
	LPCSTR   lpLoadOrderGroup,
	LPDWORD   lpdwTagId,
	LPCSTR   lpDependencies,
	LPCSTR   lpServiceStartName,
	LPCSTR   lpPassword);

FARPROC CreateServiceAAddress = NULL;
SIZE_T bytesWritten = 0;
char CreateServiceAOriginalBytes[6] = {};
char patch[6] = { 0 };
DWORD oldProtect;

VOID __stdcall HookedCreateServiceA(SC_HANDLE hSCManager,
	LPCSTR   lpServiceName,
	LPCSTR   lpDisplayName,
	DWORD     dwDesiredAccess,
	DWORD     dwServiceType,
	DWORD     dwStartType,
	DWORD     dwErrorControl,
	LPCSTR   lpBinaryPathName,
	LPCSTR   lpLoadOrderGroup,
	LPDWORD   lpdwTagId,
	LPCSTR   lpDependencies,
	LPCSTR   lpServiceStartName,
	LPCSTR   lpPassword) {

	// print intercepted values from the CreateServiceAA function
	CHAR nameBuffer[500];
	sprintf_s(nameBuffer, "%s", lpServiceName);
	if (strstr(nameBuffer, "pmem"))
	{
		OutputDebugStringA("[+] Preventing kernel driver load!\n");
		return;
	}

	VirtualProtect(CreateServiceAAddress, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Add our JMP addr for our hook

	// Copy over our trampoline
	memcpy(CreateServiceAAddress, CreateServiceAOriginalBytes, sizeof(CreateServiceAOriginalBytes));

	// Restore previous page protection so Dom doesn't shout
	VirtualProtect(CreateServiceAAddress, 0x1000, oldProtect, &oldProtect);

	// unpatch CreateServiceAA
	//WriteProcessMemory(GetCurrentProcess(), (LPVOID)CreateServiceAAddress, CreateServiceAOriginalBytes, sizeof(CreateServiceAOriginalBytes), &bytesWritten);

	// call the original CreateServiceAA
	SC_HANDLE ret = CreateServiceA(hSCManager,
		lpServiceName,
		lpDisplayName,
		dwDesiredAccess,
		dwServiceType,
		dwStartType,
		dwErrorControl,
		lpBinaryPathName,
		lpLoadOrderGroup,
		lpdwTagId,
		lpDependencies,
		lpServiceStartName,
		lpPassword);

	//Start();
	return;
}

int Start()
{
	// show CreateServiceA before hooking

	HINSTANCE library = LoadLibraryA("sechost.dll");
	SIZE_T bytesRead = 0;
	
	// get address of the CreateServiceA function in memory
	CreateServiceAAddress = GetProcAddress(library, "CreateServiceA");

	// save the first 6 bytes of the original CreateServiceAA function - will need for unhooking
	ReadProcessMemory(GetCurrentProcess(), CreateServiceAAddress, CreateServiceAOriginalBytes, 6, &bytesRead);

	// create a patch "push <address of new CreateServiceAA); ret"
	void* hookedCreateServiceAAddress = &HookedCreateServiceA;
	
	memcpy_s(patch, 1, "\x68", 1);
	memcpy_s(patch + 1, 4, &hookedCreateServiceAAddress, 4);
	memcpy_s(patch + 5, 1, "\xC3", 1);

	VirtualProtect(CreateServiceAAddress, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Add our JMP addr for our hook

	// Copy over our trampoline
	memcpy(CreateServiceAAddress, patch, sizeof(patch));

	// Restore previous page protection so Dom doesn't shout
	VirtualProtect(CreateServiceAAddress, 0x1000, oldProtect, &oldProtect);



	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	OutputDebugStringA("Injected\n");
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Start();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}