_release: 
	dir ..
	dir ..\Release
	copy /Y ..\Release\MDFtoEMT.exe ..\MDFTools
	copy /Y ..\MDFtoEMT\MDFtoEMT.bat ..\MDFTools

_debug: 
	copy /Y /V ..\Debug\MDFtoEMT.exe \MDFTools
	copy /Y /V ..\MDFtoEMT\MDFtoEMT.bat \MDFTools