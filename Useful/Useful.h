#pragma once

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define MAX_PATHLENGTH	1024

// Useful constants for accessing components of 
//  vectors and quaternions.
#define X	0
#define Y	1
#define Z	2
#define M	3

#define ROLL	0
#define PITCH	1
#define YAW		2

static unsigned long __nan = 0x7ff7ffff;
#define NaN				(*((double *) &__nan))

#define MISSING_DOUBLE	999999.999999
#define MISSING_FLOAT	999999.999999f
#define MISSING_INT		((int)(MISSING_DOUBLE))
#define MISSING_CHAR	127

#define Pi	M_PI
#define PI Pi
#define DegreesToRadians(x) ((x) * PI / 180.0)
#define RadiansToDegrees(x) ((x) * 180.0 / PI)

#define UNDEFINED	-1
#define local		static

#ifndef __cplusplus
#define FALSE 0
#define TRUE  1
#endif

#define YES	TRUE
#define NO	FALSE
#define ESCAPE 0x1B

// Some useful constants.

#define NORMAL_EXIT 0
#define ESCAPE_EXIT 1
#define IGNORE_EXIT 2
#define ERROR_EXIT 3
#define ABORT_EXIT 4
#define RETRY_EXIT 5

// Check if a file exists. Often used to check for 
// the presence of a "cookie" file.
#define FileExists(fn) ( 0 == _access_s( fn, 0x00 ))

// I define here Vector, Quaternion and Matrix data types.
// I use them in both standard C code and in C++ code.
// The problem is that these types are also defined elswhere in 
// Windows Forms packages. To make them unique, I want to put them
// in my own namespace. But that cannot work in standard C code.
// Thus the #ifdef here is to make this header compatible with
// both my C and my C++ code.

#ifdef __cplusplus
namespace PsyPhy {
#endif

typedef double Vector3[3];
typedef float  Vector3f[3];		// This should go away in favor of fVector3.
typedef float  fVector3[3];
typedef double Matrix3x3[3][3];
typedef float  fMatrix3x3[3][3];
typedef double Quaternion[4];
typedef float  fQuaternion[4];

#ifdef __cplusplus
}
#endif

