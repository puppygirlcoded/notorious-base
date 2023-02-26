#include "hooks.hpp"
#include "../state/state.hpp"
#include "../session/session.hpp"
#include "../utilities/console/console.hpp"
#include "../utilities/security/xor.hpp"
#include "../utilities/utilities.hpp"

void c_hooks::on_entry() {
	static auto swap_buffers_target = GetProcAddress(session->m_opengl_module, xs("wglSwapBuffers"));
	static auto set_playback_rate_target = utilities->find_signature(xs("55 8B EC 56 8B  35 ? ? ? ? 85 F6 75 05 5E"));

	/* Render */	
	// wglSwapBuffers
	n_detours::h_swap_buffers.enable(swap_buffers_target, n_detours::swap_buffers_hooked);
	

	/* Audio Engine */
	// SetPlaybackRate
	n_detours::h_set_playback_rate.enable(set_playback_rate_target, n_detours::set_playback_rate_hooked);

	console->print(xs("Hooks created"));
}