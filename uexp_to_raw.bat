@echo off

set input= %1
set "verbose = false"

if "%verbose%" == "true" (
	ECHO Input is: %input%
)

FOR /F "tokens=* USEBACKQ" %%F IN (`.\Universal_Column_Fetcher.exe %input%`) DO (
SET output=%%F
)

if "%verbose%" == "true" (
	ECHO Columns are
	ECHO %output%
	ECHO Running Grabber:
)


(.\Universal_Grabber.exe %output% ) > tabular_raw_bytes.csv

if "%verbose%" == "true" (
	ECHO Done.
	ECHO Running Parser...
)

(.\parse_seperated_data.exe tabular_raw_bytes.csv) > tabular_parsed.csv
IF %errorlevel%==1 goto parse_error

if "%verbose%" == "true" (
	ECHO Done.
)

exit
:parse_error
echo Error level: %errorlevel%
del tabular_parsed.csv