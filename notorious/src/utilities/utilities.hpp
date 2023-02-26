#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>
#include <list>
#include <cstdarg>
#include <string>

#define strong_inline __pragma(warning(suppress : 4714)) inline __forceinline
#define mem_likely(x) static_cast<bool>(x)
#define mem_unlikely(x) static_cast<bool>(x)

#include "memory_scanner/pattern.hpp"


struct sig_helper_t {
	int offset{};
	const char* sig{};

	sig_helper_t() {
		offset = 0x0; sig = "";
	}

	sig_helper_t(int _offset, const char* _sig) {
		offset = _offset; sig = _sig;
	}
};

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

	// Thanks @Patoke
	template<unsigned int idx, typename t, typename ... args>
	static t callv_function(void* thisptr, args ... argList) {
		using fn = t(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<fn**>(thisptr))[idx](thisptr, argList...);
	}

	template <typename t>
	static t getv_function(void* class_pointer, size_t index) {
		return (*(t**)class_pointer)[index];
	}

	template<typename T>
	inline size_t read_memory(uintptr_t address, T* out, size_t count = 0x1) {
		size_t read = 0;

		if (!ReadProcessMemory(session->m_handle, reinterpret_cast<LPCVOID>(address),
			reinterpret_cast<LPVOID>(out), count * sizeof(T),
			reinterpret_cast<SIZE_T*>(&read))) {
			return 0;
		}

		return read;
	}

	template<typename T>
	inline T read_memory(uintptr_t address) {
		T out{};

		if (!read_memory(address, &out, 0x1))
			console->print("couldn't read memory address");

		return out;
	}

	struct memory_region_t {
		DWORD base;
		SIZE_T region_size;
		DWORD state;
		DWORD protect;
		DWORD type;

		memory_region_t(MEMORY_BASIC_INFORMATION32 mbi) {
			base = mbi.BaseAddress;
			region_size = mbi.RegionSize;
			state = mbi.State;
			protect = mbi.Protect;
			type = mbi.Type;
		}
	};

	static uintptr_t find_signature(uintptr_t start, uintptr_t length, sig_helper_t& signature) {
		mem::region range;
		range.start = start;
		range.size = length;

		mem::pattern pattern_to_search(signature.sig);
		mem::default_scanner scanner(pattern_to_search);
		uintptr_t return_value{};

		scanner(range, [&](mem::pointer address) {
			return_value = address.add(signature.offset).as<uintptr_t>();
		return false;
			});
		return return_value;
	}

	// needs module
	uintptr_t find_signature(const char* hmodule, sig_helper_t& signature) {
		DWORD range_start = (DWORD)GetModuleHandleA(hmodule);
		MODULEINFO mod_info{};
		GetModuleInformation(GetCurrentProcess(), (HMODULE)range_start, &mod_info, sizeof(MODULEINFO));

		return find_signature(range_start, mod_info.SizeOfImage, signature);
	}

	// doesn't lol
	uintptr_t find_signature(const char* signature) {
		auto pattern_bytes = std::vector<int>{ };

		for (auto cur = signature; *cur; cur++) {
			if (*cur == '?')
				// If the current byte is a wildcard push a dummy byte
				pattern_bytes.push_back(-1);

			else if (*cur == ' ')
				continue;
			else
				pattern_bytes.push_back(strtol(cur, const_cast<char**>(&cur), 16));
		}

		auto pattern_size = pattern_bytes.size();

		const size_t chunk_size = 4096;
		std::byte chunk_bytes[chunk_size];

		for (uintptr_t i = 1; i < INT_MAX; i += chunk_size - pattern_size) {
			if (!read_memory<std::byte>(i, chunk_bytes, chunk_size)) {
				continue;
			}

			for (size_t j = 0; j < chunk_size; j++) {
				bool hit = true;

				for (size_t k = 0; k < pattern_size; k++) {
					if (pattern_bytes[k] == -1) {
						continue;
					}

					if (chunk_bytes[j + k] !=
						static_cast<std::byte>(pattern_bytes[k])) {
						hit = false;
						break;
					}
				}

				if (hit) {
					return i + j;
				}
			}
		}

		return 0;
	}

	static uintptr_t find_signature(sig_helper_t& signature) {
		std::list<memory_region_t> memory_regions{};
		uintptr_t return_value{};

		// dump memory regions
		[&memory_regions]() -> void {
			memory_regions.clear();

			MEMORY_BASIC_INFORMATION32 mbi{};
			LPCVOID address = 0;

			while (VirtualQueryEx(GetCurrentProcess(), address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof(mbi)) != 0) {
				if (mbi.State == MEM_COMMIT && mbi.Protect >= 0x10 && mbi.Protect <= 0x80)
					memory_regions.push_back(*new memory_region_t(mbi));

				address = reinterpret_cast<LPCVOID>(mbi.BaseAddress + mbi.RegionSize);
			}
		}();

		for (const auto& region : memory_regions) {
			if (return_value)
				break;

			return_value = find_signature(region.base, region.region_size, signature);
		}

		if (!return_value)
			console->print("couldn't find (%s) at 0x%x", signature.sig, return_value);
		else
			console->print("in_use (%s) at 0x%x", signature.sig, return_value);

		return return_value;
	}

};

inline auto utilities = std::make_unique<c_utilities>();