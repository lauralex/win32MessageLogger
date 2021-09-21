// HookDll.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "HookDll.h"
#include "message_mapper.h"

#include <iostream>
#include <ostream>
#include <string>

extern HANDLE std_out_con;

bool console_created = false;

message_mapper mapper;

HOOKDLL_API LRESULT CALLBACK message_hook(int code, WPARAM wParam, LPARAM lParam)
{
    if (!console_created)
    {
		STARTUPINFOA startupinfo;
		PROCESS_INFORMATION p_info;

		ZeroMemory(&startupinfo, sizeof(startupinfo));
		startupinfo.cb = sizeof(startupinfo);
		ZeroMemory(&p_info, sizeof(p_info));
		auto command_line = static_cast<LPSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CHAR) * 50));

		LPCSTR command_l = "cmd";
		CopyMemory(command_line, command_l, strlen(command_l) + 1);
		BOOL res = CreateProcessA(NULL, command_line, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startupinfo, &p_info);
		FreeConsole();
		Sleep(2000);
		BOOL attach_res = AttachConsole(p_info.dwProcessId);
		CloseHandle(p_info.hProcess);
		CloseHandle(p_info.hThread);
		DWORD err = GetLastError();
		std_out_con = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		SMALL_RECT scrollRect;
		COORD scrollTarget;
		CHAR_INFO fill;

		// Get the number of character cells in the current buffer.
		if (!GetConsoleScreenBufferInfo(std_out_con, &csbi))
		{
			return 0;
		}

		// Scroll the rectangle of the entire buffer.
		scrollRect.Left = 0;
		scrollRect.Top = 0;
		scrollRect.Right = csbi.dwSize.X;
		scrollRect.Bottom = csbi.dwSize.Y;

		// Scroll it upwards off the top of the buffer with a magnitude of the entire height.
		scrollTarget.X = 0;
		scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

		// Fill with empty spaces with the buffer's default text attribute.
		fill.Char.UnicodeChar = TEXT(' ');
		fill.Attributes = csbi.wAttributes;

		// Do the scroll
		ScrollConsoleScreenBuffer(std_out_con, &scrollRect, NULL, scrollTarget, &fill);

		// Move the cursor to the top left corner too.
		csbi.dwCursorPosition.X = 0;
		csbi.dwCursorPosition.Y = 0;

		SetConsoleCursorPosition(std_out_con, csbi.dwCursorPosition);
		console_created = true;
    }
    if (code == HC_ACTION)
    {
        MSG* msg = (MSG*)lParam;
		std::string msg_str = mapper(msg->message, msg->lParam, msg->wParam);
		if (msg_str.length() != 0)
		{
			WriteConsoleA(std_out_con, msg_str.c_str(), msg_str.size(), NULL, NULL);
		}
        
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}


