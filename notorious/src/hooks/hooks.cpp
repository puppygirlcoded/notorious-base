#include "hooks.hpp"
#include "../state/state.hpp"
#include "../session/session.hpp"
#include "../utilities/security/xor.hpp"

void c_hooks::on_entry() {
	static auto swap_buffers_target = GetProcAddress(session->m_opengl_module, xs("wglSwapBuffers"));

	// Render
	n_detours::h_swap_buffers.enable(swap_buffers_target, n_detours::swap_buffers_hooked);
}