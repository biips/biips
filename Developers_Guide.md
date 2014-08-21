Developers guide
======================

Requirements
----------------  
* git
* cmake
* gcc (>= 4.7)
* boost (>=1.49.0)
* R (>= 3.0) + Rcpp package
* Matlab (>=R2012b) or Octave (>=3.6.4)

Development tools
----------------  
### Windows

* PuTTY
* TortoiseGit + msysgit
* CMake
* MinGW (do not add to PATH)
* R (add to `PATH`: `C:\Program Files\R\R-3.0.2\bin`) + Rcpp package
* Rtools (add to `PATH`: `C:\Rtools\gcc-4.6.3\bin`)
* RStudio
* NSIS 2.46
* MikTeX
* Matlab
* Octave (MinGW port): <http://wiki.octave.org/Octave_for_Microsoft_Windows#MinGW_ports>
* Eclipse CDT (32 bit) + CDT update site + CMakeEd plugin + EGit plugin
* clink: <http://mridgers.github.io/clink/>

### Mac OSX

* XCode + command line tools
* Git
* CMake
* MacPorts with autoconf

```
sudo ports install autoconf
```

* R + Rcpp package
* RStudio
* MacTex
* Matlab
* Octave

```
sudo ports install octave
```

* Eclipse CDT (64 bit) + CDT update site + CMakeEd plugin + EGit plugin


---------------------------------------------------------------------

Compile Boost
----------------  
### Windows

Instructions for compiling boost libraries on Windows 7 64bit using gcc compiler from Rtools.
gcc must be in the `PATH` e.g. `C:\Rtools\gcc-4.6.3\bin`

1. Download Boost for Windows and extract the archive
2. Open console (`cmd`) and place yourself in the extracted folder
3. Compile Boost.Build using Visual C++ compiler. `cl.exe` must be in the `PATH` e.g. `C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin`

```
bootstrap
```

4. Compile the desired libraries for 64bit architecture and install in the subdirectory `stage\lib`

```
b2 toolset=gcc address-model=64 --build-type=complete --with-program_options stage
```

5. Compile the desired libraries for 32bit architecture and install in the subdirectory `stage32\lib`

```
b2 toolset=gcc address-model=32 --stagedir=stage32 --build-type=complete --with-program_options stage
```

### Mac OSX

Instructions for compiling boost libraries on Mac OSX using XCode compiler.

1. Download Boost for Linux and extract the archive
2. Open the terminal and place yourself in the extracted folder
3. Compile Boost.Build

```
./bootstrap.sh
```

4. Compile the desired libraries and install in the subdirectory `stage/lib`

```
./b2 --build-type=complete --layout=versioned --with-program_options stage
```


---------------------------------------------------------------------

Manage sources
----------------  
See <http://git-scm.com/doc>

and especially
<http://git-scm.com/book/en/Git-Basics-Recording-Changes-to-the-Repository>

### Clone remote repository

Anonymous access (read-only):

```
git clone https://gforge.inria.fr/git/biips/biips.git
```
    
GForge user access:

```
git clone git+ssh://<username>@scm.gforge.inria.fr//gitroot/biips/biips.git
```
    
### Commit (local)

See status

```
git status -s
```

Add new files or files already tracked to STAGED zone

```
git add file
```

Commit STAGED zone

```
git commit
```

Commit STAGED zone and all modifications of files already tracked (without having to do `git add`)

```
git commit -a
```
    
### Push (remote)

```
git push origin my_branch
```

Ex. for master branch:

```
git push origin master
```

### View all branches

```
git branch -a
```

### Create new branch

```
git branch new_branch [source_branch]
git checkout new_branch
```

or 

```
git checkout -b new_branch [source_branch]
```

### Pull (remote)

```
git pull origin my_branch
```


---------------------------------------------------------------------

Continuous integration
-----------------------
Continuous integration is hosted on [Inria CI](https://ci.inria.fr/biips).

You first need an account on CI website.

After logging in the CI website, go to *MyAccount* page and add your ssh public key.

Ask the Biips administrators to give you access to the project.

### Slaves

* Ubuntu 12.04 amd64
* Windows 7 64bit

### Connect to slaves

On CI website, go to your *Dashboard*.

In the Biips box, click *Manage Project* and then click *Slaves* on the left panel.

Click the *Connect* button on the desired slave to get connection commands that include your username.


---------------------------------------------------------------------

Coding rules
-----------------------

### Naming conventions

* source file: `MyClass.cpp`
* header file: `MyClass.hpp // one class by file`
* class: `MyClass`
* private/protected member: `myMember_`
* public member: `MyMember_`
* private/protected method: `myMethod`
* public method: `MyMethod`
* argument: `myArgument`
* local variable: `my_variable`
* pointer: `pObject, p_object, _pObject`
* namespace: `MyNamespace`
* iterator: `iter_container`
* constant: `MY_CONSTANT`
* enum item: `MY_ITEM`

### Other rules

* Everything is in the namespace `Biips`

```
#ifndef BIIPS_MYCLASS_HPP_
#define BIIPS_MYCLASS_HPP_

#endif /* BIIPS_MYCLASS_HPP_ */
```

* All primitive types are redefined in `BiipsTypes.hpp`
* destructor must be virtual
* virtuel inherited methods must be declared virtual
 
### Notes / Common errors

* Do not use `#ifndef #define` in `.cpp`
* Do not access the content of a null pointer or shared_ptr null
* Do not create a shared_ptr with an object not dynamically allocated
* Do not define a template function in a source file (only in headers)
* Library linking order : The traditional behavior of linkers is to search 
  for external functions from left to right in the libraries specified on the 
  command line. This means that a library containing the definition of a function 
  should appear after any source files or object files which use it.
* Do not pass-by-copy a polymorphic object

