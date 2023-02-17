#include "hidden_remover.hpp"

int c_hidden_remover::set_mods(int mods) {
	this->m_mods = mods;
	return this->m_mods;
}

int c_hidden_remover::get_mods() {
	return this->m_mods;
}