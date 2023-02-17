#pragma once
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <iostream>
#include <cstdarg>
#include <string>

class c_console {
public:
	bool attach(const std::string& console_title) {
#ifdef _DEBUG
		this->m_title = console_title;
		AllocConsole();
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		SetConsoleTitleA(console_title.data());
		this->m_attached = true;
		return true;
#endif
	}
	
	void print(const char* fmt, ...) {
		if (!this->is_attached())
			return;
#ifdef _DEBUG
		char msg[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf_s(msg, fmt, args);
		std::cout << msg << std::endl;
		va_end(args);
#endif
	}

	void detach() {
#ifdef _DEBUG
		fclose((FILE*)stdin);
		fclose((FILE*)stdout);
		HWND console_hwnd = GetConsoleWindow();
		FreeConsole();
		PostMessageW(console_hwnd, WM_CLOSE, 0, 0);
		this->m_attached = false;
#endif
	}

	bool is_attached() {
		return this->m_attached;
	}

private:
	std::string m_title;
	bool m_attached;
};

inline auto console = std::make_unique<c_console>();