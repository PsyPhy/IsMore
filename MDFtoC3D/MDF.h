#pragma once

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

		char *comment;
		unsigned short *date;

		float	**iMarker;
		PsyPhy::Vector3	**marker;
		bool	**markerVisibility;
		unsigned int	nMarkers;
		unsigned int	nMarkerSamples;
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
		void	KeepOnly( int first_marker, int last_marker = -1 );
		void	FillGaps( void );

		// Constructor
		MDFRecord ( void ) : 
			headerEntryList( nullptr ),
			nForceChannels( 0 ),
			nForceSamples( 0 ),
			nAnalogChannels( 0 ),
			nAnalogSamples( 0 ),
			nMarkerSamples( 0 ),
			nMarkers( 0 )
			{}

	};


}