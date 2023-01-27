#include "service.h"

// https://social.msdn.microsoft.com/Forums/vstudio/en-US/89a4a707-778f-4e64-96d3-ca4a6c89f709/linking-error-lnk2019?forum=vclanguage
#pragma comment(lib, "advapi32.lib")

void	install(void)
{
	printf("install\n");
}

void	start(void)
{
	printf("start\n");
}

void	stop(void)
{
	printf("stop\n");
}

void	delete(void)
{
	printf("delete\n");
}

void	get_error(char *msg)
{
	DWORD	errorMessageID = GetLastError();
	LPSTR	messageBuffer = NULL;
	size_t	size;
	char	*message;

	if(!errorMessageID)
		return ;
	size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	message = messageBuffer;
	printf("%s %s\n", msg, message);
	LocalFree(messageBuffer);
}

int	main(int argc, char **argv)
{
	int			i;
	void		(*operations[4])(void) = { install, start, stop, delete };
	char		*str_operations[4] = { "install", "start", "stop", "delete" };
	SC_HANDLE	manager;

	if (argc !=2)
	{
		printf("Usage: svc.exe [instsall | start | stop | delete]\n");
		return (1);
	}
	manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!manager)
	{
		get_error("Error: OpenSCManager: ");
		return (1);
	}
	i = -1;
	while (++i < 4)
	{
		if (!strcmp(str_operations[i], argv[1]))
			operations[i]();
	}
	CloseServiceHandle(manager);
	return (0);
}
