
#include "stdafx.h"
#include "MDF.h"

using namespace PsyPhy;
using namespace MDF;

#define POINT_GROUP	-1
#define ANALOG_GROUP -2
#define BLOCK_SIZE	512
#define PARAMETER_BUFFER_BLOCKS	8
#define PARAMETER_BUFFER_BYTES (PARAMETER_BUFFER_BLOCKS * BLOCK_SIZE)

char *MDFRecord::Insert( char *ptr, unsigned char *block, int bytes ){
	memcpy( ptr, block, bytes );
	return( ptr + bytes );
}

char *MDFRecord::InsertParameter( char *ptr, int group, char *name, char *description, int value_size, unsigned char *value ){

	*(ptr++) = strlen( name );
	*(ptr++) = group;
	ptr = Insert( ptr, (unsigned char *) name, strlen( name ) );
	// Compute offset to next parameter block.
	// offset (short) + type (char) + dimensions (char) + description length (char) = 5
	// plus the size of the value and the size of the description.
	short offset = 5 + value_size + strlen( description ); 
	ptr = Insert( ptr, (unsigned char *) &offset, sizeof( offset ) );
	*(ptr++) = value_size;
	*(ptr++) = 0;
	ptr = Insert( ptr, value, value_size );
	char description_length = strlen( description );
	*(ptr++) = description_length;
	ptr = Insert( ptr, (unsigned char *) description, description_length );
	return( ptr );

}

char *MDFRecord::InsertParameterArray( char *ptr, int group, char *name, char *description, int value_size, int dimensions, unsigned char *array_limits, unsigned char *array ){

	*(ptr++) = strlen( name );
	*(ptr++) = group;
	ptr = Insert( ptr, (unsigned char *) name, strlen( name ) );
	// Compute offset to next parameter block.
	// offset (short) + type (char) + dimensions (char) + description length (char) = 5
	// plus the size of the array limits vector,
	// plus the size of the array of values
	// plus the size of the description.
	short offset = 5 + dimensions + strlen( description ); 
	short values = 1;
	for ( int i = 0; i < dimensions; i++ ) values *= array_limits[i];
	offset += (values * abs( value_size ) );
	ptr = Insert( ptr, (unsigned char *) &offset, sizeof( offset ) );
	*(ptr++) = value_size;
	*(ptr++) = dimensions;
	ptr = Insert( ptr, array_limits, dimensions );
	ptr = Insert( ptr, array, values * abs( value_size ) );
	char description_length = strlen( description );
	*(ptr++) = description_length;
	ptr = Insert( ptr, (unsigned char *) description, description_length );
	return( ptr );

}

char *MDFRecord::InsertStringArray( char *ptr, int group, char *name, char *description, int string_length, int n_strings, char **strings ) {
	unsigned char limits[2] = { string_length, n_strings };
	char buffer[256 * 256];
	char *local_ptr;
	int i;
	for ( i = 0,  local_ptr = buffer; i < n_strings; i++, local_ptr += string_length ) {
		// In a C3D file, all marker descriptions have to have the same length.
		// But the lengths of the marker names in the MDF file are variable length.
		// So prefill the C3D description with blanks so that what comes after the
		// actual string length is printable.
		for ( int j = 0; j < string_length; j++ ) *(local_ptr+j) = ' ';
		int bytes = strlen( strings[i] );
		memcpy( local_ptr, strings[i], bytes );
	}
	ptr = InsertParameterArray( ptr, group, name, description, -1, 2, limits, (unsigned char *) buffer );
	return( ptr );
}

