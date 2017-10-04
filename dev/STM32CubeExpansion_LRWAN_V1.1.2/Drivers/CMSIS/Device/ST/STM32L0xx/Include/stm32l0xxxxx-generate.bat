@color 0B
@echo off

REM Generate all CMSIS files 
REM Active perl must be installed

set scriptPATH=%CD%\..\..\..\..\..\..\[INTERNAL]_Validation_Integration\Integration_Tool\_CmsisDeviceGenerator\

if not exist "%scriptPATH%"  (
	echo Input directory does not exist!
	pause
	exit
)

cd %scriptPATH%

DeviceGeneration.pl  --target STM32L0xx -replace

pause
:EOF
