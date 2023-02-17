#include "session.hpp"

#include "../utilities/security/xor.hpp"
#include "../utilities/console/console.hpp"
#include "../utilities/utilities.hpp"

void c_session::on_entry() {
	this->m_process_id = utilities->get_process_id(xs("osu!.exe"));
	this->m_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, this->m_process_id);
	this->m_module = GetModuleHandle(NULL);
	this->m_opengl_module = GetModuleHandleA(xs("opengl32.dll"));

	if (!this->m_handle || this->m_handle == INVALID_HANDLE_VALUE || !this->m_module || !this->m_opengl_module) {
		console->print(xs("Failed to get handle and/or modules"));
		return;
	}

	console->print(xs("Session created"));
}