@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
echo === Cleaning and rebuilding ===
cmake --build build --config Debug --target clean 2>/dev/null
cmake --build build --config Debug
echo === Build complete ===