void MDFRecord::WriteC3D( const char *filename ) {

	//Structure of the C3D header.
	struct {

		unsigned short	tip;
		short	markers;
		short	samples;
		short	first_3D;
		short	last_3D;
		short	max_interpolation;
		float	scaling;
		short	start_of_data_block;
		short	samples_per_frame;
		float	frame_rate;
		short	reserved1[135];
		short	labels_present;
		short	labels_block_start;
		short	quad_labels;
		short	events;
		short	reseverd2;
		float	event_times[18];
		char	event_flag[18];	
		short	reserved3;
		char	label[18][4];
		short	reserved4[22];

	} header;

	//////////////////////////////////////////////////////////////////////////////////////////////

	//// Open a known good C3D file. This is used only for reverse engineering. Should be deleted.
	//int size = sizeof( header);
	//FILE *fpg = fopen( "Eb015pr.c3d", "rb" );
	//if ( !fpg ) {
	//	fMessageBox( MB_OK, "MDF", "Error openning %s for writing.", "Eb015pr.c3d" );
	//	return;
	//}
	//// Read and discard the header from the 'good' file.
	//fread( &header, sizeof( header ), 1, fpg );


	//////////////////////////////////////////////////////////////////////////////////////////////

	header.samples_per_frame = analogRate / markerRate;
	if ( nAnalogSamples > 0 && header.samples_per_frame * markerRate != analogRate ) fAbortMessage( "MDFtoC3D", "MDFto3CD does not yet handle analog sampling frequencies that are non-integer multiples of the marker frequency." );

	// Open the output file.
	FILE *fp = fopen( filename, "wb" );
	if ( !fp ) {
		fMessageBox( MB_OK, "MDF", "Error openning %s for writing.", filename );
		return;
	}

	// Where is the data in the file? 
	// Add one for the header and another 'cause they use FORTRAN base 1.
	int start_of_data_block = PARAMETER_BUFFER_BLOCKS + 2;

	header.markers = nMarkers;

	header.samples = nAnalogChannels * header.samples_per_frame;
	header.frame_rate = markerRate;

	// It is not clear to me why one can specify the first and last frame.
	// First is always 1 for us and last frame is the number of frames. 
	header.first_3D = 1;
	header.last_3D = nMarkerFrames;

	// Required first word. Parameter block at 2 (base 1), 0x50 in upper byte per spec.
	header.tip = 0x5002;

	header.start_of_data_block = start_of_data_block; 

	// How to scale coordinates. Negative valuse means that coordinates are stored as floats.
	header.scaling = -1.0f;

	// By default, no events. I don't know if .mdf files carry events.
	header.events = 0;
	header.labels_present = 0;
	header.quad_labels = 12345;

	// CODA does not impose a maximum interpolation gap, so I put a really big number.
	header.max_interpolation = 32535;

	// Write the header to the C3D file.
	fwrite( &header, sizeof( header ), 1, fp );

	// Fill the parameter section.

	// A buffer to hold the parameter definitions.
	// I allocate one extra block to allow for as an extra buffer, then I check
	// frequently if the nominal number of blocks has been overrun.
	char parameters[PARAMETER_BUFFER_BYTES + BLOCK_SIZE];

	// Constants that get inserted into the C3D file.

	const struct {
		char skip[2];
		unsigned char number_of_parameter_blocks; 
		char processor_type;			// 84 = INTEL, 85 = DEC, 86 = MIPS
	} parameter_header = { { 0, 0 }, PARAMETER_BUFFER_BLOCKS, 84 };

	const char PointGroup[10] = { 5, POINT_GROUP, 'P', 'O', 'I', 'N', 'T', 3, 0, 0 };
	const char AnalogGroup[11] = { 6, ANALOG_GROUP, 'A', 'N', 'A', 'L', 'O', 'G', 3, 0, 0 };

	char *ptr = parameters;
	ptr = Insert( ptr, (unsigned char *) &parameter_header, sizeof( parameter_header ) );
	ptr = Insert( ptr, (unsigned char *) &PointGroup, sizeof( PointGroup ) );
	ptr = Insert( ptr, (unsigned char *) &AnalogGroup, sizeof( AnalogGroup ) );
	ptr = InsertParameterShort( ptr, abs( POINT_GROUP ), "DATA_START", "Starting block of marker data.", start_of_data_block );
	ptr = InsertParameterShort( ptr, abs( POINT_GROUP ), "USED", "Number of markers.", nMarkers );
	ptr = InsertParameterShort( ptr, abs( POINT_GROUP ), "FRAMES", "Number of marker frames.", nMarkerFrames );
	ptr = InsertParameterFloat( ptr, abs( POINT_GROUP ), "RATE", "Marker frame rate (Hz).", markerRate );
	ptr = InsertParameterFloat( ptr, abs( POINT_GROUP ), "SCALE", "Marker distance scaling (ignored because we use floating point).", -1.0 );
	ptr = InsertParameterString( ptr, abs( POINT_GROUP ), "UNITS", "Units of distance.", "mm" );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );

	// MDF files can contain marker names. As far as I know they don't also have more elaborate descriptions.
	// The C3D specification talks about 4 being the nominal size of a marker (POINT) name.
	// Here I use the MDF marker name as the description in all cases.
	// Then, according to the longLabels flag, I either uses the MDF names as the POINT:LABELS in the C3D files,
	// or I create 4 character names.
	char *marker_descriptions[256];
	int max_marker_description_length = 0;
	for ( unsigned int i = 0; i < nMarkers; i++ ) {
		int bytes = strlen( markerName[i] );
		if ( bytes > max_marker_description_length ) max_marker_description_length = bytes;
		marker_descriptions[i] = (char *) malloc( bytes + 1 ) ;
		strcpy( marker_descriptions[i], markerName[i] );
	}
	ptr = InsertStringArray( ptr, abs( POINT_GROUP ), "DESCRIPTIONS", "Marker descriptions.", max_marker_description_length, nMarkers, marker_descriptions );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );

	if ( longLabels ) {
		ptr = InsertStringArray( ptr, abs( POINT_GROUP ), "LABELS", "Long Marker Labels.", max_marker_description_length, nMarkers, marker_descriptions );
	}
	else {
		char *marker_labels[256];
		for ( unsigned int i = 0; i < nMarkers; i++ ) {
			marker_labels[i] = (char *) malloc( 8 );
			sprintf( marker_labels[i], "M%03u", i + 1 );
		}
		ptr = InsertStringArray( ptr, abs( POINT_GROUP ), "LABELS", "Short Marker Labels.", 4, nMarkers, marker_labels );
		for ( unsigned int i = 0; i < nMarkers; i++ ) free( marker_labels[i] );
	}
	for ( unsigned int i = 0; i < nMarkers; i++ ) free( marker_descriptions[i] );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );

	ptr = InsertParameterShort( ptr, abs( ANALOG_GROUP), "USED", "Number of analog channels.", nAnalogChannels );
	ptr = InsertParameterShort( ptr, abs( ANALOG_GROUP), "BITS", "Number of ADC bits.", 16 );
	ptr = InsertParameterFloat( ptr, abs( ANALOG_GROUP), "GEN_SCALE", "Global scaling of analog values.", 1.0 );
	ptr = InsertParameterFloat( ptr, abs( ANALOG_GROUP ), "RATE", "Analog sample rate (Hz).", analogRate );
	float analog_scale[256];
	short analog_offset[256];
	char  *analog_units[256];
	for ( unsigned int i = 0; i < nAnalogChannels; i++ ) {
		// We are going to store the analog data as floats, so no scaling or offset required.
		analog_scale[i] = 1.0;
		analog_offset[i] = 0;
		// There is some confusion in the CodaMotion documentation about mV versus microV.
		// I believe that the data is in mV.
		analog_units[i] = "mV";
	}
	ptr = InsertFloatArray( ptr, abs( ANALOG_GROUP), "SCALE", "Individual scaling of analog values.", nAnalogChannels, analog_scale );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );
	ptr = InsertShortArray( ptr, abs( ANALOG_GROUP), "OFFSET", "Individual offset of analog values.", nAnalogChannels, analog_offset );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );
	ptr = InsertStringArray( ptr, abs( ANALOG_GROUP ), "UNITS", "Analog data units.", 4, nAnalogChannels, analog_units );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );
	ptr = InsertParameterString( ptr, abs( ANALOG_GROUP ), "FORMAT", "Binary format of offset (SIGNED or UNSIGNED).", "SIGNED" );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );

	char *analog_descriptions[256];
	int max_analog_description_length = 0;
	for ( unsigned int i = 0; i < nAnalogChannels; i++ ) {
		int bytes = strlen( analogChannelName[i] );
		if ( bytes > max_analog_description_length ) max_analog_description_length = bytes;
		analog_descriptions[i] = (char *) malloc( bytes + 1 ) ;
		strcpy( analog_descriptions[i], analogChannelName[i] );
	}
	ptr = InsertStringArray( ptr, abs( ANALOG_GROUP ), "DESCRIPTIONS", "Channel Descriptions.", max_analog_description_length, nAnalogChannels, analog_descriptions );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );
	if ( longLabels ) {
		ptr = InsertStringArray( ptr, abs( ANALOG_GROUP ), "LABELS", "Long Channel Labels.", max_analog_description_length, nAnalogChannels, analog_descriptions );
	}
	else {
		char *analog_labels[256];
		for ( unsigned int i = 0; i < nAnalogChannels; i++ ) {
			analog_labels[i] = (char *) malloc( 8 );
			sprintf( analog_labels[i], "AD%02u", i + 1 );
		}
		ptr = InsertStringArray( ptr, abs( ANALOG_GROUP ), "LABELS", "Short Channel Labels.", 4, nAnalogChannels, analog_labels );
		for ( unsigned int i = 0; i < nAnalogChannels; i++ ) free( analog_labels[i] );
	}
	for ( unsigned int i = 0; i < nAnalogChannels; i++ ) free( analog_descriptions[i] );
	if ( ptr >= parameters + PARAMETER_BUFFER_BYTES ) fAbortMessage( "MDFtoC3D", "Parameter data exceeds parameter header size. %d %d", ptr - parameters, PARAMETER_BUFFER_BYTES );

	// Fill out with zeros. This will mark the end of the list.
	while ( ptr < ( parameters + PARAMETER_BUFFER_BYTES ) ) *(ptr++) = 0;

	// Write the parameter section to the file.
	fwrite( parameters, PARAMETER_BUFFER_BYTES, 1, fp );

	// Now write the data, frame by frame.
	// This structure holds one frame of marker data.
	// It is dimensioned to hold the max number of markers (256),
	// but only actual number of markers will be written.
	struct {
		Vector3f	position;
		unsigned short	visibility;
		unsigned short	residual;
	} local_marker[256];
	int bytes_per_frame = sizeof( *local_marker );
	// This array holds one slice of analog samples.
	// Each marker frame has header.samples_per_frame slices.
	float local_analog[256];
	unsigned int ana = 0;
	int marker_items_written;
	int analog_items_written;
	unsigned long bytes_written = 0;
	for ( unsigned int frm = 0; frm < nMarkerFrames && ana < nAnalogSamples; frm ++ ) {
		for ( unsigned int mrk = 0; mrk < nMarkers && mrk < 256; mrk++ ) {
			CopyVector( local_marker[mrk].position, marker[mrk][frm] );
			if ( markerVisibility[mrk][frm] ) local_marker[mrk].visibility = 1;
			else local_marker[mrk].visibility = 0;
			local_marker[mrk].residual = 1;
		}
		marker_items_written = fwrite( local_marker, sizeof( *local_marker ), nMarkers, fp );
		bytes_written += (sizeof( local_marker ) * marker_items_written );
		for ( int repeat = 0; repeat < header.samples_per_frame; repeat++, ana++ ) {
			for ( unsigned int chan = 0; chan < nAnalogChannels; chan++ ) local_analog[chan] = analog[chan][ana];
			analog_items_written = fwrite( local_analog, sizeof( *local_analog), nAnalogChannels, fp );
			bytes_written += ( sizeof( *local_analog) * analog_items_written );
		}
	}
	unsigned char zero = 0;
	fwrite( &zero, sizeof( zero ), bytes_written % BLOCK_SIZE, fp );
	fclose( fp );

}

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
	fprintf( fp, "Frames:       \t%d\n", nMarkerFrames );
	fprintf( fp, "Start Time:   \t%8.6f\n", 0.0 );

	fprintf( fp, "Frame\tTime" );
	for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
		fprintf( fp, "\t%s.X\t%s.Y\t%s.Z", markerName[mrk], markerName[mrk], markerName[mrk] );
	}
	fprintf( fp, "\n" );

	for ( unsigned int sample = 0; sample < nMarkerFrames; sample++ ) {
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

	for ( unsigned int sample = 0; sample < nMarkerFrames; sample++ ) {
		fprintf( fp, "%d\t%8.3f", sample, (double) sample * markerInterval );
		for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
			if ( markerVisibility[mrk][sample] ) fprintf( fp, "\t1\t%8.4f\t%8.4f\t%8.4f", marker[mrk][sample][X], marker[mrk][sample][Y], marker[mrk][sample][Z] );
			else fprintf( fp, "\t0\t%8.4f\t%8.4f\t%8.4f", 0.0, 0.0, 0.0 );
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );
}

