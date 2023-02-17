#pragma once
#include <chrono>

class c_hidden_remover {
public:

	int set_mods(int mods);

	int get_mods();

private:
	int m_mods;
};

inline auto hidden_remover = std::make_unique<c_hidden_remover>();