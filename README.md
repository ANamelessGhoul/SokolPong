# Pong with SDL + sokol_gfx

A simple pong recreation using SDL for the application layer and sokol_gfx for the rendering

## Building

Build using cmake with
```
cmake -G "MinGW Makefiles" -B build -D CMAKE_BUILD_TYPE=Release
cmake --build ./build
```
or run `build.bat`


The version of SDL linked against is the MinGW version. Don't forget to get the appropriate version if building with a different compiler.


