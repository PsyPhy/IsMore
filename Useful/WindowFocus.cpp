#include <Windows.h>
#include "WindowFocus.h"

BOOL CALLBACK CloseCallback( HWND hwnd, LPARAM lParam ) {
	ShowWindow( hwnd, SW_FORCEMINIMIZE );
	Sleep( 100 );
	return( true );
}

void MinimizeAllWindows( void ) {
	EnumWindows( CloseCallback, NULL );
}

void GiveExclusiveFocus( HWND hwnd ) {

	// Minimize all the windows.
	MinimizeAllWindows();
	// Restore the specified window.
	ShowWindow( hwnd, SW_RESTORE );
	// Try to give it focus.
	SwitchToThisWindow( hwnd, false );

}

// Simulate a click of the left mouse button.
// Use with caution. The click is not restricted to the
// calling process. 
void PhantomLeftClick( void ) {

	INPUT MouseLeftInput;

	MouseLeftInput.mi.time = 0;
	MouseLeftInput.mi.dx = 0;
	MouseLeftInput.mi.dy = 0;
	MouseLeftInput.mi.mouseData = 0;
	MouseLeftInput.type = INPUT_MOUSE;

	MouseLeftInput.mi.dwFlags =   MOUSEEVENTF_LEFTDOWN;
	SendInput( 1, &MouseLeftInput, sizeof( MouseLeftInput ) );
	Sleep( 100 );
	MouseLeftInput.mi.dwFlags =   MOUSEEVENTF_LEFTUP;
	SendInput( 1, &MouseLeftInput, sizeof( MouseLeftInput ) );

}


