#pragma once
// Disable warnings about unsafe functions such as sprintf().
// We use the 'unsafe' versions to maintain source-code compatibility with Visual C++ 6.
// Also, I am not convinced that the 'safe' ones are all that much safer. For instance, you 
//  are supposed to specify how many parameters there are in the sprintf format string, but
//  you can just as easily get that wrong as to not pass the right number of parameters to
//  the 'unsafe' version.
#define _CRT_SECURE_NO_WARNINGS

#include "..\Useful\Useful.h"
#include "..\VectorsMixin\VectorsMixin.h"

#define MDF_FILEOPEN_ERROR	-2
#define MDF_FILEREAD_ERROR	-3

#define MDF_MAX_HEADER_ENTRIES	64

namespace MDF {

	extern char *HeaderTypeName[];

	class MDFHeaderEntry {
	public:
		unsigned int	type;
		unsigned int	size;
		unsigned int	dimensionality;
		unsigned int	nArrays;

		MDFHeaderEntry *next;
		MDFHeaderEntry (void ) {
			this->next = nullptr;
		}
		
	};
	
	class MDFRecord : public PsyPhy::VectorsMixin {

	public:

		char filename[1024];

		char identifier[8];
		char version[16];

		bool longLabels;

		char *comment;
		unsigned short *date;

		float	**iMarker;
		PsyPhy::Vector3	**marker;
		bool	**markerVisibility;
		unsigned int	nMarkers;
		unsigned int	nMarkerFrames;
		unsigned short	markerRate;
		double			markerInterval;
		unsigned short	*markerResolution;
		unsigned short	*markerHardwareCount;
		char			**markerName;

		short	**iForce;
		float	**force;
		unsigned int	nForceChannels;
		unsigned int	nForceSamples;
		unsigned short	forceRate;
		double			forceInterval;
		float			*forceResolution;
		char			**forceChannelName;

		short	**iAnalog;
		float	**analog;
		unsigned int	nAnalogChannels;
		unsigned int	nAnalogSamples;
		unsigned short	analogRate;
		double			analogInterval;
		float			*analogResolution;
		char			**analogChannelName;

		unsigned short	**event;
		unsigned short	eventRate;
		double			eventInterval;

	private:

		unsigned int	nHeaderEntries;
		MDFHeaderEntry *headerEntryList;

	public:
		int		ReadDataFile( const char *filename, bool verbose = false );
		void	WriteMarkersASCII( const char *filename );
		void	WriteAnalogASCII( const char *filename );
		void	WriteMarkersEMT( const char *filename );
		void	WriteAnalogEMT( const char *filename );
		void	KeepOnly( int first_marker, int last_marker = -1 );
		void	FillGaps( void );

		void	WriteC3D( const char *filename );
		char	*Insert( char *ptr, unsigned char *block, int bytes );
		char	*InsertParameter( char *ptr, int group, char *name, char *description, int value_size, unsigned char *value );
		char	*InsertParameterShort( char *ptr, int group, char *name, char *description, int value ) {
			unsigned short short_value = value;
			return( InsertParameter( ptr, group, name, description, sizeof( short_value ), (unsigned char *) &short_value ) );
		}
		char	*InsertParameterByte( char *ptr, int group, char *name, char *description, int value ) {
			unsigned char byte_value = value;
			return( InsertParameter( ptr, group, name, description, sizeof( byte_value ), (unsigned char *) &byte_value ) );
		}
		char	*InsertParameterFloat( char *ptr, int group, char *name, char *description, float value ) {
			float float_value = value;
			return( InsertParameter( ptr, group, name, description, sizeof( float_value ), (unsigned char *) &float_value ) );
		}
		char *InsertParameterArray( char *ptr, int group, char *name, char *description, int value_size, int dimensions, unsigned char *array_limits, unsigned char *array );
		char *InsertFloatArray( char *ptr, int group, char *name, char *description, int n_values, float *values ) {
			unsigned char c_values = n_values;
			return( InsertParameterArray( ptr, group, name, description, sizeof( float ), 1, &c_values, (unsigned char *) values ) );
		}
		char *InsertShortArray( char *ptr, int group, char *name, char *description, int n_values, short *values ) {
			unsigned char c_values = n_values;
			return( InsertParameterArray( ptr, group, name, description, sizeof( short ), 1, &c_values, (unsigned char *) values ) );
		}

