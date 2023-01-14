#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

class Memory
{
private:
	DWORD id = 0;
	HANDLE processHandle = NULL;
public:
	Memory(const char* proecssName);
	Memory();


	DWORD GetProcessId();
	HANDLE GetProcessHandle();

	uintptr_t GetModuleAddy(const char* moduleName);

	template <typename T>
	T Read(uintptr_t addy) {
		T value;
		ReadProcessMemory(this->processHandle, (LPCVOID)addy, &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	bool Write(uintptr_t addy, T value) {
		return WriteProcessMemory(this->processHandle, (LPVOID)addy, &value, sizeof(T), NULL);
	}
};