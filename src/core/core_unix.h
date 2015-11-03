#define GetCurrentDir getcwd

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "dirent.h"
#include <termios.h>
//#include <arpa/inet.h>
//#include<netinet/in.h>
//#include<sys/socket.h>
//#include<netdb.h>

#define core_stop(fmt, ...) \
{\
	char text[10000];\
	sprintf(text,("" fmt "\n\nCallstack:\n%s [%s:%d]"),\
	##__VA_ARGS__,__FUNCTION__,__FILE__,  __LINE__);\
	printf(text);\
	exit(0);\
};


