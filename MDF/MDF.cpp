
#include "stdafx.h"
#include "MDF.h"

using namespace PsyPhy;
using namespace MDF;

void MDFRecord::KeepOnly( int first_marker, int last_marker ) {

	if ( last_marker < 0 ) last_marker = nMarkers - 1;
	if ( last_marker >= (int) nMarkers ) last_marker = nMarkers - 1;
	if ( first_marker < 0 ) first_marker = 0;

	unsigned int shifted, mrk;
	if ( first_marker != 0 ) {
		for ( mrk = (unsigned int) first_marker, shifted = 0; mrk <= (unsigned int) last_marker; mrk++, shifted++ ) {
			free( marker[shifted] );
			marker[shifted] = marker[mrk];
			free( iMarker[shifted] );
			iMarker[shifted] = iMarker[mrk];
			free( markerVisibility[shifted] );
			markerVisibility[shifted] = markerVisibility[mrk];
			free( markerName[shifted] );
			markerName[shifted] = markerName[mrk];
		}
	}
	for ( shifted = last_marker - first_marker + 1; shifted < nMarkers; shifted++ ) {
		free( marker[shifted] );
		free( iMarker[shifted] );
		free( markerVisibility[shifted] );
	}
	nMarkers = last_marker - first_marker + 1;
}

void MDFRecord::WriteAnalogEMT( const char *filename ) {

	FILE *fp = fopen( filename, "w" );
	if ( !fp ) {
		fMessageBox( MB_OK, "MDF", "Error openning %s for writing.", filename );
		return;
	}
	fprintf( fp, "BTS ASCII format\n\nType:         \tEmg tracks\nMeasure unit:\tmV\n\n" );
	fprintf( fp, "Tracks:       \t%d\n", nAnalogChannels );
	fprintf( fp, "Frequency:    \t%d Hz\n", analogRate );
	fprintf( fp, "Frames:       \t%d\n", nAnalogSamples );
	fprintf( fp, "Start Time:   \t%8.6f\n", 0.0 );

	fprintf( fp, "Frame\tTime" );
	for ( unsigned int channel = 0; channel < nAnalogChannels; channel++ ) fprintf( fp, "\t%s", analogChannelName[channel] );
	fprintf( fp, "\n" );

	for ( unsigned int sample = 0; sample < nAnalogSamples; sample++ ) {
		fprintf( fp, "%4d\t%8.3f", sample, (double) sample * analogInterval );
		for ( unsigned int channel = 0; channel < nAnalogChannels; channel++ ) fprintf( fp, "\t%12.8f", analog[channel][sample] );
		fprintf( fp, "\n" );
	}
	fclose( fp );
}

void MDFRecord::WriteMarkersEMT( const char *filename ) {

	FILE *fp = fopen( filename, "w" );
	if ( !fp ) {
		fMessageBox( MB_OK, "MDF", "Error openning %s for writing.", filename );
		return;
	}

	fprintf( fp, "BTS ASCII format\n\nType:         \tPoint 3D tracks\nMeasure unit:\tm\n\n" );
	fprintf( fp, "Tracks:       \t%d\n", nMarkers );
	fprintf( fp, "Frequency:    \t%d Hz\n", markerRate );
	fprintf( fp, "Frames:       \t%d\n", nMarkerSamples );
	fprintf( fp, "Start Time:   \t%8.6f\n", 0.0 );

	fprintf( fp, "Frame\tTime" );
	for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
		fprintf( fp, "\t%s.X\t%s.Y\t%s.Z", markerName[mrk], markerName[mrk], markerName[mrk] );
	}
	fprintf( fp, "\n" );

	for ( unsigned int sample = 0; sample < nMarkerSamples; sample++ ) {
		fprintf( fp, "%d\t%8.3f", sample, (double) sample * markerInterval );
		for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
			if ( markerVisibility[mrk][sample] ) fprintf( fp, "\t%8.4f\t%8.4f\t%8.4f", marker[mrk][sample][X], marker[mrk][sample][Y], marker[mrk][sample][Z] );
			else fprintf( fp, "\tNaN\tNaN\tNaN" );
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );
}
void MDFRecord::WriteAnalogASCII( const char *filename ) {

	FILE *fp = fopen( filename, "w" );
	if ( !fp ) {
		fMessageBox( MB_OK, "MDF", "Error openning %s for writing.", filename );
		return;
	}

	fprintf( fp, "sample\ttime" );
	for ( unsigned int channel = 0; channel < nAnalogChannels; channel++ ) fprintf( fp, "\t%s", analogChannelName[channel] );
	fprintf( fp, "\n" );

	for ( unsigned int sample = 0; sample < nAnalogSamples; sample++ ) {
		fprintf( fp, "%d\t%8.3f", sample, (double) sample * analogInterval );
		for ( unsigned int channel = 0; channel < nAnalogChannels; channel++ ) fprintf( fp, "\t%8.4f", analog[channel][sample] );
		fprintf( fp, "\n" );
	}
	fclose( fp );
}

