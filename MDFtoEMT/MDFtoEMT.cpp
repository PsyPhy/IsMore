// MDFtoEMT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../MDF/MDF.h"


int main(int argc, char *argv[])
{
	int return_code;
	MDF::MDFRecord *mdf;
	char filename[1024];
	char *ptr;

	for ( int arg = 1; arg < argc; arg++ ) {

		strcpy( filename, argv[arg] );
		fprintf( stderr, "%s ... ", filename );
		mdf = new MDF::MDFRecord();
		return_code = mdf->ReadDataFile( filename );
		if ( !return_code ) {

			// Keep only the marker computed from the combined set of CODAs;
			mdf->KeepOnly( 0, 27 );

			if ( ptr = strstr( filename, ".mdf" ) ) strcpy( ptr, "_mrk.emt" );
			else strcat( filename, "_mrk.emt" );
			mdf->WriteMarkersEMT( filename );
			fprintf( stderr, "\n  %s ... ", filename );
			if ( ptr = strstr( filename, "_mrk.emt" ) ) strcpy( ptr, "_emg.emt" );
			else strcat( filename, "_emg.emt" );
			mdf->WriteAnalogEMT( filename );
			fprintf( stderr, "\n  %s ... 0K\n", filename );

		}
		else fprintf( stderr, " failed.\n" );
		delete mdf;
	}

	return 0;
}

