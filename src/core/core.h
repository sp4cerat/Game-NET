#pragma once

#define _USE_MATH_DEFINES
#define GLM_FORCE_RADIANS
//_______________________________________________________________//
// System
#ifdef __CYGWIN__
#define SYSTEM_UNIX
#endif
#ifdef __linux__
#define SYSTEM_UNIX
#endif
#ifdef WIN32
#ifndef __CYGWIN__
#define SYSTEM_WINDOWS
#endif
#endif

#include <enet/enet.h>

//_______________________________________________________________//
// Includes
#ifdef SYSTEM_WINDOWS
#include "core_windows.h"
#endif
#ifdef SYSTEM_UNIX
#include "core_unix.h"
#endif
//_______________________________________________________________//
//#define WGL_EXT_swap_control

#include <thread>
#include <set>
#include <malloc.h>
#include <stdio.h>
#include <fcntl.h>
//#include <io.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <utility> //pair
#include <algorithm>    // std::sort
#include <string>
#include <fstream>
#include <tuple>
#include <istream>
#include <functional>

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
//_______________________________________________________________//
// Functions

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int

#define loopi(start_l,end_l) for ( int i=start_l;i<end_l;++i )
#define loopj(start_l,end_l) for ( int j=start_l;j<end_l;++j )
#define loopk(start_l,end_l) for ( int k=start_l;k<end_l;++k )
#define loopl(start_l,end_l) for ( int l=start_l;l<end_l;++l )
#define loopm(start_l,end_l) for ( int m=start_l;m<end_l;++m )
#define loopn(start_l,end_l) for ( int n=start_l;n<end_l;++n )
#define loop(a_l,start_l,end_l) for ( int a_l = start_l;a_l<end_l;++a_l )
//_______________________________________________________________//
#define loopij(_sti,_stj,_eni,_enj) loopi(_sti,_eni)loopj (_stj,_enj)
#define loopijk(_sti,_stj,_stk,_eni,_enj,_enk) loopi(_sti,_eni) loopj (_stj,_enj) loopk (_stk,_enk)
#define looplmn(_stl,_stm,_stn,_enl,_enm,_enn) loopl(_stl,_enl) loopm (_stm,_enm) loopn (_stn,_enn)
//_______________________________________________________________//
#define vswap(a,b) { auto c=a;a=b;b=c; }
#define frac(a) (a-floor(a))
//_______________________________________________________________//
#define ifeq(sa,sb) if( 0==strncmp(sa,sb,min(strnlen(sa,1000),strnlen(sb,1000)) )) 
//_______________________________________________________________//
#define ifeq_lower(a,b) if( strnlen(a,1000)>0 && strnlen(b,1000)>0 && core_strcmp_lower(a,b)) 
#define ifexist( fname ) if( core_file_exists(fname) )
#define ifnotexist( fname ) if( !core_file_exists(fname) )
//_______________________________________________________________//
float core_cubicInterpolate(float p[4], float x);
float core_bicubicInterpolate(float p[4][4], float x, float y);
std::string core_pure_filename ( std::string filename );
std::string core_get_path ( std::string filename );
//std::string int_to_str(const int x);
char* str( const char* format, ... );
bool core_file_exists(char* f);
std::string core_current_dir();
void core_get_dir(const char* dirname,
			 std::vector<std::string>	&listdirs,
			 std::vector<std::string>	&listfiles,
			 std::vector<long>			&listfilesize,
			 char* extension);
FILE* core_fopen(const char *name, const char *mode);
int core_splitstr(char* s, char** sp1, char** sp2, char** sp3);
std::vector<uchar> core_read_file(const char* filename);
//_______________________________________________________________//
void core_console_draw( std::string buffer );
int  core_keyb();
void core_sleep(unsigned int msec);
unsigned int core_time();
//_______________________________________________________________//
template<class T>
std::string core_to_string(T i)
{
    std::stringstream ss; ss << i;
    return ss.str();
}
//_______________________________________________________________//
template <class T, class OWNER>
class readonly
{
	friend OWNER;
public:
	explicit readonly() {}
	explicit readonly(const T &t) : m_t(t){}
	~readonly(){}
	operator const T&() const{ return m_t; }

private:
	T& operator =(const T &t)	{ m_t = t; return m_t; }
	T m_t;
};
//_______________________________________________________________//