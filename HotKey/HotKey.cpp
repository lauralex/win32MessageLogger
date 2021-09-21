// HotKey.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include "HookDll.h"
#include <limits>



int main()
{
	std::string window_title;
	std::cout << "Enter a window title: ";
	std::cin >> window_title;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	auto hwnd = FindWindowExA(NULL, NULL, NULL, window_title.c_str());

	DWORD thread_id = GetWindowThreadProcessId(hwnd, NULL);
	if (thread_id == 0) return -1;
	HINSTANCE hook_dll_handle = GetModuleHandleA("HookDll");
	auto hook = SetWindowsHookExA(WH_GETMESSAGE, message_hook, hook_dll_handle, thread_id);
	std::cout << "Press enter to stop then wait for \"ciccio\"" << std::endl;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	UnhookWindowsHookEx(hook);
	return 0;
}
