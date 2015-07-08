if "%1" == "" (
    echo "Usage:   build_biips.bat [-jN [-g [-32 [-oct]]]]"
    echo "    Where N=nb of parallel jobs."
    echo "    The options order matters."
    echo "    Use any string that does not match the option, e.g. '-', to skip an option"
)

:: Change these variables to fit your needs
::-----------------------------------------
set BIIPS_SRC=%~dp0
set BIIPS_ROOT=%USERPROFILE%\biips
set BOOST_ROOT=%ProgramFiles%\boost\boost_1_58_0
set BOOSTLIB=%BOOST_ROOT%
set PAGEANT=%ProgramFiles(x86)%\PuTTY\pageant.exe
set GFORGE_PRIVATE_KEY=%USERPROFILE%\Documents\GForge_Inria_key.ppk
set TORTOISEGITPROC=%ProgramFiles%\TortoiseGit\bin\TortoiseGitProc.exe
set ECLIPSE=%ProgramFiles%\eclipse\eclipse.exe
set R_BINDIR=%ProgramFiles%\R\R-3.2.0\bin
set MATLAB_ROOT=%ProgramFiles%\MATLAB\R2012a
set OCTAVE_ROOT=%HOMEDRIVE%\Octave\Octave3.6.4_gcc4.6.2
set GCC_BINDIR=%HOMEDRIVE%\Rtools\gcc-4.6.3\bin
set RTOOLS_BINDIR=%HOMEDRIVE%\Rtools\bin
set CMAKE_GENERATOR="Eclipse CDT4 - MinGW Makefiles"
set CMAKE_OPTIONS=-DCMAKE_ECLIPSE_VERSION=4.3 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=%1 -DR_ARCH=x64
set CPACK_GENERATOR=NSIS
set MAKE=%HOMEDRIVE%\MinGW\bin\mingw32-make
set MAKE_OPT=%1

set BIIPS_BUILD=%USERPROFILE%\workspace\biips-build
set CMAKE_BUILD_TYPE=Release

if "%2"=="-g" (
    set BIIPS_BUILD=%USERPROFILE%\workspace\biips-debug
::    set CMAKE_BUILD_TYPE=RelWithDebInfo
    set CMAKE_BUILD_TYPE=Debug
)

if "%2"=="-msvc" (
    set BIIPS_BUILD=%USERPROFILE%\workspace\biips-msvc
    set CMAKE_BUILD_TYPE=RelWithDebInfo
::    set CMAKE_BUILD_TYPE=Release
    set CMAKE_GENERATOR="MinGW Makefiles"
    set CMAKE_OPTIONS=-DBUILD_RBIIPS=OFF -DBUILD_TESTS=OFF -DBUILD_MATBIIPS=ON -DSUPPRESS_DEPRECATED_WARNINGS=OFF -DBUILD_64BIT=ON
)

if "%3"=="-32" (
    set BIIPS_BUILD=%BIIPS_BUILD%-32bit
    set CMAKE_OPTIONS=%CMAKE_OPTIONS% -DBUILD_64BIT=OFF -DBOOST_LIBRARYDIR="%BOOST_ROOT%\stage32\lib" -DR_ARCH=i386
	
	if NOT "%4" == "-oct" (
		set CMAKE_OPTIONS=%CMAKE_OPTIONS% -DBUILD_MATBIIPS=OFF
	)
)

if "%4" == "-oct" (
    set BIIPS_BUILD=%BIIPS_BUILD%-oct
    set CMAKE_OPTIONS=%CMAKE_OPTIONS% -DFIND_OCTAVE=ON
)

::-----------------------------------------


choice /m "Git pull"
if "%errorlevel%"=="1" (
    "%PAGEANT%" "%GFORGE_PRIVATE_KEY%"
    "%TORTOISEGITPROC%" /command:pull /path:"%BIIPS_SRC%" /closeonend:2
)

choice /m "Run CMake"
if "%errorlevel%"=="1" (
	call:ask_clear
	TIMEOUT /T 1
	cd "%BIIPS_BUILD%"
	set "PATH=%GCC_BINDIR%;%PATH%"
	TIMEOUT /T 1
	cmake -G%CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% %CMAKE_OPTIONS% -DCMAKE_INSTALL_PREFIX="%BIIPS_ROOT%" -DCMAKE_ECLIPSE_EXECUTABLE="%ECLIPSE%" "%BIIPS_SRC%"
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

choice /m "Build/install Rbiips"
if "%errorlevel%"=="1" (
	set "PATH=%RTOOLS_BINDIR%;%PATH%"
	cd "%BIIPS_BUILD%"
	"%MAKE%" VERBOSE=1 Rbiips_build_bin
	call:ask_make_pdf
)

choice /m "Build Matbiips"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%"
	"%MAKE%" %MAKE_OPT% matbiips_package
	call:ask_test_matbiips
)

choice /m "Make examples package"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%"
	"%MAKE%" examples_package
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
	TIMEOUT /T 1
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
choice /m "Make Rbiips PDF doc"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%"
	"%MAKE%" %MAKE_OPT% Rbiips_rd2pdf
)
goto:eof

:ask_test_matbiips
choice /m "Run Matbiips tests"
if "%errorlevel%"=="1" (
	cd "%BIIPS_BUILD%\matbiips"
	ctest -VV test
)
goto:eof
