#!/bin/bash

set -x
# Change these variables to fit your needs
#-----------------------------------------
# prefer absolute over relative paths
export BIIPS_SRC=/home/adrien-alea/workspace/biips-git/
export BIIPS_BUILD=/home/adrien-alea/workspace/biips-build
export BIIPS_ROOT=/home/adrien-alea/biips
export LIBnn=lib/x86_64
# OpenSuse: use lib or lib64
# Ubuntu: use lib/x86 or lib/x86_64
export ECLIPSE=/home/adrien-alea/eclipse_4.3
export MATLAB_ROOT=/usr/local/MATLAB/R2012b
export CMAKE_BUILD_TYPE=Release
export CMAKE_GENERATOR="Eclipse CDT4 - Unix Makefiles"
export CMAKE_OPTIONS="-DBoost_USE_STATIC_LIBS=ON -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j20"
# OpenSuse: use OFF
export CPACK_GENERATOR=DEB
# OpenSuse: use RPM
export MAKE="make -j20"
#-----------------------------------------

set +x; read -p 'Press [Enter] key to continue...'; set -x
cd $BIIPS_SRC
git pull origin master

set +x; echo -n "Run CMake ? (y/[n])"; read ans
if [[ $ans == "y" ]]; then
	echo -n "Clear build directory ? (y/[n])"; read ans
	if [[ $ans == "y" ]]; then set -x
	    rm -rf $BIIPS_BUILD; mkdir $BIIPS_BUILD
	fi
    set -x
    cd $BIIPS_BUILD
    cmake -G"$CMAKE_GENERATOR" $CMAKE_OPTIONS -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$BIIPS_ROOT $BIIPS_SRC
fi

cd $BIIPS_BUILD

set +x; echo -n "Build/install Biips ? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    rm -rf $BIIPS_ROOT; mkdir $BIIPS_ROOT
    $MAKE install

    set +x; echo -n "Run BiipsTest tests ? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/test
        $MAKE test
    fi

    set +x; echo -n "Run BiipsTestCompiler tests ? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/testcompiler
        $MAKE test
    fi

    set +x; echo -n "Package Biips ? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD
        cpack -G $CPACK_GENERATOR
        cpack -G TGZ
    fi
fi


set +x; echo -n "Install/build RBiips ? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    export BIIPS_INCLUDE=${BIIPS_ROOT}/include/biips
    export BIIPS_LIB=${BIIPS_ROOT}/$LIBnn
    cd $BIIPS_BUILD
    $MAKE RBiips_INSTALL
    $MAKE RBiips_build
fi

set +x; echo -n "Build MatBiips ? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    cd $BIIPS_BUILD
    $MAKE matbiips_package

    set +x; echo -n "Run MatBiips tests ? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/matbiips
        $MAKE test
    fi
fi


set +x; read -p 'Press [Enter] key to finish...'

