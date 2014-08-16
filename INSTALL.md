Windows
=============

install Rbiips
--------------
* Requirements: R (>= 3.0) with Rcpp package

* Install Rbiips package from R console:

        install.packages('path/to/Rbiips_X.X.X.zip')

install Matbiips
----------------
* Requirements: Matlab (>=R2012b)

* Extract Matbiips archive

* Before using Matbiips in Matlab, you need to add the directory to the Matlab search path by typing in Matlab console:

        addpath('path/to/matbiips/matlab')

-------------------------------------------------------------------------------
Mac OS X
============

install Rbiips
--------------
* Requirements: R (>= 3.0) with Rcpp package
* Install Rbiips package from R console:

        install.packages('path/to/Rbiips_X.X.X.tgz')

install Matbiips
----------------
* Requirements: Matlab (>=R2012b)

* Extract Matbiips archive

* Before using Matbiips in Matlab, you need to add the directory to the Matlab search path by typing in Matlab console:

        addpath('path/to/matbiips/matlab')

-------------------------------------------------------------------------------
Linux
=============

install biips from binary
-------------------------
* Requirements: libboost-dev (>=1.49)

* Root installation (Debian/Ubuntu) from terminal:
        sudo dpkg -i biips_x.x.x_Linux.deb

...or double-click on the deb file in your file browser. This will install it as root in /usr/...

* Local installation (Debian/Ubuntu) from terminal:
        dpkg-deb -x biips_x.x.x.deb  path/to/install

FIXME: a warning message appears when installing the debian package

        Lintian check results for /home/adrien-alea/Dropbox/BiiPS/biips-share/new_release/biips_0.8.1_x86_64_Linux.deb:
        E: biips: control-file-has-bad-permissions md5sums 0664 != 0644
        E: biips: control-file-has-bad-owner md5sums ci/ci != root/root

* Other Linux distributions:
    OpenSuse:
    
        sudo rpm -i biips_x.x.x_Linux.rpm
        
    Mandriva:
    
        sudo urpmi biips_x.x.x_Linux.rpm

install Rbiips
--------------
* Requirements : biips, R (>= 3.0) with Rcpp package

* Install Rbiips package from linux terminal:

        env BIIPS_INCLUDE=path/to/install/usr/include/biips/ BIIPS_LIB=/path/to/install/lib/ARCH R CMD INSTALL Rbiips_0.8.1.tar.gz

install Matbiips
----------------
* Requirements: Matlab (>=R2012b). Previous biips installation is not needed for Matbiips

* Extract Matbiips archive

        tar xvzf matbiips_X.X.X.tar.gz

* Test Matbiips

        cd path/to/matbiips/tests
        LD_PRELOAD=/usr/libARCH/libstdc++.so.6  matlab
        >> test_smc

where `libARCH` is a directory depending on your Linux distribution and architecture, e.g.:
- on 64bit Debian/Ubuntu: `libARCH=lib/x86_64-linux-gnu`
- on 64bit Fedora/openSUSE/Mandriva/CentOS: `libARCH=lib64`

Note: alternatively to using the `LD_PRELOAD=...` command, you can rename the `path/to/MATLAB/R20XXx/sys/os/glnxa64/libstdc++.so.6` to e.g. `libstdc++.so.6.bak`.
This usually requires administrator permissions.

install biips from sources
--------------------------
* Requirements:
    - git
    - cmake
    - gcc (>= 4.7)
    - boost (>=1.49.0)
    - R (>= 3.0) + Rcpp package
    - Matlab or Octave

* Extract source archive

* Configure

        mkdir biips_build
        cd biips_build
        cmake -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release path/to/sources

* Compile

        make -jN (where N = number of parallel jobs)

* Install

        make install

* Test

        make test

Note: If you lose control of your terminal during the tests, type

        stty echo

