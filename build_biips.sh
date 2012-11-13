#!/bin/bash
set -x

# Change these variables to fit your needs
#-----------------------------------------
export BIIPS_SRC=/home/adrien-alea/workspace/biips-src/trunk
export BIIPS_BUILD=/home/adrien-alea/workspace/biips-release
export BIIPS_ROOT=/home/adrien-alea/biips
export LIBnn=lib64
export ECLIPSE=/home/adrien-alea/eclipse/eclipse
export MATLAB_ROOT=/home/adrien-alea/MATLAB/R2012a
export CMAKE_BUILD_TYPE=Release
export CMAKE_GENERATOR="Eclipse CDT4 - Unix Makefiles"
export CMAKE_OPTIONS=-DBoost_USE_STATIC_LIBS=OFF
export CPACK_GENERATOR=RPM
export NJOBS=10
#-----------------------------------------

set +x; read -p 'Press [Enter] key to continue...'; set -x
svn up $BIIPS_SRC

set +x; echo -n "Run CMake ? (y/n)"; read ans
if [[ $ans == "y" ]]; then
	echo -n "Clear build directory ? (y/n)"; read ans
	if [[ $ans == "y" ]]; then set -x
	    rm -rf $BIIPS_BUILD; mkdir $BIIPS_BUILD
	fi
    set -x
    cd $BIIPS_BUILD
    cmake -G"$CMAKE_GENERATOR" $CMAKE_OPTIONS -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$BIIPS_ROOT $BIIPS_SRC
fi

cd $BIIPS_BUILD

set +x; echo -n "Build/install Biips ? (y/n)"; read ans
if [[ $ans == "y" ]]; then set -x
    rm -rf $BIIPS_ROOT; mkdir $BIIPS_ROOT
    make -j$NJOBS install

    set +x; echo -n "Run BiipsTest tests ? (y/n)"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/test
        make -j$NJOBS test
    fi

    set +x; echo -n "Run BiipsTestCompiler tests ? (y/n)"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/testcompiler
        make -j$NJOBS test
    fi

    set +x; echo -n "Package Biips ? (y/n)"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD
        cpack -G $CPACK_GENERATOR
        cpack -G TGZ
    fi
fi


set +x; echo -n "Install/build RBiips ? (y/n)"; read ans
if [[ $ans == "y" ]]; then set -x
    export BIIPS_INCLUDE=${BIIPS_ROOT}/include/biips
    export BIIPS_LIB=${BIIPS_ROOT}/$LIBnn
    cd $BIIPS_BUILD
    make -j$NJOBS RBiips_INSTALL

    set +x; read -p 'Press [Enter] key to continue...'; set -x
    make -j$NJOBS RBiips_build
fi

set +x; echo -n "Build MatBiips ? (y/n)"; read ans
if [[ $ans == "y" ]]; then set -x
    cd $BIIPS_BUILD
    make -j$NJOBS matbiips

    set +x; echo -n "Run MatBiips tests ? (y/n)"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/matbiips
        make -j$NJOBS test
    fi
fi


set +x; read -p 'Press [Enter] key to finish...'