void MDFRecord::WriteMarkersASCII( const char *filename ) {

	FILE *fp = fopen( filename, "w" );
	if ( !fp ) {
		fMessageBox( MB_OK, "MDF", "Error openning %s for writing.", filename );
		return;
	}

	fprintf( fp, "sample\ttime" );
	for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
		fprintf( fp, "\t%s.V\t%s.X\t%s.Y\t%s.Z", markerName[mrk], markerName[mrk], markerName[mrk], markerName[mrk] );
	}
	fprintf( fp, "\n" );

	for ( unsigned int sample = 0; sample < nMarkerSamples; sample++ ) {
		fprintf( fp, "%d\t%8.3f", sample, (double) sample * markerInterval );
		for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
			if ( markerVisibility[mrk][sample] ) fprintf( fp, "\t1\t%8.4f\t%8.4f\t%8.4f", marker[mrk][sample][X], marker[mrk][sample][Y], marker[mrk][sample][Z] );
			else fprintf( fp, "\t0\t%8.4f\t%8.4f\t%8.4f", 0.0, 0.0, 0.0 );
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );
}

void MDFRecord::FillGaps( void ) {

	unsigned int mrk, sample;
	int s_sample;
	Vector3 *highrate;
	bool	*visible;

	// Fill blanks at begining or end with first (last) valide data.
	fprintf( stderr, "Fill" );

	for ( mrk = 0; mrk < nMarkers; mrk++ ) {

		// Allocate buffers for marker data at higher rate.
		highrate = ( Vector3 * )malloc( nAnalogSamples * sizeof( *highrate ) );
		fAbortMessageOnCondition( !highrate, "MDF", "Error allocating memory for hirate marker array %d.", mrk );
		visible = (bool *) malloc( nAnalogSamples * sizeof( *visible ) );
		fAbortMessageOnCondition( !visible, "MDF", "Error allocating memory for hirate visibility array %d.", mrk );

		if ( 0 == mrk % 28 ) fprintf( stderr, " | " );
		fprintf( stderr, "." );
		for ( sample = 0; sample < nMarkerSamples; sample++ ) {
			if ( markerVisibility[mrk][sample] ) break;
		}
		if ( sample < nMarkerSamples ) {
			for ( unsigned int i = 0; i < sample; i++ ) {
				CopyVector( marker[mrk][i], marker[mrk][sample] );
				markerVisibility[mrk][i] = true;
			}
		
			for ( s_sample = (int) (nMarkerSamples - 1); s_sample >= 0; s_sample-- ) {
				if ( markerVisibility[mrk][s_sample] ) break;
			}
			if ( s_sample >= 0 ) {
				for ( unsigned int i = (unsigned) s_sample + 1; i < nMarkerSamples; i++ ) {
					CopyVector( marker[mrk][i], marker[mrk][s_sample] );
					markerVisibility[mrk][i] = true;
				}
			}

			// Now fill spaces between analog samples by linear interpolation.
			unsigned int before = 0;
			unsigned int after = 0;
			for ( sample = 0; sample < nAnalogSamples; sample++ ) {
				for ( int i = before; i * markerInterval <= sample * analogInterval; i++ ) {
					if ( markerVisibility[mrk][i] ) before = i;
				}
				for ( after = before + 1; after < nMarkerSamples && after * markerInterval >= sample * analogInterval; after++ ) {
					if ( markerVisibility[mrk][after] ) break;
				}
				double relative = (double)( sample * analogInterval - before * markerInterval ) / (double) ( ( after - before ) * markerInterval );
				Vector3 delta;
				SubtractVectors( delta, marker[mrk][after], marker[mrk][before] );
				ScaleVector( delta, delta, relative );
				AddVectors( highrate[sample], marker[mrk][before], delta );
				visible[sample] = true;
			}
		}
		else {
			// If we are here, then the marker was never visible.
			for ( sample = 0; sample < nAnalogSamples; sample++ ) {
				CopyVector( highrate[sample], zeroVector );
				visible[sample] = false;
			}
		}
		free( marker[mrk] );
		marker[mrk] = highrate;
		free( markerVisibility[mrk] );
		markerVisibility[mrk] = visible;
	}
	fprintf( stderr, "\n" );
	markerRate = analogRate;
	markerInterval = analogInterval;
	nMarkerSamples = nAnalogSamples;
}


