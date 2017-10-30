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
	return_code = mdf->ReadDataFile( "ismorefile" );

	return return_code; 
}

