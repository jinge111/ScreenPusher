#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if (defined(_WIN32) || defined(_WIN64))
#define strcpy_m strcpy_s 
#define strncpy_m strncpy_s
#define vsprintf_m vsprintf_s
#define sprintf_m sprintf_s
#define strcat_m strcat_s
#define access_m _access
#else
#define strcpy_m strcpy
#define strncpy_m strncpy
#define vsprintf_m vsprintf
#define sprintf_m sprintf
#define strcat_m strcat
#define access_m access
#endif

#endif