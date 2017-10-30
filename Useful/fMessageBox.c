// Make it easier to construct messages and display them as a MessageBox.

// Disable warnings about unsafe functions.
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#include "fMessageBox.h"
 
int fMessageBox( int mb_type, const char *caption, const char *format, ... ) {
	
	int items;
	
	va_list args;
	
	// The character buffer is really long so that there is little chance of overrunning it.
	char message[10240];
	
	va_start(args, format);
	items = vsprintf(message, format, args);
	va_end(args);
	
	return( MessageBox( NULL, message, caption, mb_type ) );
		
}

void fAbortMessageOnCondition( int condition, const char *caption, const char *format, ... ) {

	va_list args;
	int items;

	// The character buffer is really long so that there is little chance of overrunning it.
	char message[10240];
	
	va_start(args, format);
	items = vsprintf(message, format, args);
	va_end(args);

	if ( condition ) {
		MessageBox( NULL, message, caption, MB_OK | MB_ICONEXCLAMATION );
		exit( -1 );
	}
}

void fAbortMessage( const char *caption, const char *format, ... ) {

	va_list args;
	int items;

	// The character buffer is really long so that there is little chance of overrunning it.
	char message[10240];
	
	va_start(args, format);
	items = vsprintf(message, format, args);
	va_end(args);

	MessageBox( NULL, message, caption, MB_OK | MB_ICONEXCLAMATION );
	exit( -1 );

}