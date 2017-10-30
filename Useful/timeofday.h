#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64 

#ifndef _WINSOCKAPI_
// MSVC defines this in winsock2.h!?
typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;
#endif


#ifdef __cplusplus
extern "C" {
#endif

int gettimeofday(struct timeval * tp, struct timezone * tzp);

#ifdef __cplusplus
}
#endif