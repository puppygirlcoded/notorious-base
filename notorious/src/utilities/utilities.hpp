#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <cstdarg>
#include <string>

class c_utilities {
public:
	unsigned long get_process_id(const std::string& process_name) {
		PROCESSENTRY32 processInfo{};
		processInfo.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (!snapshot || snapshot == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("failed to get process list");
		}

		Process32First(snapshot, &processInfo);
		if (process_name == processInfo.szExeFile) {
			CloseHandle(snapshot);
			return processInfo.th32ProcessID;
		}

		while (Process32Next(snapshot, &processInfo)) {
			if (process_name == processInfo.szExeFile) {
				CloseHandle(snapshot);
				return processInfo.th32ProcessID;
			}
		}

		CloseHandle(snapshot);
		return 0ul;
	}
};

inline auto utilities = std::make_unique<c_utilities>();