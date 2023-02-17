#pragma once
#include <chrono>

class c_variables {
public:
	struct {
		bool hidden_remover = false;
	} enlighten;
};

inline auto variables = std::make_unique<c_variables>();