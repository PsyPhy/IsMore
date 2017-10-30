#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "useful.h"

#include "ParseCommaDelimitedLine.h"

static char return_tokens[PARSER_BUFFERS][1024];
static int circular = 0;

int ParseCommaDelimitedLine ( char *tokens[MAX_TOKENS], const char *line ) {


	char *tkn, *chr;
	unsigned int	 n = 0, i, j;

	if ( strlen( line ) > sizeof( return_tokens[circular] ) ) {
		fprintf( stderr, "Line too long.\n%s\n", line );
		exit( -1 );
	}

	// Copy and replace escapted commas with semicolons. 
	for ( i = 0, chr = return_tokens[circular]; i < strlen( line ); i++ ) {
		if ( line[i] == '\\' && line[i+1] == ',' ) {
			*chr++ = ';';
			i++;
		}
		else  *chr++ = line[i];
	}
	*chr = 0;
	
	tkn = strtok( return_tokens[circular], "," );
	while ( tkn && n < MAX_TOKENS - 1 ) {
		/* Skip to first non-white character. */
		while ( iswhite( *tkn ) && *tkn ) tkn++;
		/* Strip off any trailing whitespace. */
		chr = tkn + strlen( tkn );
		while ( iswhite( *chr ) && chr >= tkn ) *chr-- = 0;
		/* Break out if we have hit a comment or the end of the line. */
		if ( *tkn == '#' || *tkn == 0 ) break;
		/* Record this as a valid token. */
		tokens[n++] = tkn;
		/* Parse for the next one. */
		tkn = strtok( NULL, "," );
	}

	/* Last token shall be a null pointer by definition. */
	tokens[n] = NULL;
	// Convert semicolons back to commas.
	for ( i = 0; i < n; i++ ) {
		for ( j = 0; j < strlen( tokens[i] ); j++ ) if ( tokens[i][j] == ';' ) tokens[i][j] = ',';
	}

	/* Next time around use a different buffer for the strings. */
	circular = ( circular + 1 ) % PARSER_BUFFERS;

	return( n );
}

