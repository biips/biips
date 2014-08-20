Windows
=============

Install Rbiips
--------------
* Requirements: R (>= 3.0) with Rcpp package

* Install Rbiips package from R console:

```
  install.packages('path/to/Rbiips_X.X.X.zip')
```

Install Matbiips
----------------
* Requirements: Windows (x64), Matlab (>=R2012b), [Redistribuable Visual C++ 2012 (x64)](http://www.microsoft.com/en-US/download/details.aspx?id=30679)

* Extract Matbiips archive

* Before using Matbiips, you need to add the directory to the Matlab search path by typing in Matlab console:

```
addpath('path/to/matbiips')
```

-------------------------------------------------------------------------------

Mac OS X
============

Install Rbiips
--------------
* Requirements: R (>= 3.0) with Rcpp package

* Install Rbiips package from R console:

```
install.packages('path/to/Rbiips_X.X.X.tgz')
```

Install Matbiips
----------------
* Requirements: Matlab (>=R2012b)

* Extract Matbiips archive

* Before using Matbiips, you need to add the directory to the search Matlab path by typing in Matlab console:

```
addpath('path/to/matbiips')
```
-------------------------------------------------------------------------------

Linux
=============

Install biips from binary
-------------------------
* Requirements: libboost-dev (>=1.49)

* Root installation (Debian/Ubuntu) from terminal:

```
sudo dpkg -i biips_x.x.x_Linux.deb
```

...or double-click on the deb file in your file browser. This will install it as root in /usr/...

* Local installation (Debian/Ubuntu) from terminal:

```
dpkg-deb -x biips_x.x.x.deb  path/to/install
```

* Other Linux distributions:
    - OpenSuse: `sudo rpm -i biips_x.x.x_Linux.rpm`
    - Mandriva: `sudo urpmi biips_x.x.x_Linux.rpm `

Install Rbiips
--------------
* Requirements : biips, R (>= 3.0) with Rcpp package

* Install Rbiips package from linux terminal:

```
env BIIPS_INCLUDE=path/to/install/usr/include/biips/ BIIPS_LIB=/path/to/install/lib/ARCH R CMD INSTALL Rbiips_0.8.1.tar.gz
```

Install Matbiips
----------------
* Requirements: Matlab (>=R2012b). Previous biips installation is not needed for Matbiips

* Extract Matbiips archive

```
tar -xvzf matbiips_X.X.X.tar.gz
```

* Start Matlab using the following command:

```
LD_PRELOAD=/usr/libARCH/libstdc++.so.6 matlab
```

where `libARCH` is a directory depending on your Linux distribution and architecture, *e.g.*:

- on 64bit Debian/Ubuntu: `libARCH=lib/x86_64-linux-gnu`
- on 64bit Fedora/openSUSE/Mandriva/CentOS: `libARCH=lib64`

Note: alternatively to using the `LD_PRELOAD=...` command, you can rename the `path/to/MATLAB/R20XXx/sys/os/glnxa64/libstdc++.so.6` to e.g. `libstdc++.so.6.bak`.
This usually requires administrator permissions.

* Before using Matbiips, you need to add the directory to the search Matlab path by typing in Matlab console:

```
addpath('path/to/matbiips')
```

Install biips from sources
--------------------------
* Requirements:
    - git
    - cmake
    - gcc (>= 4.7)
    - boost (>=1.49.0)
    - R (>= 3.0) + Rcpp package
    - Matlab or Octave

* Extract source archive

```
tar -xvzf path matbiips_X.X.X.tar.gz path/to/sources
```

* Configure

```
mkdir path/to/build
cd path/to/build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install -DCMAKE_BUILD_TYPE=Release path/to/sources
```

* Compile

```
make -jN (where N = number of parallel jobs)
```

* Install

```
make install
```

* Test

```
make test
```

Note: If you lose control of your terminal during the tests, type

```
stty echo
```

