#include <windows.h>

#include "session/session.hpp"
#include "hooks/hooks.hpp"

#include "utilities/security/xor.hpp"
#include "utilities/console/console.hpp"

unsigned long __stdcall create_session(void* arg) {
	try {
		console->attach(xs("Notorious"));
		console->print(xs("Creating session"));
		
		session->on_entry();
		hooks->on_entry();
	}
	catch (const std::exception& ex) {
#ifdef _DEBUG
		MessageBoxA(nullptr, ex.what(), "Error", MB_OK);
		FreeLibraryAndExitThread(static_cast<HMODULE>(arg), 0);
#else
		__fastfail(rand() % RAND_MAX);
#endif
	}

#ifdef _DEBUG
	while (!GetAsyncKeyState(VK_DELETE)) // detaching not yet finished
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
#else
	while(true) // never stop running the thread for release 
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
#endif

	FreeLibraryAndExitThread(static_cast<HMODULE>(arg), 0);
}

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		if (auto h = CreateThread(nullptr, NULL, create_session, mod, NULL, nullptr))
			CloseHandle(h);
	}

	return TRUE;
}

