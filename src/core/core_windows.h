#define GetCurrentDir _getcwd
#define WIN32_DEFAULT_LIBS

#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#include <direct.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"winspool.lib")
#pragma comment(lib,"comdlg32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")
#pragma comment(lib,"uuid.lib")
#pragma comment(lib,"odbc32.lib")
#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"strmiids.lib")

#include "dirent_win.h"
#include "stackwalker/StackWalker.h"

#define core_stop(fmt, ...) \
{\
	StackWalker sw; \
	char* callstack=sw.ShowCallstack(); \
	char text[10000];\
	sprintf(text,("" fmt "\n\nCallstack:\n%s [%s:%d]\n%s"),\
	##__VA_ARGS__,__FUNCTION__,__FILE__,  __LINE__, callstack);\
	printf(text);\
	MessageBoxA(0,text,"Error",0); ::exit(0);\
	while(1);;\
};