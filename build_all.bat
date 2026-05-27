@echo off
setlocal

:: Set JUCE framework path
set "JUCE_FRAMEWORK=C:\Users\ideiudicibus\JUCE"

:: Verify JUCE path exists
if not exist "%JUCE_FRAMEWORK%\CMakeLists.txt" (
    echo ERROR: JUCE not found at %JUCE_FRAMEWORK%
    exit /b 1
)

:: Setup MSVC environment
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 (
    echo ERROR: Failed to setup MSVC environment
    exit /b 1
)

:: Verify ninja
where ninja >nul 2>&1
if errorlevel 1 (
    echo ERROR: ninja not found in PATH
    exit /b 1
)

:: Clean build
echo Cleaning previous build...
if exist build rmdir /S /Q build

:: Configure
echo Configuring with CMake...
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="%JUCE_FRAMEWORK%"
if errorlevel 1 (
    echo ERROR: CMake configuration failed
    exit /b 1
)

:: Build
echo Building...
cmake --build build --config Debug
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

:: Copy VST3
echo Copying VST3 to local plugins folder...
set "VST3_DEST=%LOCALAPPDATA%\Programs\Common\VST3"
if not exist "%VST3_DEST%" mkdir "%VST3_DEST%"
copy /Y "build\Samplexpress_artefacts\Debug\VST3\Samplexpress.vst3\Contents\x86_64-win\Samplexpress.vst3" "%VST3_DEST%\Samplexpress.vst3"
if errorlevel 1 (
    echo WARNING: Failed to copy VST3 binary
)

echo Build complete!
