#pragma once
#include <chrono>
#include <windows.h>
#include <windef.h>

class c_menu {
public:
	void on_swap_buffers_entry(HDC hdc);
	void on_swap_buffers();

	bool m_opened = false;
};

inline auto menu = std::make_unique<c_menu>();