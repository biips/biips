cmake_minimum_required (VERSION 2.8)

# project configuration
# =====================
project (Biips CXX C)

set (BIIPS_VERSION_MAJOR 0)
set (BIIPS_VERSION_MINOR 11)
set (BIIPS_VERSION_PATCH 0)
set (BIIPS_VERSION ${BIIPS_VERSION_MAJOR}.${BIIPS_VERSION_MINOR}.${BIIPS_VERSION_PATCH})

set (BIIPS_VENDOR Inria)
set (BIIPS_COPYRIGHT_YEAR 2012-2017)
set (BIIPS_LICENSE GPL-3)
set (BIIPS_URL https://biips.github.io)
set (BIIPS_SOURCES_URL https://github.com/biips/biips)
set (BIIPS_ISSUES_URL https://github.com/biips/biips/issues)
set (BIIPS_DESCRIPTION_SUMMARY "Bayesian inference with interacting particle systems")
set (BIIPS_CONTACT_EMAIL "adrien.todeschini@gmail.com")
set (BIIPS_CONTACT "Adrien Todeschini <${BIIPS_CONTACT_EMAIL}>")

set (BIIPS_DESCRIPTION "Biips is a general software for Bayesian inference with interacting particle systems, a.k.a. sequential Monte Carlo (SMC) methods.
It aims at popularizing the use of these methods to non-statistician researchers and students, thanks to its automated \"black box\" inference engine.
It borrows from the BUGS/JAGS software, widely used in Bayesian statistics, the statistical modeling with graphical models and the language associated with their descriptions.")

if (NOT BIIPS_DATE)
    if (WIN32)
        execute_process (COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE TODAY)
    elseif (UNIX)
        execute_process(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE TODAY)
    endif ()
    if (TODAY)
        string (REGEX REPLACE
            "(..)/(..)/(....).*"
            "\\3-\\2-\\1"
            BIIPS_DATE
            ${TODAY}
        )
    else ()
        message (SEND_ERROR "date not implemented.\nPlease set variable BIIPS_DATE to yyyy-mm-dd")
    endif ()
endif ()

file (WRITE ${CMAKE_BINARY_DIR}/Description.txt ${BIIPS_DESCRIPTION})


# set dependencies versions
set (Boost_VERSION_REQUIRED 1.49.0)

# configure resource files
configure_file (README.md.IN README.txt @ONLY)
configure_file (README.md.IN ${CMAKE_SOURCE_DIR}/README.md @ONLY)
configure_file (COPYING.txt.IN ${CMAKE_SOURCE_DIR}/COPYING.txt @ONLY)

# configure build
math(EXPR BITS "${CMAKE_SIZEOF_VOID_P} * 8")

# Determine compiler target architecture
set(SYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})

if (UNIX) # Linux or MacOSX
  #execute_process(COMMAND "dpkg" "--print-architecture" OUTPUT_VARIABLE SYSTEM_ARCH)
  #string(REGEX MATCH "[^\n]*" SYSTEM_ARCH ${SYSTEM_ARCH})
  execute_process(COMMAND uname -m
    OUTPUT_VARIABLE SYSTEM_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

elseif (WIN32) # Windows
  option(BUILD_64BIT "Build 64bit binaries" ON)

  if(BUILD_64BIT)
      set(BITS "64")
  endif()

  if (BITS STREQUAL 64)
    set( SYSTEM_ARCH "x86_64" )
  else()
    set( SYSTEM_ARCH "x86" )
  endif()
endif()

# Determine compiler
if (UNIX) # Linux or MacOSX
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -v
      OUTPUT_VARIABLE COMPILER_INFO_OUT
      ERROR_VARIABLE COMPILER_INFO_ERR
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCH clang|gcc COMPILER_NAME ${COMPILER_INFO_OUT} ${COMPILER_INFO_ERR})

  if (COMPILER_NAME STREQUAL clang)
    # set(BIIPS_COMPILE_FLAGS "-stdlib=libstdc++ -fPIC -Wno-redeclared-class-member -Wno-c++11-extensions")
    set(BIIPS_COMPILE_FLAGS "-std=c++11 -stdlib=libstdc++ -fPIC -Wno-redeclared-class-member")
    set(BIIPS_LINK_FLAGS "-stdlib=libstdc++")

  elseif (COMPILER_NAME STREQUAL gcc)
      # TODO : check if g++ version >= 4.7 or 4.8 ?
    set(BIIPS_COMPILE_FLAGS "-std=c++11 -fPIC -Wno-unused-local-typedefs -Wno-deprecated-declarations")

  else()
    message(WARNING "FIXME: Could not determine compiler name")
    set(BIIPS_COMPILE_FLAGS "-fPIC")
  endif()

elseif (WIN32) # Windows
  set(BIIPS_COMPILE_FLAGS "-m${BITS}")
  set(BIIPS_LINK_FLAGS "-m${BITS}")
endif()

set(BIIPS_COMPILE_FLAGS "${BIIPS_COMPILE_FLAGS}")

add_definitions(${BIIPS_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BIIPS_LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${BIIPS_LINK_FLAGS}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (BIIPS_DEBUG TRUE)
else ()
    set (BIIPS_DEBUG FALSE)
endif()

option (BIIPS_DEBUG_PARSER "Print BUGS language parser output (ParseTree)." OFF)

option (BIIPS_CATCH_TEST "Catch exceptions in BiipsTest program." ON)
option (BIIPS_CATCH_TESTCOMPILER "Catch exceptions in BiipsTestCompiler program." ON)

option(BUILD_TESTS "Activate BiipsTest and BiipsTestCompiler programs build" ON)

# configure and find boost
option (Boost_USE_STATIC_LIBS "Force the use of the static boost libraries" ON)
option (Boost_USE_MULTITHREADED "Use the non-multithreaded boost libraries" OFF)
option (Boost_USE_STATIC_RUNTIME
	"If enabled, searches for boost libraries linked against a static C++ standard library ('s' ABI tag)" OFF)

if(BUILD_TESTS)
  set(BOOST_REQUIRED_COMPONENTS program_options)
endif()

find_package(Boost ${Boost_VERSION_REQUIRED} REQUIRED COMPONENTS ${BOOST_REQUIRED_COMPONENTS})
if (BUILD_TESTS)
  if(Boost_PROGRAM_OPTIONS_FOUND)
      set(EXTRA_LIBS ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} ${EXTRA_LIBS})
  else()
     message(FATAL_ERROR "Boost Program Options Not Found!")
  endif()
endif()

# configure install directories and output directories
include (GNUInstallDirs)
if (UNIX)
	set (BIIPS_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
    set (BIIPS_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set (BIIPS_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/biips)
	set (BIIPS_INSTALL_TESTROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/biips)

	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIIPS_INSTALL_LIBDIR})
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIIPS_INSTALL_LIBDIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIIPS_INSTALL_BINDIR})
elseif (WIN32)
	set (BIIPS_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
	set (BIIPS_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set (BIIPS_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
	set (BIIPS_INSTALL_TESTROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/biips)
    set (BIIPS_INSTALL_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR})
endif ()


# print system info
option(PRINT_SYSTEM_INFO "Print system information when running cmake" OFF)
if (PRINT_SYSTEM_INFO)
    message (CMAKE_SYSTEM=${CMAKE_SYSTEM})
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
    message (CMAKE_COMPILER_IS_GNUC=${CMAKE_COMPILER_IS_GNUC})
    message (CMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX})
    message (CMAKE_C_FLAGS=${CMAKE_C_FLAGS})
    message (CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})
    message (BITS=${BITS})
    message (SYSTEM_ARCH=${SYSTEM_ARCH})
    message (COMPILER_NAME=${COMPILER_NAME})
    message (COMPILER_INFO_STRING=${COMPILER_INFO_STRING})
endif ()


enable_testing()

# ==============================
# Add subdirectories
# ==============================
add_subdirectory (bin)
add_subdirectory (include)
add_subdirectory (src)
if(BUILD_TESTS)
  add_subdirectory (test)
  add_subdirectory (testcompiler)
endif ()
add_subdirectory (doc)

#============= CPACK commands ============
# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_NAME biips)
set (CPACK_PACKAGE_VENDOR ${BIIPS_VENDOR})

# Generic package options
set (CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${BIIPS_VERSION}_${SYSTEM_ARCH}_${CMAKE_SYSTEM_NAME})
set (CPACK_PACKAGE_SOURCE_FILE_NAME ${CPACK_PACKAGE_NAME}-src_${BIIPS_VERSION})
set (CPACK_PACKAGE_VERSION_MAJOR ${BIIPS_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${BIIPS_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${BIIPS_VERSION_PATCH})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${BIIPS_DESCRIPTION_SUMMARY})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "BIIPS-${BIIPS_VERSION}")
set (CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_BINARY_DIR}/Description.txt)
set (CPACK_PACKAGE_CONTACT ${BIIPS_CONTACT})
set (CPACK_RESOURCE_FILE_README ${CMAKE_BINARY_DIR}/README.txt)
set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING.txt)
set (CPACK_PACKAGE_ICON "${BIIPS_SOURCE_DIR}\\\\doc\\\\biips-logo\\\\biips-logo-large-256.bmp")

