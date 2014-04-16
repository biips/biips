cmake_minimum_required (VERSION 2.8)

# project configuration
# =====================
project (Biips CXX C)

set (Biips_VERSION_MAJOR 0)
set (Biips_VERSION_MINOR 9)
set (Biips_VERSION_PATCH 0)
set (Biips_VERSION ${Biips_VERSION_MAJOR}.${Biips_VERSION_MINOR}.${Biips_VERSION_PATCH})

file (WRITE VERSION.txt ${Biips_VERSION})

set (Biips_OWNER Inria)
set (Biips_COPYRIGHT_YEAR 2012)
set (Biips_LICENSE GPL-3)
set (Biips_URL http://alea.bordeaux.inria.fr/biips)
set (Biips_SOURCES_URL https://gforge.inria.fr/projects/biips/)
set (Biips_DESCRIPTION_SUMMARY "Bayesian inference with interacting Particle Systems")
set (Biips_CONTACT "biips-project <biips-project@lists.gforge.inria.fr>")
set (Biips_CONTACT_EMAIL "biips-project@lists.gforge.inria.fr")

set (Biips_DESCRIPTION " BiiPS is a general software for Bayesian inference with interacting particle systems, a.k.a. sequential Monte Carlo (SMC) methods.
 It aims at popularizing the use of these methods to non-statistician researchers and students, thanks to its automated \"black box\" inference engine.
 It borrows from the BUGS/JAGS software, widely used in Bayesian statistics, the statistical modeling with graphical models and the language associated with their descriptions.
 Unlike MCMC methods used by BUGS/JAGS, SMC methods are more adapted to dynamic problems (tracking, signal filtering, etc).")

if (NOT Biips_DATE)
    if (WIN32)
        execute_process (COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE TODAY)
    elseif (UNIX)
        execute_process(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE TODAY)
    endif ()
    if (TODAY)
        string (REGEX REPLACE
            "(..)/(..)/(....).*"
            "\\3-\\2-\\1"
            Biips_DATE
            ${TODAY}
        )
    else ()
        message (SEND_ERROR "date not implemented.\nPlease set variable Biips_DATE to yyyy-mm-dd")
    endif ()
endif ()
message (STATUS Biips_DATE=${Biips_DATE})

file (WRITE Description.txt ${Biips_DESCRIPTION})


# set dependencies versions
set (Boost_VERSION_REQUIRED 1.49.0)
set (JAGS_VERSION 3.2.0)
set (rjags_VERSION 3.2)
set (Rcpp_VERSION 0.10.6)
set (R_VERSION_REQUIRED 3.0.2)


# configure resource files
configure_file (README.md.IN README.txt @ONLY)
configure_file (README.md.IN ${CMAKE_SOURCE_DIR}/README.md @ONLY)
#configure_file (Welcome Welcome.txt COPYONLY)
configure_file (COPYING.txt COPYING.txt COPYONLY)

# configure build
math(EXPR BITS "${CMAKE_SIZEOF_VOID_P} * 8")

if (APPLE)
  set(EXTRA_COMPILE_FLAGS "-fPIC")
elseif(UNIX)
    # TODO : if g++ version >= 4.7 or 4.8 ?
  set(EXTRA_COMPILE_FLAGS "--std=c++0x -fPIC")
elseif(WIN32)
  option(BUILD_64BIT "Build 64bit binaries" ON)
  
  if(BUILD_64BIT)
      set(BITS "64")
  endif()
  
  set(EXTRA_COMPILE_FLAGS "${EXTRA_COMPILE_FLAGS} -m${BITS}")
  set(EXTRA_LINK_FLAGS "${EXTRA_LINK_FLAGS} -m${BITS}")
endif()

option (SUPPRESS_DEPRECATED_WARNINGS "Suppress deprecated warnings: -Wno-deprecated." ON)
if (SUPPRESS_DEPRECATED_WARNINGS)
	set (EXTRA_COMPILE_FLAGS "${EXTRA_COMPILE_FLAGS} -Wno-deprecated")
endif ()

add_definitions(${EXTRA_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EXTRA_LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${EXTRA_LINK_FLAGS}")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules/)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (BIIPS_DEBUG TRUE)
else ()
    set (BIIPS_DEBUG FALSE)
endif()

option (BIIPS_DEBUG_PARSER "Print BUGS language parser output (ParseTree)." OFF)

option (BIIPS_CATCH_TEST "Catch exceptions in BiipsTest program." ON)
option (BIIPS_CATCH_TESTCOMPILER "Catch exceptions in BiipsTestCompiler program." ON)


# configure and find boost
option (Boost_USE_STATIC_LIBS "Force the use of the static boost libraries" ON)
option (Boost_USE_MULTITHREADED "Use the non-multithreaded boost libraries" OFF)
option (Boost_USE_STATIC_RUNTIME
	"If enabled, searches for boost libraries linked against a static C++ standard library ('s' ABI tag)" OFF)

find_package(Boost ${Boost_VERSION_REQUIRED} REQUIRED COMPONENTS program_options regex)
if(Boost_PROGRAM_OPTIONS_FOUND)
    set(EXTRA_LIBS ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} ${EXTRA_LIBS})
else()
   message(FATAL_ERROR "Boost Program Options Not Found!") 	
endif()

if(Boost_REGEX_FOUND)
    set(EXTRA_LIBS ${Boost_REGEX_LIBRARY_RELEASE} ${EXTRA_LIBS})
else()
    message(FATAL_ERROR "Boost Regex Not Found!") 	
endif()

# configure install directories and output directories
include (GNUInstallDirs)
if (UNIX)
	set (Biips_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
	set (Biips_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set (Biips_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/biips)
	set (Biips_INSTALL_TESTROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/biips)
	
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${Biips_INSTALL_LIBDIR})
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Biips_INSTALL_LIBDIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Biips_INSTALL_BINDIR})
elseif (WIN32)
    #set (Biips_INSTALL_BINDIR_i386 i386/${CMAKE_INSTALL_BINDIR})
    #set (Biips_INSTALL_BINDIR_x64 x64/${CMAKE_INSTALL_BINDIR})
    #set (Biips_INSTALL_LIBDIR_i386 ${Biips_INSTALL_BINDIR_i386})
    #set (Biips_INSTALL_LIBDIR_x64 ${Biips_INSTALL_BINDIR_x64})
	set (Biips_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
	set (Biips_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set (Biips_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
	set (Biips_INSTALL_TESTROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/biips)
    set (Biips_INSTALL_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR})
endif ()


# configure interfaces build
option (BUILD_RBIIPS "Activate RBiips (BiiPS interface for GNU R) build" ON)
option (BUILD_MATBIIPS "Activate MatBiips (BiiPS interface for Matlab) build" ON)
option (FIND_OCTAVE "Build Matbiips for Octave instead of Matlab" OFF)


# print system info
option(PRINT_SYSTEM_INFO "Print system information when running cmake" OFF)
if (PRINT_SYSTEM_INFO)
    message (CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
    message (CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION})
    message (CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR})
    message (WIN32=${WIN32})
    message (UNIX=${UNIX})
    message (MINGW=${MINGW})
    message (APPLE=${APPLE})
    message (CMAKE_GENERATOR=${CMAKE_GENERATOR})
    message (CMAKE_BUILD_TOOL=${CMAKE_BUILD_TOOL})
    message (CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P})
    message (CMAKE_C_COMPILER=${CMAKE_C_COMPILER})
    message (CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})
    message (CMAKE_COMPILER_IS_GNUCC=${CMAKE_COMPILER_IS_GNUCC})
    message (CMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX})
    message (CMAKE_C_FLAGS=${CMAKE_C_FLAGS})
    message (CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})
