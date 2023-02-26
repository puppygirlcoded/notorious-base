#pragma once
#include <windows.h>
#include <chrono>

#include "detour/detour.hpp"

namespace n_detours {
	namespace n_message_proc_callback {
		using fn = LRESULT(__stdcall*)(int, WPARAM, LPARAM);
		static DWORD __stdcall thread();
		static LRESULT __stdcall hooked(int code, WPARAM wparam, LPARAM lparam);
	}

	inline HHOOK msg_proc_callback_original;

	inline define_hook(h_swap_buffers, HDC, _stdcall);
	bool _stdcall swap_buffers_hooked(HDC hdc);

	inline define_hook(h_set_playback_rate, double, __stdcall);
	void __stdcall set_playback_rate_hooked(double rate);
}

class c_hooks {
public:
	void on_entry();
private:
};

inline auto hooks = std::make_unique<c_hooks>();