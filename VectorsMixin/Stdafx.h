// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

// Disable warnings about unsafe functions such as sprintf().
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6.
// Also, I am not convinced that the 'safe' ones are all that much safer. For instance, you 
//  are supposed to specify how many parameters there are in the sprintf format string, but
//  you can just as easily get that wrong as to not pass the right number of parameters to
//  the 'unsafe' version.
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