		char *InsertStringArray( char *ptr, int group, char *name, char *description, int string_length, int n_strings, char **strings );
		char *InsertParameterString( char *ptr, int group, char *name, char *description, char *value ) {
			int length = strlen( value );
			return( InsertStringArray( ptr, group, name, description, length, 1, &value ) );
		}

		// Constructor
		MDFRecord ( void ) : 
			headerEntryList( nullptr ),
			iMarker( nullptr ),
			marker( nullptr ),
			markerVisibility( nullptr ),
			iForce( nullptr ),
			force( nullptr ),
			iAnalog( nullptr ),
			analog( nullptr ),
			markerResolution( nullptr ),
			markerHardwareCount( nullptr ),
			forceResolution( nullptr ),
			analogResolution( nullptr ),
			markerName( nullptr ),
			forceChannelName( nullptr ),
			analogChannelName( nullptr ),
			event( nullptr ),

			nForceChannels( 0 ),
			nForceSamples( 0 ),
			nAnalogChannels( 0 ),
			nAnalogSamples( 0 ),
			nMarkerFrames( 0 ),
			nMarkers( 0 ),

			longLabels( true ) // Determines if C3D marker names are 4 chars or more.

			{}

		// Destructor
		~MDFRecord( void ) {

			unsigned int i;

			if ( iMarker ) {
				for ( i = 0; i < nMarkers; i++ ) {
					if ( iMarker[i] ) free( iMarker[i] );
				}
				free( iMarker );
			}
			if ( marker ) {
				for ( i = 0; i < nMarkers; i++ ) {
					if ( marker[i] ) free( marker[i] );
				}
				free( marker );
			}
			if ( markerVisibility ) {
				for ( i = 0; i < nMarkers; i++ ) {
					if ( markerVisibility[i] ) free( markerVisibility[i] );
				}
				free( markerVisibility );
			}
			if ( markerName ) {
				for ( i = 0; i < nMarkers; i++ ) {
					if ( markerName[i] ) free( markerName[i] );
				}
			}
			if ( iForce ) {
				for ( i = 0; i < nForceChannels; i++ ) {
					if ( iForce[i] ) free( iForce[i] );
				}
				free( iForce );
			}
			if ( force ) {
				for ( i = 0; i < nForceChannels; i++ ) {
					if ( force[i] ) free( force[i] );
				}
				free( force );
			}
			if ( forceChannelName ) {
				for ( i = 0; i < nMarkers; i++ ) {
					if ( forceChannelName[i] ) free( forceChannelName[i] );
				}
				free( forceChannelName );
			}
			if ( iAnalog ) {
				for ( i = 0; i < nAnalogChannels; i++ ) {
					if ( iAnalog[i] ) free( iAnalog[i] );
				}
				free( iAnalog );
			}
			if ( analog ) {
				for ( i = 0; i < nAnalogChannels; i++ ) {
					if ( analog[i] ) free( analog[i] );
				}
				free( analog );
			}
			if ( analogChannelName ) {
				for ( i = 0; i < nAnalogChannels; i++ ) {
					if ( analogChannelName[i] ) free( analogChannelName[i] );
				}
				free( analogChannelName );
			}
			if ( markerResolution ) free( markerResolution );
			if ( markerHardwareCount ) free( markerHardwareCount );
			if ( analogResolution ) free( analogResolution );
			if ( forceResolution ) free( forceResolution );

		}

			
		

	};


}