void MDFRecord::WriteASCII( const char *filename ) {

	if ( markerRate != analogRate ) fAbortMessage( "MDF", "Cannot write single ASCII file with different rates for markers and analog samples.\nUse FillGaps() to interpolate markers to match analog rate." );
	FILE *fp = fopen( filename, "w" );
	if ( !fp ) {
		fMessageBox( MB_OK, "MDF", "Error openning %s for writing.", filename );
		return;
	}

	fprintf( fp, "frame\ttime" );
	for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) fprintf( fp, "\t%s.V\t%s.X\t%s.Y\t%s.Z", markerName[mrk], markerName[mrk], markerName[mrk], markerName[mrk] );
	for ( unsigned int channel = 0; channel < nAnalogChannels; channel++ ) fprintf( fp, "\t%s", analogChannelName[channel] );
	fprintf( fp, "\n" );

	for ( unsigned int sample = 0; sample < nMarkerFrames; sample++ ) {
		fprintf( fp, "%d\t%8.3f", sample, (double) sample * markerInterval );
		for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
			if ( markerVisibility[mrk][sample] ) fprintf( fp, "\t1\t%8.4f\t%8.4f\t%8.4f", marker[mrk][sample][X], marker[mrk][sample][Y], marker[mrk][sample][Z] );
			else fprintf( fp, "\t0\t%8.4f\t%8.4f\t%8.4f", 0.0, 0.0, 0.0 );
		}
		for ( unsigned int channel = 0; channel < nAnalogChannels; channel++ ) fprintf( fp, "\t%8.4f", analog[channel][sample] );
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
		for ( sample = 0; sample < nMarkerFrames; sample++ ) {
			if ( markerVisibility[mrk][sample] ) break;
		}
		if ( sample < nMarkerFrames ) {
			for ( unsigned int i = 0; i < sample; i++ ) {
				CopyVector( marker[mrk][i], marker[mrk][sample] );
				markerVisibility[mrk][i] = true;
			}
		
			for ( s_sample = (int) (nMarkerFrames - 1); s_sample >= 0; s_sample-- ) {
				if ( markerVisibility[mrk][s_sample] ) break;
			}
			if ( s_sample >= 0 ) {
				for ( unsigned int i = (unsigned) s_sample + 1; i < nMarkerFrames; i++ ) {
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
				for ( after = before + 1; after < nMarkerFrames && after * markerInterval >= sample * analogInterval; after++ ) {
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
	nMarkerFrames = nAnalogSamples;
}


char *MDF::HeaderTypeName[] = 
{ "TextComments", "Date", "MarkerPosition", "Analog'", "InView", "EventData", "SamplingRateMarkers", 
"SamplingRateADC", "SamplingRateEvent", "TimeScale", "GraphCursor", "MarkerResolution", "HardwareMarkers",
"Identifier", "nMarkerFrames", "nADCSamples" };

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
	unsigned int nHeaderEntries = entries;
	// Here we create a linked list of header entries.
	// The class MDFHeaderEntry contains the information about each entry,
	// plus pointers to create the linked list.
	MDFHeaderEntry *headerEntryList = nullptr;				// Head of the list.
	MDFHeaderEntry *last_entry = nullptr;	// Tail of the list.
	for ( unsigned int i = 0; i < nHeaderEntries; i++ ) {
		// I want to keep the entries in the original order, so new 
		// entries will be added to the end of the list.
		// Insert a new entry at the front of the linked list.
		MDFHeaderEntry *new_entry = new MDFHeaderEntry();
		// If the list is empty, the new entry becomes the head of the list.
		if ( headerEntryList == nullptr ) headerEntryList = new_entry;
		// If the list is not empty ( last_entry is not null) then the new 
		// entry gets tacked on the end of the list.
		if ( last_entry != nullptr ) last_entry->next = new_entry;
		// Then the new entry becomes the last entry in the list.
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

	unsigned short elements;
	unsigned int items_read;
	unsigned char *packedVisibility;
	unsigned short word, bit;

	// Step through the header entries to retrieve the data, starting at the head of the list.
	while ( headerEntryList != nullptr ) {

		MDFHeaderEntry *entry = headerEntryList;

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
				nMarkerFrames = elements;
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
				fAbortMessageOnCondition( ( entry->size != sizeof( *analogResolution ) || elements != 1 ), "MDF", "Error reading analog resolution." );
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
		headerEntryList = entry->next;
		delete entry;
	}
	
	// Scale the marker data and turn them into vectors.
	marker = (Vector3 **) malloc( nMarkers * sizeof( *force ) );
	fAbortMessageOnCondition( !marker, "MDF", "Error allocating memory for marker pointers." );
	for ( unsigned int mrk = 0; mrk < nMarkers; mrk++ ) {
		marker[mrk] = (Vector3 *) malloc( nMarkerFrames * sizeof( *marker[mrk] ));
		fAbortMessageOnCondition( !marker[mrk], "MDF", "Error allocating memory for marker vector array %d.", mrk );
		for ( unsigned int sample = 0; sample < nMarkerFrames; sample++ ) {
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