# Source packages specific options
set (CPACK_SOURCE_GENERATOR TGZ;ZIP)
set (CPACK_SOURCE_IGNORE_FILES "/\\\\.svn/"
"/\\\\.git/"
"\\\\.Rhistory$"
"\\\\.gitignore$"
"\\\\.project$"
"\\\\.cproject$"
"/\\\\.settings/"
".*\\\\.html$"
".*\\\\.sh"
".*\\\\.bat"
".*\\\\.pdf"
".*\\\\.Rproj.*"
"/\\\\.Rproj\\\\.user"
"/old/"
"/html/"
".*~"
"Journal.*"
"journal.*"
"todo.*"
"Todo.*"
"TODO.*"
"TODO.*"
"install_vm.*"
${CPACK_SOURCE_IGNORE_FILES})

# Linux Debian package specific options
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7), libstdc++6 (>= 4.4.0), libboost-dev ")
if (NOT Boost_USE_STATIC_LIBS)
    set (CPACK_DEBIAN_PACKAGE_DEPENDS
        "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libboost-program-options-dev ")
endif ()
set (CPACK_DEBIAN_PACKAGE_SECTION math)
set (CPACK_DEBIAN_PACKAGE_PRIORITY optional)
set (CPACK_DEBIAN_PACKAGE_HOMEPAGE ${BIIPS_URL})
set (CPACK_DEBIAN_PACKAGE_REPLACES "libbiips, libbiips-dev, libbiips-test, biips-test")