char *MDF::HeaderTypeName[] = 
{ "TextComments", "Date", "MarkerPosition", "Analog'", "InView", "EventData", "SamplingRateMarkers", 
"SamplingRateADC", "SamplingRateEvent", "TimeScale", "GraphCursor", "MarkerResolution", "HardwareMarkers",
"Identifier", "nMarkerSamples", "nADCSamples" };

int MDFRecord::ReadDataFile( const char *filename, bool verbose ) {

	strcpy( this->filename, filename );
	if ( !strchr( filename, '.' ) ) strcat( this->filename, ".mdf" );
	FILE *fp = fopen( this->filename, "rb" );
	if ( !fp ) {
		fMessageBox( MB_OK, "MDF", "Error opening %s (%s) for read.", filename, this->filename );
		return( -1 );
	}

	char major;
	char minor;
	fread( identifier, 4, 1, fp );
	identifier[4] = '\0';
	fread( &major, sizeof( major ), 1, fp );
	fread( &minor, sizeof( minor ), 1, fp );
	sprintf( version, "%d.%03d", major, minor );
	unsigned short entries;
	fread( &entries, sizeof( entries ), 1, fp );
	nHeaderEntries = entries;

	headerEntryList = nullptr;
	MDFHeaderEntry *last_entry = nullptr;
	for ( unsigned int i = 0; i < nHeaderEntries; i++ ) {
		// Insert a new entry at the front of the linked list.
		MDFHeaderEntry *new_entry = new MDFHeaderEntry();
		if ( headerEntryList == nullptr ) headerEntryList = new_entry;
		if ( last_entry != nullptr ) last_entry->next = new_entry;
		last_entry = new_entry;
		// Read the parameters of the new header entry.
		unsigned char type;
		fread( &type, sizeof( type ), 1, fp );
		new_entry->type = type;
		unsigned char packed;
		fread( &packed, sizeof( packed ), 1, fp );
		new_entry->dimensionality = ( packed & 0xf0 ) >> 4;
		new_entry->size = ( packed & 0x0f );
		unsigned short n;
		fread( &n, sizeof( n ), 1, fp );
		new_entry->nArrays = n;
	}

	MDFHeaderEntry *entry = headerEntryList;
	unsigned short elements;
	unsigned int items_read;
	unsigned char *packedVisibility;
	unsigned short word, bit;

	do {

		// Allocate array pointers according to the entry type.
		switch ( entry->type ) {

			// For some types we do not allocate arrays of arrays, probably because we know that that type only has one array.
		case 0:	// Comment
		case 1:	// Date
		case 7: // Marker Rate
		case 8: // Force Rate
		case 9: // Analog Rate
		case 10: // Event Rate

			// For some others we just ignore the entry anyway.
		case 11: // Cursor Scale
		case 12: // Cursor Position

			break;

		case 2:	// Marker Data
			iMarker = (float **) malloc( entry->nArrays * sizeof( *iMarker ) );
			fAbortMessageOnCondition( !iMarker, "MDF", "Error allocating memory for marker array pointers." );
			break;

		case 3:	// Analog Force Data
			iForce = (short **) malloc( entry->nArrays * sizeof( *iForce ) );
			fAbortMessageOnCondition( !iForce, "MDF", "Error allocating memory for force array pointers." );
			break;

		case 4:	// Analog and EMG data
			iAnalog = (short **) malloc( entry->nArrays * sizeof( *iAnalog ) );
			fAbortMessageOnCondition( !iAnalog, "MDF", "Error allocating memory for analog array pointers." );
			break;

		case 5:	// Marker in view flags
			markerVisibility = (bool **) malloc( entry->nArrays * sizeof( *markerVisibility ) );
			fAbortMessageOnCondition( !markerVisibility, "MDF", "Error allocating memory for visibility pointers." );
			break;

		case 6:	// Events
			event = (unsigned short **) malloc( entry->nArrays * sizeof( *event ) );
			fAbortMessageOnCondition( !markerVisibility, "MDF", "Error allocating memory for event pointers." );
			break;

		case 13:	// Marker Resolution
			markerResolution = (unsigned short *) malloc( entry->nArrays * sizeof( *markerResolution ) );
			fAbortMessageOnCondition( !markerResolution, "MDF", "Error allocating memory for force resolution array." );
			break;

		case 14:	// Marker Hardware Count
			markerHardwareCount = (unsigned short *) malloc( entry->nArrays * sizeof( *markerHardwareCount ) );
			fAbortMessageOnCondition( !markerHardwareCount, "MDF", "Error allocating memory for force resolution array." );
			break;

		case 18:	// Force Resolution
			forceResolution = (float *) malloc( entry->nArrays * sizeof( *forceResolution ) );
			fAbortMessageOnCondition( !forceResolution, "MDF", "Error allocating memory for force resolution array." );
			break;

		case 19:	// Analog Resolution
			analogResolution = (float *) malloc( entry->nArrays * sizeof( *analogResolution ) );
			fAbortMessageOnCondition( !analogResolution, "MDF", "Error allocating memory for analog resolution array." );
			break;

		case 22:	// Marker Names
			markerName = (char **) malloc( entry->nArrays * sizeof( *markerName ) );
			fAbortMessageOnCondition( !markerName, "MDF", "Error allocating memory for marker name pointers." );
			break;

		case 23:	// Force Channel Names
			forceChannelName = (char **) malloc( entry->nArrays * sizeof( *forceChannelName ) );
			fAbortMessageOnCondition( !forceChannelName, "MDF", "Error allocating memory for force channel name pointers." );
			break;

		case 24:	// Analog Channel Names
			analogChannelName = (char **) malloc( entry->nArrays * sizeof( *analogChannelName ) );
			fAbortMessageOnCondition( !analogChannelName, "MDF", "Error allocating memory for analog channel name pointers." );
			break;

		default:
			if ( verbose ) printf( "Unrecognized type: %d  arrays: %d\n", entry->type, entry->nArrays );
			break;
		}

		// Now allocate each array and fill it.
		for ( unsigned int j = 0; j < entry->nArrays; j++ ) {

			items_read = fread( &elements, sizeof( elements ), 1, fp );
			fAbortMessageOnCondition( items_read != 1, "MDF", "Error reading item count for type %d.", entry->type );

			switch ( entry->type ) {

			case 0:	// Comment
				comment = (char *) malloc( (elements + 1) * entry->size );
				fAbortMessageOnCondition( !comment, "MDF", "Error allocating memory for comments." );
				items_read = fread( comment, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading comment %d.", j );
				// Make sure that the comments are null terminated.
				comment[elements] = '\0';
				break;

			case 1:	// Date
				date = (unsigned short *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !date, "MDF", "Error allocating memory for date." );
				items_read = fread( date, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading date %d.", j );
				break;

			case 2:	// Marker Data
				iMarker[j] = (float *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !iMarker[j], "MDF", "Error allocating memory for marker array %d.", j );
				items_read = fread( iMarker[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading marker data for marker %d.", j );
				nMarkers = entry->nArrays;
				nMarkerSamples = elements;
				break;

			case 3:	// Analog Force Data
				iForce[j] = (short *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !iForce[j], "MDF", "Error allocating memory for force array %d.", j );
				items_read = fread( iForce[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading force data for channel %d.", j );
				nForceChannels = entry->nArrays;
				nForceSamples = elements;
				break;

			case 4:	// Analog and EMG data
				iAnalog[j] = (short *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !iAnalog[j], "MDF", "Error allocating memory for analog array %d.", j );
				items_read = fread( iAnalog[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading analog data for channel %d.", j );
				nAnalogChannels = entry->nArrays;
				nAnalogSamples = elements;
				break;

			case 5:	// Marker in view flags
				packedVisibility = (unsigned char *) malloc( elements * entry->size );
				markerVisibility[j] = (bool *) malloc( elements * entry->size * 8 );
				fAbortMessageOnCondition( !markerVisibility[j], "MDF", "Error allocating memory for visibility array %d.", j );
				items_read = fread( packedVisibility, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading visibility data for channel %d.", j );
				for ( unsigned int pack = 0, index = 0; pack < ( elements * entry->size ); pack++ ) {
					for ( bit = 0x80; bit; bit = bit >> 1, index++ ) {
						word = packedVisibility[pack];
						markerVisibility[j][index] = ( 0 != ( word & bit ) );
					}
				}
				free( packedVisibility );
				break;

			case 6:	// Events
				event[j] = (unsigned short *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !event[j], "MDF", "Error allocating memory for visibility array %d.", j );
				items_read = fread( event[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading visibility data for event %d.", j );
				break;

			case 7:
				fAbortMessageOnCondition( ( entry->size != sizeof( markerRate ) || elements != 1 ), "MDF", "Error force rate." );
				items_read = fread( &markerRate, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading marker rate for event %d.", j );
				markerInterval = 1.0 / (double) markerRate;
				break;

			case 8:
				fAbortMessageOnCondition( ( entry->size != sizeof( forceRate) || elements != 1 ), "MDF", "Error force rate." );
				items_read = fread( &forceRate, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading force rate for event %d.", j );
				forceInterval = 1.0 / (double) forceRate;
				break;

			case 9:
				fAbortMessageOnCondition( ( entry->size != sizeof( analogRate ) || elements != 1 ), "MDF", "Error analog rate." );
				items_read = fread( &analogRate, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading analog rate for event %d.", j );
				analogInterval = 1.0 / (double) analogRate;
				break;

			case 10:
				fAbortMessageOnCondition( ( entry->size != sizeof( eventRate ) || elements != 1 ), "MDF", "Error event rate." );
				items_read = fread( &eventRate, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading analog rate for event %d.", j );
				eventInterval = 1.0 / (double) eventRate;
				break;

			case 13:
				fAbortMessageOnCondition( ( entry->size != sizeof( *markerResolution ) || elements != 1 ), "MDF", "Error reading marker resolution." );
				items_read = fread( &markerResolution[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading marker resolution for channel %d.", j );
				break;

			case 14:
				fAbortMessageOnCondition( ( entry->size != sizeof( *markerHardwareCount ) || elements != 1 ), "MDF", "Error reading marker hardware count." );
				items_read = fread( &markerHardwareCount[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading marker hardware count for channel %d.", j );
				break;

			case 18:
				fAbortMessageOnCondition( ( entry->size != sizeof( *forceResolution ) || elements != 1 ), "MDF", "Error reading force resolution." );
				items_read = fread( &forceResolution[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading force resolution for channel %d.", j );
				break;

			case 19:
				fAbortMessageOnCondition( ( entry->size != sizeof( *analogResolution ) || elements != 1 ), "MDF", "Error reading force resolution." );
				items_read = fread( &analogResolution[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading analog resolution for channel %d.", j );
				break;

			case 22:	// Marker Names
				markerName[j] = (char *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !markerName[j], "MDF", "Error allocating memory for marker name %d.", j );
				items_read = fread( markerName[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading marker name for marker %d.", j );
				for ( unsigned int i = 0; i < elements * entry->size; i++ ) {
					if ( markerName[j][i] == ' ' ) markerName[j][i] = '_';
				}
				break;

			case 23:	// Force Channel Names
				forceChannelName[j] = (char *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !forceChannelName[j], "MDF", "Error allocating memory for force channel name %d.", j );
				items_read = fread( forceChannelName[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading marker name for force channel %d.", j );
				for ( unsigned int i = 0; i < elements * entry->size; i++ ) {
					if ( forceChannelName[j][i] == ' ' ) forceChannelName[j][i] = '_';
				}
				break;

			case 24:	// Marker Names
				analogChannelName[j] = (char *) malloc( elements * entry->size );
				fAbortMessageOnCondition( !analogChannelName[j], "MDF", "Error allocating memory for analog channel name %d.", j );
				items_read = fread( analogChannelName[j], entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading marker name for analog channel %d.", j );
				for ( unsigned int i = 0; i < elements * entry->size; i++ ) {
					if ( analogChannelName[j][i] == ' ' ) analogChannelName[j][i] = '_';
				}
				break;

			default:
				char *buffer = (char * ) malloc( elements * entry->size );
				fAbortMessageOnCondition( !buffer, "MDF", "Error allocating memory for unknown arrays %d.", j );
				items_read = fread( buffer, entry->size, elements, fp );
				fAbortMessageOnCondition( items_read != elements, "MDF", "Error reading visibility data for unknown channel %d.", j );
				break;

			}
		}
		entry = entry->next;
	} while ( entry != nullptr );
	
	// Scale the marker data and turn them into vectors.
	marker = (Vector3 **) malloc( nMarkers * sizeof( *force ) );
	fAbortMessageOnCondition( !marker, "MDF", "Error allocating memory for marker pointers." );
	for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
		marker[mrk] = (Vector3 *) malloc( nMarkerSamples * sizeof( *marker[mrk] ));
		fAbortMessageOnCondition( !marker[mrk], "MDF", "Error allocating memory for marker vector array %d.", mrk );
		for ( unsigned int sample = 0; sample < nMarkerSamples; sample++ ) {
			for ( int j = 0; j < 3; j++ ) {
				marker[mrk][sample][j] = (double) iMarker[mrk][sample*3+j] * (double) markerResolution[mrk] / 1000000.0;
			}
		}
	}
	// Scale the force and analog data into floats.
	force = (float **) malloc( nForceChannels * sizeof( *force ) );
	fAbortMessageOnCondition( !force, "MDF", "Error allocating memory for analog array pointers." );
	for ( unsigned int channel = 0; channel < nForceChannels; channel++ ) {
		force[channel] = (float *) malloc( nForceSamples * sizeof( *force[channel] ));
		fAbortMessageOnCondition( !force[channel], "MDF", "Error allocating memory for analog array %d.", channel );
		for ( unsigned int sample = 0; sample < nForceSamples; sample++ ) {
			force[channel][sample] = (float) iForce[channel][sample] * forceResolution[channel];
		}
	}
	analog = (float **) malloc( nAnalogChannels * sizeof( *analog ) );
	fAbortMessageOnCondition( !analog, "MDF", "Error allocating memory for analog array pointers." );
	for ( unsigned int channel = 0; channel < nAnalogChannels; channel++ ) {
		analog[channel] = (float *) malloc( nAnalogSamples * sizeof( *analog[channel] ));
		fAbortMessageOnCondition( !analog[channel], "MDF", "Error allocating memory for analog array %d.", channel );
		for ( unsigned int sample = 0; sample < nAnalogSamples; sample++ ) {
			analog[channel][sample] = (float) iAnalog[channel][sample] * analogResolution[channel];
		}
	}


	fclose( fp );
	return ( 0 );
}
