cmake_minimum_required (VERSION 2.8) 

include (GNUInstallDirs)

# project configuration
project (BiipsTestCompiler CXX C)
set (PROJECT_EXE_NAME ${PROJECT_NAME})
string(REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_INSTALL_BINDIR}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

# find boost libraries
find_package(Boost 1.42.0 REQUIRED
	program_options
)
include_directories (${Boost_INCLUDE_DIRS})
set (EXTRA_LIBS ${EXTRA_LIBS}
	${Boost_LIBRARIES}
)

# should we use qwt5-qt4
option(USE_Qwt5_Qt4 "Use qwt5-qt4 library to plot results of tests" OFF)

# find qwt5-qt4
if (USE_Qwt5_Qt4)
    find_package(Qwt5 REQUIRED)
    if (NOT Qwt5_Qt4_FOUND)
    	message(FATAL_ERROR "Qwt5 linked with Qt4 not found")
    endif (NOT Qwt5_Qt4_FOUND)
    include_directories (${Qwt5_INCLUDE_DIR})
    include(UseQt4)
    set (EXTRA_LIBS ${EXTRA_LIBS}
    	${Qwt5_Qt4_LIBRARY}
    )
endif (USE_Qwt5_Qt4)

# configure config.hpp
configure_file(
    ${PROJECT_SOURCE_DIR}/include/config.hpp.in
    ${PROJECT_BINARY_DIR}/include/config.hpp
)

# include Biips libraries
include_directories (
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}/include
	${BiipsCompiler_SOURCE_DIR}/include
	${BiipsBase_SOURCE_DIR}/include
	${BiipsCore_SOURCE_DIR}/include
)

# source files list generation
file (GLOB SOURCE_FILES
	${PROJECT_BINARY_DIR}/src/*.cpp
	${PROJECT_BINARY_DIR}/src/*.cc
	${PROJECT_BINARY_DIR}/include/*.hpp
	${PROJECT_BINARY_DIR}/include/*.h
	${PROJECT_SOURCE_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/src/*.cc
	${PROJECT_SOURCE_DIR}/include/*.hpp
	${PROJECT_SOURCE_DIR}/include/*.h
)

# linked libraries
set (EXTRA_LIBS ${EXTRA_LIBS}
	BiipsCompiler
	BiipsBase
	BiipsCore
)

# add the executable
add_executable (${PROJECT_EXE_NAME} ${SOURCE_FILES})
target_link_libraries (${PROJECT_EXE_NAME} ${EXTRA_LIBS})
if (UNIX)
	set (EXTRA_CCFLAGS -Wno-deprecated)
	set_target_properties(${PROJECT_EXE_NAME} PROPERTIES COMPILE_FLAGS ${EXTRA_CCFLAGS})
endif (UNIX)

# add the install targets
install (TARGETS ${PROJECT_EXE_NAME}
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_REL_DIR}/bin
	COMPONENT Test
)
install (FILES
	biipstestcompiler.cfg
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_REL_DIR}
	COMPONENT Test
)
install (PROGRAMS
	BiipsTestCompiler.sh
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_REL_DIR}
	COMPONENT Test
)

install(DIRECTORY
	${PROJECT_SOURCE_DIR}/bench/
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_REL_DIR}/bench
	COMPONENT Test
	FILES_MATCHING
	PATTERN hmm_1d*.cfg
	PATTERN .svn EXCLUDE
)

install(DIRECTORY
	${PROJECT_SOURCE_DIR}/model/
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_REL_DIR}/model
	COMPONENT Test
	FILES_MATCHING
	PATTERN hmm_1d*.bug
	PATTERN .svn EXCLUDE
)
