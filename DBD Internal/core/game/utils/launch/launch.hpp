#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <string>

inline const wchar_t* TARGET_PROCESS_NAME = L"FortniteClient-Win64-Shipping.exe";

inline BOOL ValidateLaunch()
{
	DWORD currentProcessId = GetCurrentProcessId();

	HANDLE hCurrentProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, currentProcessId);
	if (hCurrentProcess == NULL)
	{
		return FALSE;
	}

	wchar_t processFileName[MAX_PATH];
	if (GetProcessImageFileName(hCurrentProcess, processFileName, MAX_PATH) == 0)
	{
		CloseHandle(hCurrentProcess);
		return FALSE;
	}

	std::wstring fullPath(processFileName);
	size_t pos = fullPath.find_last_of(L"\\");
	std::wstring processName = fullPath.substr(pos + 1);

	std::wstring semaphoreName = L"Global\\";
	semaphoreName += processName;
	HANDLE hSemaphore = CreateSemaphore(NULL, 0, 1, semaphoreName.c_str());

	if (hSemaphore == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hCurrentProcess);
		return FALSE;
	}


	if (_wcsicmp(processName.c_str(), TARGET_PROCESS_NAME) == 0)
	{
		return TRUE;
	}

	CloseHandle(hCurrentProcess);
	return FALSE;
}