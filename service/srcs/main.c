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
	printf("%s %ld: %s\n", msg, errorMessageID, message);
	LocalFree(messageBuffer);
	return (1);
}

int	install(SC_HANDLE manager)
{
	char		path[MAX_PATH];
	SC_HANDLE	service;

	GetModuleFileName(0, path, MAX_PATH);
	service = CreateService(manager, NAME, NAME, SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		path, 0, 0, 0, 0, 0);
	if (!service)
		return (get_error("Error: CreateService:"));
	CloseServiceHandle(service);
	printf("Service {%s} installed successfully\n", NAME);
	return (0);
}

int	start(SC_HANDLE manager)
{
	SC_HANDLE		service;
	char			path[MAX_PATH];

	service = OpenService(manager, NAME, SERVICE_START);
	if (!service)
		return (get_error("Error: OpenService:"));
	GetFullPathName(".\\winkey.exe", MAX_PATH, path, NULL);
	if (!StartService(service, 1, (char *[]){ path }))
		return (get_error("Error: StartService:"));
	CloseServiceHandle(service);
	printf("Service {%s} started successfully\n", NAME);
	return (0);
}

int	stop(SC_HANDLE manager)
{
	SC_HANDLE		service;
	SERVICE_STATUS	status;

	service = OpenService(manager, NAME, SERVICE_STOP);
	if (!service)
		return (get_error("Error: OpenService:"));
	if (!ControlService(service, SERVICE_CONTROL_STOP, &status))
		return (get_error("Error: ControlService:"));
	CloseServiceHandle(service);
	printf("Service {%s} stopped successfully\n", NAME);
	return (0);
}

int	delete(SC_HANDLE manager)
{
	SC_HANDLE		service;
	SERVICE_STATUS	status;

	service = OpenService(manager, NAME, SERVICE_ALL_ACCESS);
	if (!service)
		return (get_error("Error: OpenService:"));
	QueryServiceStatus(service, &status);
	if (status.dwCurrentState != SERVICE_STOPPED
		&& !ControlService(service, SERVICE_CONTROL_STOP, &status))
		return (get_error("Error: ControlService:"));
	if (!DeleteService(service))
		return (get_error("Error: DeleteService:"));
	CloseServiceHandle(service);
	printf("Service {%s} deleted successfully\n", NAME);
	return (0);
}

int	update(SC_HANDLE manager)
{
	SC_HANDLE			service;
	SERVICE_DESCRIPTION	desc;

	service = OpenService(manager, NAME, SERVICE_ALL_ACCESS);
	if (!service)
		return (get_error("Error: OpenService:"));
	desc.lpDescription = "this is a SAFE service";
	if (!ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &desc))
		return (get_error("Error: ChangeServiceConfig2:"));
	CloseServiceHandle(service);
	printf("Service {%s} updated successfully\n", NAME);
	return (0);
}

int	main(int argc, char **argv)
{
	int			i;
	int			(*operations[5])(SC_HANDLE) = { install, start, stop, delete, update };
	char		*str_operations[5] = { "install", "start", "stop", "delete", "update" };
	SC_HANDLE	manager;

	if (argc == 1 && !dispatch_service())
		return (printf("Usage: %s [install | start | stop | delete | update]\n", argv[0]));
	manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
	if (!manager)
		return (get_error("Error: OpenSCManager:"));
	i = -1;
	while (++i < 5)
	{
		if (!strcmp(str_operations[i], argv[1]))
			operations[i](manager);
	}
	CloseServiceHandle(manager);
	return (0);
}
