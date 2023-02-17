#pragma once
#include <windows.h>
#include <chrono>

class c_session {
public:
	void on_entry();

	bool m_attached;
	bool m_detach;
	int m_process_id;
	void* m_handle;
	HMODULE m_module;
	HMODULE m_opengl_module;
};

inline auto session = std::make_unique<c_session>();