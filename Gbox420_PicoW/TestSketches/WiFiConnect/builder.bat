echo >/dev/null # > nul & GOTO WINDOWS & rem ^

echo 'Processing for Linux'
# New lines in Linux: \n, Do Linux Bash commands here... for example:
StartDir="$(pwd)"

exit 0   # Linux commands are complete, end the script

:WINDOWS
@ECHO OFF
REM  New lines in Windows: \r\n
ECHO "Processing for Windows"
REM Do Windows CMD commands here... for example:
SET StartDir=%cd%
choice /C YN /T 5 /D Y /M "Keep previous build folder"
if %ERRORLEVEL% EQU 1 GOTO KEEPFOLDER
rmdir /S /Q "%StartDir%\..\build"
:KEEPFOLDER
mkdir "%StartDir%\..\build"
ECHO Rebuilding...
cd ..
cd build
cmake %StartDir% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel
mingw32-make.exe
cd %StartDir%