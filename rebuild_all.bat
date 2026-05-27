@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >/dev/null 2>&1
cmake --build build --config Debug
copy /Y "build\Samplexpress_artefacts\Debug\VST3\Samplexpress.vst3\Contents\x86_64-win\Samplexpress.vst3" "%LOCALAPPDATA%\Programs\Common\VST3\Samplexpress.vst3"
if errorlevel 1 (
    echo Creating per-user VST3 directory...
    mkdir "%LOCALAPPDATA%\Programs\Common\VST3" 2>nul
    copy /Y "build\Samplexpress_artefacts\Debug\VST3\Samplexpress.vst3\Contents\x86_64-win\Samplexpress.vst3" "%LOCALAPPDATA%\Programs\Common\VST3\Samplexpress.vst3"
)
