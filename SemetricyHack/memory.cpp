#include "memory.h"
#include <TlHelp32.h>
Memory::Memory(const char* proecssName)
{
	PROCESSENTRY32 entry; 
	entry.dwSize = sizeof(PROCESSENTRY32);
	const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	while (Process32Next(snapShot, &entry)) {
		if (!strcmp(proecssName, entry.szExeFile)) {
			this->id = entry.th32ProcessID;
			this->processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->id);
			break;
		}
	}

	if (snapShot)
		CloseHandle(snapShot);
}

Memory::Memory()
{
	if (this->processHandle)
		CloseHandle(this->processHandle);
}


DWORD Memory::GetProcessId()
{
	return this->id;
}

HANDLE Memory::GetProcessHandle()
{
	return this->processHandle;
}

uintptr_t Memory::GetModuleAddy(const char* moduleName)
{
	MODULEENTRY32 module; 
	module.dwSize = sizeof(MODULEENTRY32);
	const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->id);

	uintptr_t moduleAddy = 0; 

	while (Module32Next(snapShot, &module)) {
		if (!strcmp(moduleName, module.szModule)) {
			moduleAddy = reinterpret_cast<uintptr_t> (module.modBaseAddr);
			break;
		}
	}

	if (snapShot)
		CloseHandle(snapShot);


	return moduleAddy;
}