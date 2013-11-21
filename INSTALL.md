From binaries
======================
Linux
-----------------------
### Requirements
* R (>= 2.13)
* Rcpp package

### Install RBiips
1. Install biips library package

    For Debian/Ubuntu, type in a terminal:
    
        sudo dpkg -i biips_x.x.x_Linux.deb
        
    For OpenSuse, type in a terminal:
    
        sudo rpm -i biips_x.x.x_Linux.rpm
        
    For Mandriva, type in a terminal:
    
        sudo urpmi biips_x.x.x_Linux.rpm
        
2. Install RBiips package: type in R console

        install.packages("path/to/RBiips_x.x.x.tar.gz")

Windows
-----------------------
### Requirements
* R (>= 2.13)
* Rcpp package

### Install RBiips
Type in R console:

    install.packages("path/to/RBiips_x.x.x.zip")
    
---------------------------------------------------------------------

From sources
===================
Linux
-----------------------
### Requirements
* git
* cmake
* gcc (>= 4.7)
* boost (>=1.49.0)
* R (>= 2.13)
* Rcpp package
* Matlab or Octave

### Extract source archive
### Configure

    cd biips_sources
    mkdir build
    cd build
    cmake  -DBUILD_MATBIIPS=ON -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release -DMEX=mex ..   
### Compile
    make -jN (N is the number of CPUS + 1)
### Install
    make install
### Test
    make test
     
If you lose control of your terminal during the tests, type

    stty echo