endif ()


enable_testing()
# ==============================
# define INCLUDE VARIABLES
# ==============================
set(Core_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/core 
	              ${CMAKE_SOURCE_DIR}/core/include 
		      ${CMAKE_BINARY_DIR}/core/include # for Config.hpp 
			   
)

set(Base_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/base
	              ${CMAKE_SOURCE_DIR}/base/include 
			   
)
set(Compiler_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/compiler
	                  ${CMAKE_SOURCE_DIR}/compiler/include 
			   
)

# ==============================
# Add subdirectories
# ==============================
add_subdirectory (core)
add_subdirectory (base)
add_subdirectory (test)
add_subdirectory (compiler)
add_subdirectory (testcompiler)
add_subdirectory (doc)
add_subdirectory (examples)
if (BUILD_RBIIPS)
    add_subdirectory (RBiips)
endif ()
if (BUILD_MATBIIPS)
    add_subdirectory (matbiips)
endif ()


#============= CPACK commands ============
# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_NAME biips)
set (CPACK_PACKAGE_VENDOR BiiPS)

#if (UNIX)
#	execute_process(COMMAND "dpkg" "--print-architecture" OUTPUT_VARIABLE SYSTEM_ARCH)
#	string(REGEX MATCH "[^\n]*" SYSTEM_ARCH ${SYSTEM_ARCH})
#else ()
#	set (SYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})
#endif ()

