 rm -rf build
 mkdir build
 cd core;
 cmake -G "MinGW Makefiles" -C "../config-release.cmake" -B "../build/core"
 cd ../build/core
 make
 cd ../../parsers;
 cmake -G "MinGW Makefiles" -C "../config-release.cmake" -B "../build/parsers"
 cd ../build/parsers
 make 
 cd ../../
 cmake -G "MinGW Makefiles" -C "config-release.cmake" -B "build"
 cd build
 make