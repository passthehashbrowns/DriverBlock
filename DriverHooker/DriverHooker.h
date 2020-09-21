#pragma once
#include <tdh.h>
#include <winternl.h>


VOID DoOriginalCreateService(
	SC_HANDLE hSCManager,
	LPCWSTR   lpServiceName,
	LPCWSTR   lpDisplayName,
	DWORD     dwDesiredAccess,
	DWORD     dwServiceType,
	DWORD     dwStartType,
	DWORD     dwErrorControl,
	LPCWSTR   lpBinaryPathName,
	LPCWSTR   lpLoadOrderGroup,
	LPDWORD   lpdwTagId,
	LPCWSTR   lpDependencies,
	LPCWSTR   lpServiceStartName,
	LPCWSTR   lpPassword
);

VOID WINAPI CreateServiceHook(
	SC_HANDLE hSCManager,
	LPCWSTR   lpServiceName,
	LPCWSTR   lpDisplayName,
	DWORD     dwDesiredAccess,
	DWORD     dwServiceType,
	DWORD     dwStartType,
	DWORD     dwErrorControl,
	LPCWSTR   lpBinaryPathName,
	LPCWSTR   lpLoadOrderGroup,
	LPDWORD   lpdwTagId,
	LPCWSTR   lpDependencies,
	LPCWSTR   lpServiceStartName,
	LPCWSTR   lpPassword
);

VOID HookCreateService();

typedef int(WINAPI* CreateService_) (SC_HANDLE hSCManager,
	LPCWSTR   lpServiceName,
	LPCWSTR   lpDisplayName,
	DWORD     dwDesiredAccess,
	DWORD     dwServiceType,
	DWORD     dwStartType,
	DWORD     dwErrorControl,
	LPCWSTR   lpBinaryPathName,
	LPCWSTR   lpLoadOrderGroup,
	LPDWORD   lpdwTagId,
	LPCWSTR   lpDependencies,
	LPCWSTR   lpServiceStartName,
	LPCWSTR   lpPassword);

BOOL PlaceHook();

extern "C" {
	BOOL WINAPI EnumProcessModules(
		HANDLE hProcess,
		HMODULE* lphModule,
		DWORD cb,
		LPDWORD lpcbNeeded
	);
}

extern "C"
{
	DWORD
		WINAPI
		GetModuleBaseNameA(
			_In_ HANDLE hProcess,
			_In_opt_ HMODULE hModule,
			_Out_writes_(nSize) LPSTR lpBaseName,
			_In_ DWORD nSize
		); }