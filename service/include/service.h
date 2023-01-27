#ifndef SERVICE_H
# define SERVICE_H

// https://learn.microsoft.com/it-it/cpp/error-messages/compiler-warnings/c5045?view=msvc-170
# pragma warning(disable : 5045)

// https://social.msdn.microsoft.com/Forums/en-US/01354cd0-f0bc-40b3-a024-4e96643fc8b8/uwpc14393-warning-c4668-win32winntwin10th2-is-not-defined-as-a-preprocessor-macro?forum=wpdevelop
# define WIN32_LEAN_AND_MEAN

# include <stdio.h>
# include <string.h>
# include <Windows.h>
# include <tlhelp32.h>

# define NAME "tinky"
# define KEYLOGGER "winkey.exe"


#endif
