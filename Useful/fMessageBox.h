#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int fMessageBox( int mb_type, const char *caption, const char *format, ... );
void fAbortMessageOnCondition( int condition, const char *caption, const char *format, ... );
void fAbortMessage( const char *caption, const char *format, ... );

#ifdef __cplusplus
}
#endif