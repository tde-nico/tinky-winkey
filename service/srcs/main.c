#include "service.h"

// https://social.msdn.microsoft.com/Forums/vstudio/en-US/89a4a707-778f-4e64-96d3-ca4a6c89f709/linking-error-lnk2019?forum=vclanguage
#pragma comment(lib, "advapi32.lib")

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

void	install(SC_HANDLE manager)
{
	char		path[MAX_PATH];
	SC_HANDLE	service;

	GetModuleFileName(NULL, path, MAX_PATH);
	service = CreateService(manager, NAME, NAME, SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		path, NULL, NULL, NULL, NULL, NULL);
	if (!service)
	{
		get_error("Error: Installation: ");
		return ;
	}
	CloseServiceHandle(service);
	printf("Service <%s> installed successfully\n", NAME);
}

void	start(SC_HANDLE manager)
{
	CloseServiceHandle(manager);
	printf("Service <%s> started successfully\n", NAME);
}

void	stop(SC_HANDLE manager)
{
	CloseServiceHandle(manager);
	printf("Service <%s> stopped successfully\n", NAME);
}

void	delete(SC_HANDLE manager)
{
	SC_HANDLE		service;
	SERVICE_STATUS	status;

	service = OpenService(manager, NAME, SERVICE_ALL_ACCESS);
	if (!service)
	{
		get_error("Error: Deleting: ");
		return ;
	}
	QueryServiceStatus(service, &status);
	if (status.dwCurrentState != SERVICE_STOPPED)
	{
		CloseServiceHandle(service);
		printf("Service <%s> running, please stop\n", NAME);
		return ;
	}
	if (!DeleteService(service))
	{
		get_error("Error: DeleteService: ");
		return ;
	}
	CloseServiceHandle(service);
	printf("Service <%s> deleted successfully\n", NAME);
}

int	main(int argc, char **argv)
{
	int			i;
	void		(*operations[4])(SC_HANDLE) = { install, start, stop, delete };
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
			operations[i](manager);
	}
	CloseServiceHandle(manager);
	return (0);
}