# Linux RPM package specific options
set (CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.7, libstdc++46 >= 4.4.0, boost-devel")
set (CPACK_RPM_PACKAGE_RELEASE 1)
set (CPACK_RPM_PACKAGE_LICENSE ${BIIPS_LICENSE})
set (CPACK_RPM_PACKAGE_GROUP Applications/Engineering)
set (CPACK_RPM_PACKAGE_URL ${BIIPS_URL})
#set (CPACK_RPM_CHANGELOG_FILE ${CMAKE_SOURCE_DIR}/ChangeLog.txt)

# Windows NSIS package specific options
if (WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set (CPACK_NSIS_EXECUTABLES_DIRECTORY ${BIIPS_INSTALL_BINDIR})
  set (CPACK_NSIS_INSTALL_ROOT "\$PROGRAMFILES64\\\\biips")
  set (CPACK_NSIS_DISPLAY_NAME "biips ${BIIPS_VERSION}")
  set (CPACK_NSIS_HELP_LINK ${BIIPS_URL})
  set (CPACK_NSIS_URL_INFO_ABOUT ${BIIPS_URL})
  set (CPACK_NSIS_CONTACT ${BIIPS_CONTACT})
  set (CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/doc/biips-logo/biips-icon-48.ico")
  set (CPACK_NSIS_MUI_UNICON "${CMAKE_SOURCE_DIR}/doc/biips-logo/biips-icon-dark-blue-48.ico")
endif ()
include (CPack)
