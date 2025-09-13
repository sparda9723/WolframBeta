rename wasmLib.h wasmLib.cpp
em++ wasmLib.cpp -o wasmLib.js -std=c++20 -s MODULARIZE=1 -s EXPORT_NAME=Descartes -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']" --bind -sNO_DISABLE_EXCEPTION_CATCHING --closure 1 -O3 --strip-debug
rename wasmLib.cpp wasmLib.h
