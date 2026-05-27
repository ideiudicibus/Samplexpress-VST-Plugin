#pragma once

#include "PluginProcessor.h"
#include "SamplexpressLookAndFeel.h"
#include "AdsrDisplayComponent.h"
#include "FilterResponseComponent.h"
#include "SpectrumAnalyzerComponent.h"
#include "WaveformDisplay.h"
#include "TabBar.h"
#include "PresetManager.h"
#include <juce_audio_processors/juce_audio_processors.h>

class SamplexpressAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                               private juce::Timer,
                                               public juce::FileDragAndDropTarget
{
public:
    explicit SamplexpressAudioProcessorEditor (SamplexpressAudioProcessor&);
    ~SamplexpressAudioProcessorEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void loadButtonClicked();
    void updateSampleInfo();
    void switchToTab (int tabIndex);

    // FileDragAndDropTarget
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void fileDragEnter (const juce::StringArray& files, int x, int y) override;
    void fileDragExit (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

    SamplexpressAudioProcessor& processorRef;
    SamplexpressLookAndFeel customLookAndFeel;

    juce::Label titleLabel;
    juce::TextButton loadButton;
    juce::TextButton playButton;
    juce::Label fileNameLabel;
    juce::Label sampleInfoLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;
    bool isDragOver{false};

    // Invisible backing sliders for APVTS
    juce::Slider volAttackSlider, volDecaySlider, volSustainSlider, volReleaseSlider;
    juce::Slider filtAttackSlider, filtDecaySlider, filtSustainSlider, filtReleaseSlider;
    juce::Slider pitchAttackSlider, pitchDecaySlider, pitchSustainSlider, pitchReleaseSlider;
    juce::Slider filtCutoffSlider, filtResonanceSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttackAttachment, volDecayAttachment, volSustainAttachment, volReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filtAttackAttachment, filtDecayAttachment, filtSustainAttachment, filtReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttackAttachment, pitchDecayAttachment, pitchSustainAttachment, pitchReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filtCutoffAttachment, filtResonanceAttachment;

    // Interactive displays
    AdsrDisplayComponent volAdsrDisplay, filtAdsrDisplay, pitchAdsrDisplay;
    FilterResponseComponent filtResponseDisplay;
    SpectrumAnalyzerComponent spectrumAnalyzer;
    WaveformDisplay waveformDisplay;
    TabBar tabBar;

    // Preset UI
    PresetManager presetManager;
    juce::ComboBox presetComboBox;
    juce::TextButton savePresetButton;
    juce::TextButton deletePresetButton;
    juce::Slider tabAlphaSlider;

    void updateTabAlpha();
    void refreshPresetList();
    void savePresetClicked();
    void deletePresetClicked();
    void presetChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplexpressAudioProcessorEditor)
};