# Generic package options
set (CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${Biips_VERSION}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_SYSTEM_NAME})
set (CPACK_PACKAGE_SOURCE_FILE_NAME ${CPACK_PACKAGE_NAME}-src_${Biips_VERSION})
set (CPACK_PACKAGE_VERSION_MAJOR ${Biips_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${Biips_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${Biips_VERSION_PATCH})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${Biips_DESCRIPTION_SUMMARY})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "BiiPS-${Biips_VERSION}")
set (CPACK_PACKAGE_DESCRIPTION_FILE ${Biips_SOURCE_DIR}/Description.txt)
set (CPACK_PACKAGE_CONTACT ${Biips_CONTACT})
set (CPACK_RESOURCE_FILE_README ${CMAKE_BINARY_DIR}/README.txt)
#set (CPACK_RESOURCE_FILE_WELCOME ${CMAKE_BINARY_DIR}/Welcome.txt)
set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_BINARY_DIR}/COPYING.txt)
set (CPACK_PACKAGE_ICON "${Biips_SOURCE_DIR}\\\\doc\\\\biips-logo\\\\biips-logo-large-256.bmp")

# Source packages specific options
set (CPACK_SOURCE_GENERATOR TGZ;ZIP)
set (CPACK_SOURCE_IGNORE_FILES "/\\\\.svn/"
"/\\\\.git/"
"\\\\.Rhistory$" 
"\\\\.gitignore$" 
"\\\\.project$" 
"/\\\\.settings/" 
".*\\\\.html$" 
".*~" 
${CPACK_SOURCE_IGNORE_FILES})

#set (CPACK_PACKAGE_EXECUTABLES "biips_info;Biips info")

# Linux Debian package specific options
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7), libstdc++6 (>= 4.4.0), libboost-dev ")
if (NOT Boost_USE_STATIC_LIBS)
    set (CPACK_DEBIAN_PACKAGE_DEPENDS
        "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libboost-program-options-dev ")
endif ()
set (CPACK_DEBIAN_PACKAGE_SUGGESTS "r-base (>= 2.13)")
if (USE_Qwt5_Qt4)
    set (CPACK_DEBIAN_PACKAGE_DEPENDS
        "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libqwt5-qt4 (>= 5.2.0)")
endif ()
set (CPACK_DEBIAN_PACKAGE_SECTION math)
set (CPACK_DEBIAN_PACKAGE_PRIORITY optional)
set (CPACK_DEBIAN_PACKAGE_HOMEPAGE ${Biips_URL})
set (CPACK_DEBIAN_PACKAGE_REPLACES "libbiips, libbiips-dev, libbiips-test, biips-test")

# Linux RPM package specific options
set (CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.7, libstdc++46 >= 4.4.0, boost-devel")
if (USE_Qwt5_Qt4)
    set (CPACK_RPM_PACKAGE_REQUIRES
        "${CPACK_RPM_PACKAGE_REQUIRES}, libqwt5 >= 5.2.0")
endif ()
set (CPACK_RPM_PACKAGE_RELEASE 1)
set (CPACK_RPM_PACKAGE_LICENSE ${Biips_LICENSE})
set (CPACK_RPM_PACKAGE_GROUP Applications/Engineering)
set (CPACK_RPM_PACKAGE_URL ${Biips_URL})
set (CPACK_RPM_CHANGELOG_FILE ${Biips_SOURCE_DIR}/ChangeLog.txt)

# Windows NSIS package specific options
if (WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set (CPACK_NSIS_EXECUTABLES_DIRECTORY ${Biips_INSTALL_BINDIR})
  set (CPACK_NSIS_INSTALL_ROOT "\$PROGRAMFILES64\\\\BiiPS")
  set (CPACK_NSIS_DISPLAY_NAME "BiiPS ${Biips_VERSION}")
  set (CPACK_NSIS_HELP_LINK ${Biips_URL})
  set (CPACK_NSIS_URL_INFO_ABOUT ${Biips_URL})
  set (CPACK_NSIS_CONTACT ${Biips_CONTACT})
  set (CPACK_NSIS_MUI_ICON "${Biips_SOURCE_DIR}/doc/biips-logo/biips-icon-48.ico")
  set (CPACK_NSIS_MUI_UNICON "${Biips_SOURCE_DIR}/doc/biips-logo/biips-icon-dark-blue-48.ico")
endif ()
include (CPack)
