// MDFtoC3D.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../MDF/MDF.h"

using namespace PsyPhy;
using namespace MDF;

int main(int argc, char *argv[])
{

	int return_code;

	MDF::MDFRecord *mdf = new MDF::MDFRecord();
	return_code = mdf->ReadDataFile( "IsMoreFile.mdf" );
//	mdf->KeepOnly( 0, 27 );
	mdf->WriteC3D( "IsMoreFile.C3D" );
	return return_code; 
}

