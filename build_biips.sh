#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage:   ./build_biips.sh [-jN [-g [-oct]]]"
    echo "    Where N=nb of parallel jobs."
    echo "    The options order matters."
    echo "    Use any string that does not match the option, e.g. '-', to skip an option"
fi

set -x;
# Change these variables to fit your needs
#-----------------------------------------
# prefer absolute over relative paths

if [[ "$(uname)" == "Darwin" ]]; then
    # environment variables for Mac
    export BIIPS_SRC=`pwd`
    export BIIPS_BUILD=/Users/adrien/workspace/biips-build
    export BIIPS_ROOT=/Users/adrien/biips
    export BOOST_ROOT=/Users/adrien/boost_1_53_0
    export LIBnn=lib
    export MATLAB_ROOT=/Applications/MATLAB_R2012a.app
    export OCTAVE_ROOT=/opt/local
    export CMAKE_BUILD_TYPE=Release
    export CMAKE_GENERATOR="Unix Makefiles"
    export CMAKE_OPTIONS="-DBUILD_TESTS=OFF"
    #export CMAKE_OPTIONS="-DBUILD_TESTS=OFF -DPRINT_SYSTEM_INFO=ON"
    export CPACK_GENERATOR="PackageMaker"
    export MAKE="make $1"
    
    if [[ "$2" == "-g" ]]; then
        export BIIPS_BUILD=/Users/adrien/workspace/biips-debug
        export CMAKE_BUILD_TYPE=Debug
    fi

else
    # environment variables for Linux
    export BIIPS_SRC=`pwd`
    export BIIPS_BUILD=/home/adrien-alea/workspace/biips-build
    export BIIPS_ROOT=/home/adrien-alea/biips
    export LIBnn=lib/x86_64-linux-gnu
    # Ubuntu: use lib/i386-linux-gnu or lib/x86_64-linux-gnu
    # OpenSuse: use lib or lib64
    export MATLAB_ROOT=/usr/local/MATLAB/R2014a
    export ECLIPSE=/home/adrien-alea/eclipse_4.3/eclipse
    export CMAKE_ECLIPSE_VERSION=4.3
    export CMAKE_BUILD_TYPE=Release
    export CMAKE_GENERATOR="Eclipse CDT4 - Unix Makefiles"
    export CMAKE_OPTIONS="-DCMAKE_ECLIPSE_EXECUTABLE=$ECLIPSE -DCMAKE_ECLIPSE_VERSION=$CMAKE_ECLIPSE_VERSION -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=$1 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE"
    # OpenSuse: add -DBoost_USE_STATIC_LIBS=OFF
    export CPACK_GENERATOR=DEB
    # OpenSuse: use RPM
    export MAKE="make $1"
    
    if [[ "$2" == "-g" ]]; then
        export BIIPS_BUILD=/home/adrien-alea/workspace/biips-debug
        # export MATLAB_ROOT=/usr/local/MATLAB/R2010b
        export CMAKE_BUILD_TYPE=Debug
    fi
fi

if [[ "$3" == "-oct" ]]; then
    export CMAKE_OPTIONS="$CMAKE_OPTIONS -DFIND_OCTAVE=ON"
    export BIIPS_BUILD=${BIIPS_BUILD}-oct
fi


#-----------------------------------------

set +x; echo -n "*** Git pull? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    cd $BIIPS_SRC
    git pull
fi

set +x; echo -n "*** Run CMake? (y/[n])"; read ans
if [[ $ans == "y" ]]; then
	echo -n "*** Clear build directory? (y/[n])"; read ans
	if [[ $ans == "y" ]]; then set -x
	    rm -rf $BIIPS_BUILD
	fi
	set +x; if [ ! -d "$BIIPS_BUILD" ]; then set -x
	    mkdir $BIIPS_BUILD
	fi
    set -x
    cd $BIIPS_BUILD
    if [ -e  _CPack_Packages ]; then
        sudo rm -rf _CPack_Packages
    fi
    cmake -G"$CMAKE_GENERATOR" $CMAKE_OPTIONS -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$BIIPS_ROOT $BIIPS_SRC
fi

cd $BIIPS_BUILD

set +x; echo -n "*** Build/install Biips? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    rm -rf $BIIPS_ROOT; mkdir $BIIPS_ROOT
    $MAKE install

    set +x; echo -n "*** Run BiipsTest tests? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/test
        $MAKE test
    fi

    set +x; echo -n "*** Run BiipsTestCompiler tests? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/testcompiler
        $MAKE test
    fi

    set +x; echo -n "*** Package Biips? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD
        sudo cpack -G $CPACK_GENERATOR
        sudo cpack -G TGZ
	    sudo $MAKE package_source

        if [[ "$(uname)" != "Darwin" ]]; then
            set +x; echo -n "*** Install Biips DEB package ? (y/[n])"; read ans
            if [[ $ans == "y" ]]; then set -x
                cd $BIIPS_BUILD
                sudo dpkg -i $BIIPS_BUILD/*.deb
                sudo apt-get -f install
            fi
        fi
    fi
fi


set +x; echo -n "*** Install/build Rbiips? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    export BIIPS_INCLUDE=${BIIPS_ROOT}/include/biips
    export BIIPS_LIB=${BIIPS_ROOT}/$LIBnn
    cd $BIIPS_BUILD
    if [ "$(uname)" == "Darwin" ]; then
        $MAKE VERBOSE=1 Rbiips_INSTALL_build
    else
        $MAKE VERBOSE=1 Rbiips_INSTALL
        $MAKE Rbiips_build
    fi
    set +x; echo -n "*** Make Rbiips PDF doc? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        $MAKE Rbiips_Rd2pdf
    fi
fi

set +x; echo -n "*** Build Matbiips? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    cd $BIIPS_BUILD
    $MAKE matbiips_package

    set +x; echo -n "*** Run Matbiips tests? (y/[n])"; read ans
    if [[ $ans == "y" ]]; then set -x
        cd $BIIPS_BUILD/matbiips
        ctest -VV
    fi
fi

set +x; echo -n "*** Make examples package? (y/[n])"; read ans
if [[ $ans == "y" ]]; then set -x
    cd $BIIPS_BUILD
    $MAKE examples_package
fi


set +x; read -p "*** Press [Enter] key to finish..."

