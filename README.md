# cel
C++ Event Loop - Pretty much just a lazy wrapper around libuv

# Overview
A partial C++ wrapper around [libuv](https://libuv.org/) for building servers.

# Setup
Once the repo is checked out, make sure to setup the dependencies:
- Navigate to the root directory
- `git submodule init`
- `git submodule update`
### libuv
Follow the instructions on the [github repo](https://github.com/libuv/libuv).
#### Mac / Linux
Using [cmake](https://cmake.org/), do the following:
- `cd dep/libuv`
- `mkdir -p build`
- `cd build`
- `cmake ..`
- `cd ..`
- `cmake --build build`

# Building
The projects can be generated with premake.
### Mac / Linux
Run `premake.sh` and then you can use `make` in the `build/` folder.
### Windows
Run `premake.bat` and then open the solution in the `build/` folder.

# Running Tests
Once built, navigate to `bin/[Debug|Release]/` and run `EchoServer`.  In a shell you can run `nc localhost 8070` and anything you type will be echoed back.

# Usage
This library tries to be straightforward.  You will want to construct an App and then run it.
```
// First, always setup the global logger if desired.
YourLogger logger(someLogLevel /*see: cel::LogLevel*/);
cel::SetLogger(&logger);

// We can access the app singleton anywhere
cel::App& app = cel::App::GetInstance();
// Next setup all the App components
EchoServer server(8070, 128);
// Make sure to call app.Initialize before running (and only once)
app.Initialize(&server, nullptr);

// Finally run the app, which will execute the libuv event loop (once this returns the process will exit)
app.Run();
```
