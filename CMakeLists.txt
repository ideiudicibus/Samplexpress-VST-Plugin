cmake_minimum_required(VERSION 3.22)

project(Samplexpress VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory($ENV{JUCE_FRAMEWORK} ${CMAKE_BINARY_DIR}/JUCE)

juce_add_plugin(Samplexpress
    PRODUCT_NAME "Samplexpress"
    COMPANY_NAME "Samplexpress"
    PLUGIN_MANUFACTURER_CODE Smpx
    PLUGIN_CODE Smpx
    FORMATS VST3 Standalone
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "$ENV{LOCALAPPDATA}/Programs/Common/VST3"
    DESCRIPTION "Samplexpress sampler synthesizer plugin"
)

target_sources(Samplexpress
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/SamplexpressVoice.cpp
        Source/SamplexpressLookAndFeel.cpp
        Source/AdsrDisplayComponent.cpp
        Source/FilterResponseComponent.cpp
        Source/SpectrumAnalyzerComponent.cpp
        Source/PresetManager.cpp
        Source/WaveformDisplay.cpp
        Source/TabBar.cpp
)

target_compile_definitions(Samplexpress
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_MP3AUDIOFORMAT=1
)

target_link_libraries(Samplexpress
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)