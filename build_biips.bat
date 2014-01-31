:: Change these variables to fit your needs
::-----------------------------------------
set BIIPS_SRC=%~dp0
set BIIPS_BUILD=C:\Users\adrien\workspace\biips-build
set BIIPS_ROOT=C:\Users\adrien\biips
set BOOST_ROOT=C:\Program Files\boost\boost_1_49_0
::set BOOST_LIBRARYDIR64=%BOOST_ROOT%\stage\lib64
set PAGEANT=C:\Program Files (x86)\PuTTY\pageant.exe
set GFORGE_PRIVATE_KEY=C:\Users\adrien\Dropbox\INRIA\ssh\GForge_Inria_key.ppk
set TORTOISEGITPROC=C:\Program Files\TortoiseGit\bin\TortoiseGitProc.exe
set ECLIPSE=C:\Program Files\eclipse\eclipse.exe
set RTOOLS_BINDIR=C:\Rtools\bin
set CMAKE_BUILD_TYPE=Release
set CMAKE_GENERATOR="Eclipse CDT4 - MinGW Makefiles"
set CMAKE_OPTIONS="-DCMAKE_ECLIPSE_VERSION=4.3 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=%1"
set CPACK_GENERATOR=NSIS
set MAKE=C:\MinGW\bin\mingw32-make
set MAKE_OPT=%1
::-----------------------------------------

pause
"%PAGEANT%" "%GFORGE_PRIVATE_KEY%"
"%TORTOISEGITPROC%" /command:pull origin master /path:"%BIIPS_SRC%" /closeonend:2

choice /m "Run CMake"
if "%errorlevel%"=="1" (
	call:ask_clear
	cd "%BIIPS_BUILD%"
	cmake -G%CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% -DCMAKE_INSTALL_PREFIX="%BIIPS_ROOT%" -DCMAKE_ECLIPSE_EXECUTABLE="%ECLIPSE%" "%BIIPS_SRC%"
)

cd "%BIIPS_BUILD%"

choice /m "Build/install Biips"
if "%errorlevel%"=="1" (
	rmdir /S /Q "%BIIPS_ROOT%"
	mkdir "%BIIPS_ROOT%"
	cd "%BIIPS_BUILD%"
	"%MAKE%" %MAKE_OPT% install
	call:ask_test
	call:ask_testcompiler
)

choice /m "Build/install RBiips"
if "%errorlevel%"=="1" (
	set "PATH=%RTOOLS_BINDIR%;%PATH%"
	cd "%BIIPS_BUILD%"
	"%MAKE%" %MAKE_OPT% VERBOSE=1 RBiips_INSTALL_build
	call:ask_make_pdf
)

choice /m "Build MatBiips"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%"
	"%MAKE%" %MAKE_OPT% matbiips_package
	call:ask_test_matbiips
)

cd "%BIIPS_SRC%"

pause

goto:eof

::-----------------
::Functions section
::-----------------

:ask_clear
choice /m "Clear build directory"
if "%errorlevel%"=="1" (
	rmdir /S /Q "%BIIPS_BUILD%"
	mkdir "%BIIPS_BUILD%"
)
goto:eof

:ask_test
choice /m "Run BiipsTest tests"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%\test"
	"%MAKE%" %MAKE_OPT% test
)
goto:eof

:ask_testcompiler
choice /m "Run BiipsTestCompiler tests"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%\testcompiler"
	"%MAKE%" %MAKE_OPT% test
)
goto:eof

:ask_make_pdf
choice /m "Make RBiips PDF doc"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%"
	"%MAKE%" %MAKE_OPT% RBiips_Rd2pdf
)
goto:eof

:ask_test_matbiips
choice /m "Run MatBiips tests"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%\matbiips"
	"%MAKE%" %MAKE_OPT% test
)
goto:eof
