#include "../hooks.hpp"
#include "../../variables.hpp"

// This is detected without any bypasses and patches but this is how we modify the game playback rate and create the cheat named "Timewarp"
// this also doesn't patch the audio errors when playing. TODO: Fix audio errors
void __stdcall n_detours::set_playback_rate_hooked(double rate) {

	if (variables->timewarp.enabled) {
		return h_set_playback_rate.invoke<decltype(&set_playback_rate_hooked)>(variables->timewarp.rate);
	}

	h_set_playback_rate.invoke<decltype(&set_playback_rate_hooked)>(rate);
}