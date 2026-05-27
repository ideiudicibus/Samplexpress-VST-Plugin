#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

class PresetManager
{
public:
    PresetManager();

    juce::StringArray getPresetList() const;
    bool savePreset (const juce::String& name, const juce::MemoryBlock& stateData);
    bool loadPreset (const juce::String& name, juce::MemoryBlock& stateData) const;
    bool deletePreset (const juce::String& name);
    bool presetExists (const juce::String& name) const;

    juce::File getPresetsDirectory() const;

private:
    juce::File getPresetFile (const juce::String& name) const;
    void ensurePresetsDirectoryExists() const;
    void createFactoryPresetIfNeeded();

    static constexpr auto presetExtension = ".samplexpress";
};
