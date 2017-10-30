#pragma once
#include <stdio.h>
#include <stdlib.h>

#define iswhite( c ) ( c <= 0x20 || c >= 0x7f )

#define MAX_TOKENS	32
#define PARSER_BUFFERS	16

#ifdef __cplusplus
extern "C" {
#endif

int ParseCommaDelimitedLine ( char *tokens[MAX_TOKENS], const char *line );

#ifdef __cplusplus
}
#endif
