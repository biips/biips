cmake_minimum_required (VERSION 2.6) 

include (GNUInstallDirs)

# project configuration
project (BiipsTestCompiler-src CXX)
set (PROJECT_EXE_NAME BiipsTestCompiler)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_INSTALL_BINDIR}/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

# find boost libraries
set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_MULTITHREADED OFF)
set (Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.42.0 REQUIRED
	unit_test_framework
	program_options
)
include_directories (${Boost_LIBRARY_DIRS})

# find qwt5-qt4
find_package(Qwt5 REQUIRED)
if (NOT Qwt5_Qt4_FOUND)
	message(FATAL_ERROR "Qwt5 linked with Qt4 not found")
endif (NOT Qwt5_Qt4_FOUND)
include_directories (${Qwt5_INCLUDE_DIR})
include(UseQt4)

# include Biips libraries
include_directories (
	${PROJECT_SOURCE_DIR}/include
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
set (SOURCE_FILES
	${SOURCE_FILES}
	${PROJECT_SOURCE_DIR}/BiipsTestCompiler.cpp
	${PROJECT_SOURCE_DIR}/config.hpp
)

# linked libraries
set (EXTRA_LIBS ${EXTRA_LIBS}
	${Boost_LIBRARIES}
	${Qwt5_Qt4_LIBRARY}
	BiipsCompiler
	BiipsBase
	BiipsCore
)

# add the executable
add_executable (${PROJECT_EXE_NAME} ${SOURCE_FILES})
target_link_libraries (${PROJECT_EXE_NAME} ${EXTRA_LIBS})
set_target_properties(${PROJECT_EXE_NAME} PROPERTIES COMPILE_FLAGS -Wno-deprecated)

# add the install targets
install (TARGETS ${PROJECT_EXE_NAME}
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_EXE_NAME}/bin
	COMPONENT test
)
install (FILES
	biipstestcompiler.cfg
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_EXE_NAME}
	COMPONENT test
)
install (PROGRAMS
	BiipsTestCompiler.sh
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_EXE_NAME}
	COMPONENT test
)

install(DIRECTORY
	${PROJECT_SOURCE_DIR}/bench/
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_EXE_NAME}/bench
	COMPONENT test
	FILES_MATCHING
	PATTERN hmm_1d*.cfg
	PATTERN .svn EXCLUDE
)

install(DIRECTORY
	${PROJECT_SOURCE_DIR}/model/
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/biips/${PROJECT_EXE_NAME}/model
	COMPONENT test
	FILES_MATCHING
	PATTERN hmm_1d*.bug
	PATTERN .svn EXCLUDE
)
