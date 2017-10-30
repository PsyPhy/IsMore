#pragma once

#ifdef __cplusplus
extern "C" {
#endif

BOOL CALLBACK CloseCallback( HWND hwnd, LPARAM lParam );
void MinimizeAllWindows( void );
void GiveExclusiveFocus( HWND hwnd );
void PhantomLeftClick( void );

#ifdef __cplusplus
}
#endif