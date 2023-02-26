#pragma once
#include <chrono>

class c_variables {
public:
	struct {
		bool enabled = false;
		int rate = 100;
	} timewarp;
};

inline auto variables = std::make_unique<c_variables>();