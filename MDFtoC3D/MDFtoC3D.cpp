// MDFtoC3D.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MDF.h"

using namespace PsyPhy;
using namespace MDF;

int main(int argc, char *argv[])
{

	int return_code;

	MDF::MDFRecord *mdf = new MDF::MDFRecord();
	return_code = mdf->ReadDataFile( "IsMoreFile" );
	mdf->WriteMarkersASCII( "lowrate.txt" );
	mdf->FillGaps();
	mdf->WriteMarkersASCII( "hirate.txt" );

	return return_code; 
}

