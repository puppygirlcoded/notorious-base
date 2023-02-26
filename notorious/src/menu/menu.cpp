#include "menu.hpp"

#include "../utilities/lib/imgui/imgui.h"
#include "../utilities/lib/imgui/imgui_impl_win32.h"
#include "../utilities/lib/imgui/imgui_impl_opengl3.h"

#include "../utilities/security/xor.hpp"
#include "../variables.hpp"

void c_menu::on_swap_buffers_entry(HDC hdc) {
	auto ctx = ImGui::CreateContext();
	ImGui::SetCurrentContext(ctx);

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(WindowFromDC(hdc));
	ImGui_ImplOpenGL3_Init();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().FontDefault = io.Fonts->AddFontFromFileTTF(xs("c:\\Windows\\Fonts\\ebrima.ttf"), 16.0f);
}


void c_menu::on_swap_buffers() {

	ImGui::GetIO().MouseDrawCursor = this->m_opened;

	if (!this->m_opened)
		return;

	if (ImGui::Begin(xs("Notorious"), nullptr)) {

		ImGui::Checkbox(xs("Timewarp"), &variables->timewarp.enabled);
		if (variables->timewarp.enabled) {
			ImGui::SliderInt(xs("Rate"), &variables->timewarp.rate, 25, 175);
		}

		ImGui::End();
	}
}
