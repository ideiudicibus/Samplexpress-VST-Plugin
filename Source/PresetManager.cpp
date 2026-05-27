#include "PresetManager.h"

PresetManager::PresetManager()
{
    ensurePresetsDirectoryExists();
    createFactoryPresetIfNeeded();
}

juce::File PresetManager::getPresetsDirectory() const
{
    return juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
           .getChildFile ("Samplexpress/Presets");
}

void PresetManager::ensurePresetsDirectoryExists() const
{
    getPresetsDirectory().createDirectory();
}

juce::File PresetManager::getPresetFile (const juce::String& name) const
{
    return getPresetsDirectory().getChildFile (name + presetExtension);
}

juce::StringArray PresetManager::getPresetList() const
{
    juce::StringArray presets;
    auto dir = getPresetsDirectory();
    if (! dir.exists())
        return presets;

    auto files = dir.findChildFiles (juce::File::findFiles, false, "*" + juce::String (presetExtension));
    for (const auto& f : files)
        presets.add (f.getFileNameWithoutExtension());

    presets.sortNatural();
    return presets;
}

bool PresetManager::savePreset (const juce::String& name, const juce::MemoryBlock& stateData)
{
    if (name.isEmpty())
        return false;

    auto file = getPresetFile (name);
    return file.replaceWithData (stateData.getData(), stateData.getSize());
}

bool PresetManager::loadPreset (const juce::String& name, juce::MemoryBlock& stateData) const
{
    auto file = getPresetFile (name);
    if (! file.existsAsFile())
        return false;

    return file.loadFileAsData (stateData);
}

bool PresetManager::deletePreset (const juce::String& name)
{
    auto file = getPresetFile (name);
    if (! file.existsAsFile())
        return false;

    return file.deleteFile();
}

bool PresetManager::presetExists (const juce::String& name) const
{
    return getPresetFile (name).existsAsFile();
}

void PresetManager::createFactoryPresetIfNeeded()
{
    if (! getPresetList().isEmpty())
        return;

    juce::ValueTree emptyState ("SAMPLEXPRESS");
    emptyState.setProperty ("sampleRate", 0.0, nullptr);
    emptyState.setProperty ("numChannels", 0, nullptr);
    emptyState.setProperty ("numSamples", 0, nullptr);
    emptyState.setProperty ("loadedFileName", "", nullptr);

    auto apvtsState = juce::ValueTree ("PARAMETERS");
    emptyState.addChild (apvtsState, -1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (emptyState.createXml());
    juce::String xmlStr = xml->toString();
    juce::MemoryBlock data (xmlStr.toRawUTF8(), static_cast<size_t> (xmlStr.getNumBytesAsUTF8()));

    savePreset ("Init", data);
}
