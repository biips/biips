Install from sources
======================

Requirements
-----------------------
To compile Biips, you need:
* git
* cmake
* gcc (>= 4.7)
* boost-X.XX (X>=1.49)
* Matlab or Octave
* R with RCpp package

Extract source archive
-----------------------

Configure
---------

    cd biips_sources
    mkdir build
    cd build
    cmake  -DBUILD_MATBIIPS=ON -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release -DMEX=mex ..

Compile
-------
     
     make -jN (N is the number of CPUS + 1)

Install
-------

     make install
Test
----
     
     make test
     
If you lose control of your terminal when exitting matlab (eg. during the tests).
stty echo
