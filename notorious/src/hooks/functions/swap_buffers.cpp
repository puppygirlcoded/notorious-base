#include <functional>

#include "../../session/session.hpp"

#include "../hooks.hpp"
#include "../../utilities/lib/imgui/imgui.h"
#include "../../utilities/lib/imgui/imgui_impl_win32.h"
#include "../../utilities/lib/imgui/imgui_impl_opengl3.h"

#include "../../menu/menu.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT __stdcall n_detours::n_message_proc_callback::hooked(int code, WPARAM wparam, LPARAM lparam) {
	if (code > 0)
		return CallNextHookEx(n_detours::msg_proc_callback_original, code, wparam, lparam);

	MSG* msg = reinterpret_cast<MSG*>(lparam);

	const auto is_button_toggled = [msg](bool& bButton, int vKey, std::function<void()> fn) {
		if (msg->wParam != vKey) return;

		if (msg->message == WM_KEYUP) {
			bButton = !bButton;

			fn();
		}
	};

	if (wparam == PM_REMOVE) {

		if (msg->message == WM_KEYDOWN)
			msg->message = WM_KEYDOWN;

		// input here
		is_button_toggled(menu->m_opened, VK_HOME, [&] {

		});

		ImGui_ImplWin32_WndProcHandler(msg->hwnd, msg->message, msg->wParam, msg->lParam);
	}

	if (menu->m_opened) {

		if (msg->message == WM_CHAR || msg->message == WM_KEYDOWN) {
			msg->message = WM_NULL;
			return 1;
		}

		if ((WM_MOUSEFIRST <= msg->message && msg->message <= WM_MOUSELAST)
			|| (msg->message == WM_MOUSEMOVE || msg->message == WM_NCHITTEST) || (msg->message == WM_SETCURSOR)) {
			msg->message = WM_NULL;
			return 1;
		}
	}

	return CallNextHookEx(n_detours::msg_proc_callback_original, code, wparam, lparam);

}

static DWORD __stdcall n_detours::n_message_proc_callback::thread() {

	MSG msg;
	while (n_detours::msg_proc_callback_original && GetMessageA(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return 0;
}

bool _stdcall n_detours::swap_buffers_hooked(HDC hdc) {

	static bool once = false;
	if (!once) {
		std::thread(n_message_proc_callback::thread).detach();
		msg_proc_callback_original = SetWindowsHookExA(WH_GETMESSAGE, n_message_proc_callback::hooked, session->m_module, GetCurrentThreadId());
		menu->on_swap_buffers_entry(hdc);
		once = true;
	}

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	menu->on_swap_buffers();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return h_swap_buffers.invoke<decltype(&swap_buffers_hooked)>(hdc);
}