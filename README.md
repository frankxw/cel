# cel
C++ Event Loop - Pretty much just a lazy wrapper around libuv

# Overview
A partial C++ wrapper for [libuv](https://libuv.org/) for building servers.

# Setup
Once the repo is checked out, make sure to setup the dpendencies:
### libuv
- Navigate to the root directory
- git submodule init
- git submodule update
- cd libuv
- ./autogen.sh
- ./configure
- make

# Building
The projects can be generated with premake.
### Mac / Linux
Run `premake.sh` and then you can use `make` in the `build/` folder.
### Windows
Run `premake.bat` and then open the solution in the `build/` folder.

