#include "PluginEditor.h"

SamplexpressAudioProcessorEditor::SamplexpressAudioProcessorEditor (SamplexpressAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), spectrumAnalyzer (p), waveformDisplay (p)
{
    setSize (600, 440);
    setLookAndFeel (&customLookAndFeel);

    titleLabel.setText ("Samplexpress", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::left);
    titleLabel.setFont (juce::Font (juce::FontOptions { 14.0f, juce::Font::bold }));
    addAndMakeVisible (titleLabel);

    loadButton.setButtonText ("Load");
    loadButton.onClick = [this] { loadButtonClicked(); };
    addAndMakeVisible (loadButton);

    playButton.setButtonText ("Play");
    playButton.onClick = [this] { processorRef.previewPlayRequested.store (true); };
    addAndMakeVisible (playButton);

    fileNameLabel.setText ("", juce::dontSendNotification);
    fileNameLabel.setFont (juce::Font (juce::FontOptions { 11.0f }));
    addAndMakeVisible (fileNameLabel);

    sampleInfoLabel.setText ("", juce::dontSendNotification);
    sampleInfoLabel.setFont (juce::Font (juce::FontOptions { 11.0f }));
    addAndMakeVisible (sampleInfoLabel);

    auto& apvts = processorRef.getAPVTS();

    auto setupInvisibleSlider = [] (juce::Slider& slider)
    {
        slider.setSliderStyle (juce::Slider::Rotary);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    };

    for (auto* s : { &volAttackSlider, &volDecaySlider, &volSustainSlider, &volReleaseSlider,
                     &filtAttackSlider, &filtDecaySlider, &filtSustainSlider, &filtReleaseSlider,
                     &pitchAttackSlider, &pitchDecaySlider, &pitchSustainSlider, &pitchReleaseSlider })
        setupInvisibleSlider (*s);

    volAttackAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "vol_attack",  volAttackSlider);
    volDecayAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "vol_decay",   volDecaySlider);
    volSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "vol_sustain", volSustainSlider);
    volReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "vol_release", volReleaseSlider);

    filtAttackAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "filt_attack",  filtAttackSlider);
    filtDecayAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "filt_decay",   filtDecaySlider);
    filtSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "filt_sustain", filtSustainSlider);
    filtReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "filt_release", filtReleaseSlider);

    pitchAttackAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "pitch_attack",  pitchAttackSlider);
    pitchDecayAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "pitch_decay",   pitchDecaySlider);
    pitchSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "pitch_sustain", pitchSustainSlider);
    pitchReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "pitch_release", pitchReleaseSlider);

    filtCutoffAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "filt_cutoff",    filtCutoffSlider);
    filtResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "filt_resonance", filtResonanceSlider);

    for (auto* s : { &filtCutoffSlider, &filtResonanceSlider })
        setupInvisibleSlider (*s);

    // Interactive displays
    addAndMakeVisible (volAdsrDisplay);
    addAndMakeVisible (filtAdsrDisplay);
    addAndMakeVisible (pitchAdsrDisplay);
    addAndMakeVisible (filtResponseDisplay);

    volAdsrDisplay.setValueChangedCallback ([this] (float a, float d, float s, float r)
    {
        volAttackSlider.setValue  (a, juce::sendNotificationSync);
        volDecaySlider.setValue   (d, juce::sendNotificationSync);
        volSustainSlider.setValue (s, juce::sendNotificationSync);
        volReleaseSlider.setValue (r, juce::sendNotificationSync);
    });

    filtAdsrDisplay.setValueChangedCallback ([this] (float a, float d, float s, float r)
    {
        filtAttackSlider.setValue  (a, juce::sendNotificationSync);
        filtDecaySlider.setValue   (d, juce::sendNotificationSync);
        filtSustainSlider.setValue (s, juce::sendNotificationSync);
        filtReleaseSlider.setValue (r, juce::sendNotificationSync);
    });

    pitchAdsrDisplay.setValueChangedCallback ([this] (float a, float d, float s, float r)
    {
        pitchAttackSlider.setValue  (a, juce::sendNotificationSync);
        pitchDecaySlider.setValue   (d, juce::sendNotificationSync);
        pitchSustainSlider.setValue (s, juce::sendNotificationSync);
        pitchReleaseSlider.setValue (r, juce::sendNotificationSync);
    });

    filtResponseDisplay.setValueChangedCallback ([this] (float cut, float res)
    {
        filtCutoffSlider.setValue    (cut, juce::sendNotificationSync);
        filtResonanceSlider.setValue (res, juce::sendNotificationSync);
    });

    // Spectrum
    addAndMakeVisible (spectrumAnalyzer);
    spectrumAnalyzer.prepare (processorRef.getSampleRate());

    // Waveform + tabs
    addAndMakeVisible (waveformDisplay);
    addAndMakeVisible (tabBar);
    tabBar.setTabNames ({ "SAMPLE", "VOLUME", "FILTER", "PITCH" });
    tabBar.setTabChangedCallback ([this] (int idx) { switchToTab (idx); });

    // Preset UI
    presetComboBox.setTextWhenNothingSelected ("Select preset...");
    addAndMakeVisible (presetComboBox);
    presetComboBox.onChange = [this] { presetChanged(); };

    savePresetButton.setButtonText ("Save");
    savePresetButton.onClick = [this] { savePresetClicked(); };
    addAndMakeVisible (savePresetButton);

    deletePresetButton.setButtonText ("Delete");
    deletePresetButton.onClick = [this] { deletePresetClicked(); };
    addAndMakeVisible (deletePresetButton);

    refreshPresetList();

    tabBar.setCurrentTab (1); // Default to VOLUME tab
    startTimerHz (10);
}

SamplexpressAudioProcessorEditor::~SamplexpressAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void SamplexpressAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF121214u));

    if (isDragOver)
    {
        g.setColour (juce::Colours::cyan.withAlpha (0.7f));
        g.drawRect (getLocalBounds(), 3);
    }
}

void SamplexpressAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (16);

    // Title bar
    auto titleBar = bounds.removeFromTop (24);
    titleLabel.setBounds (titleBar.removeFromLeft (110));
    titleBar.removeFromLeft (12);
    loadButton.setBounds (titleBar.removeFromLeft (55));
    titleBar.removeFromLeft (4);
    playButton.setBounds (titleBar.removeFromLeft (55));
    titleBar.removeFromLeft (8);
    fileNameLabel.setBounds (titleBar);

    bounds.removeFromTop (4);

    // Waveform
    waveformDisplay.setBounds (bounds.removeFromTop (110));

    bounds.removeFromTop (4);

    // Tab bar
    tabBar.setBounds (bounds.removeFromTop (28));

    bounds.removeFromTop (4);

    // Content area split into top controls + bottom spectrum
    auto contentTop = bounds.removeFromTop (140);
    bounds.removeFromTop (4);
    auto spectrumArea = bounds;

    // Position all tab content (only active one is visible)
    {
        auto sampleArea = contentTop;
        sampleInfoLabel.setBounds (sampleArea.removeFromTop (20));
        auto presetBar = sampleArea.removeFromTop (26);
        presetComboBox.setBounds (presetBar.removeFromLeft (140));
        presetBar.removeFromLeft (6);
        savePresetButton.setBounds (presetBar.removeFromLeft (55));
        presetBar.removeFromLeft (4);
        deletePresetButton.setBounds (presetBar.removeFromLeft (55));
    }

    volAdsrDisplay.setBounds (contentTop);

    {
        auto filterArea = contentTop;
        filtResponseDisplay.setBounds (filterArea.removeFromTop (contentTop.getHeight() / 2));
        filterArea.removeFromTop (2);
        filtAdsrDisplay.setBounds (filterArea);
    }

    pitchAdsrDisplay.setBounds (contentTop);
    spectrumAnalyzer.setBounds (spectrumArea);
}

void SamplexpressAudioProcessorEditor::switchToTab (int tabIndex)
{
    sampleInfoLabel.setVisible (false);
    presetComboBox.setVisible (false);
    savePresetButton.setVisible (false);
    deletePresetButton.setVisible (false);
    volAdsrDisplay.setVisible (false);
    filtResponseDisplay.setVisible (false);
    filtAdsrDisplay.setVisible (false);
    pitchAdsrDisplay.setVisible (false);
    // spectrumAnalyzer stays visible on every tab

    switch (tabIndex)
    {
        case 0:
            sampleInfoLabel.setVisible (true);
            presetComboBox.setVisible (true);
            savePresetButton.setVisible (true);
            deletePresetButton.setVisible (true);
            break;
        case 1:
            volAdsrDisplay.setVisible (true);
            break;
        case 2:
            filtResponseDisplay.setVisible (true);
            filtAdsrDisplay.setVisible (true);
            break;
        case 3:
            pitchAdsrDisplay.setVisible (true);
            break;
    }
}

void SamplexpressAudioProcessorEditor::loadButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Load Audio Sample",
        juce::File::getSpecialLocation (juce::File::userDesktopDirectory),
        processorRef.getFormatManager().getWildcardForAllFormats(),
        true);

    fileChooser-> launchAsync (
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file != juce::File{})
                processorRef.loadFile (file);
        });
}

void SamplexpressAudioProcessorEditor::updateSampleInfo()
{
    if (processorRef.sampleChanged.exchange (false))
    {
        fileNameLabel.setText (processorRef.getLoadedFileName(), juce::dontSendNotification);

        auto sr = processorRef.getSampleSampleRate();
        auto numSamples = processorRef.getSampleBuffer().getNumSamples();
        auto duration = (sr > 0.0) ? static_cast<double> (numSamples) / sr : 0.0;

        juce::String info;
        info << juce::String (static_cast<int> (sr)) << " Hz, "
             << juce::String (duration, 1) << " sec, "
             << numSamples << " samples";

        sampleInfoLabel.setText (info, juce::dontSendNotification);
    }
}

void SamplexpressAudioProcessorEditor::timerCallback()
{
    updateSampleInfo();

    if (! volAdsrDisplay.isDragging())
        volAdsrDisplay.setParameters (
            static_cast<float> (volAttackSlider.getValue()),
            static_cast<float> (volDecaySlider.getValue()),
            static_cast<float> (volSustainSlider.getValue()),
            static_cast<float> (volReleaseSlider.getValue()));

    if (! filtAdsrDisplay.isDragging())
        filtAdsrDisplay.setParameters (
            static_cast<float> (filtAttackSlider.getValue()),
            static_cast<float> (filtDecaySlider.getValue()),
            static_cast<float> (filtSustainSlider.getValue()),
            static_cast<float> (filtReleaseSlider.getValue()));

    if (! pitchAdsrDisplay.isDragging())
        pitchAdsrDisplay.setParameters (
            static_cast<float> (pitchAttackSlider.getValue()),
            static_cast<float> (pitchDecaySlider.getValue()),
            static_cast<float> (pitchSustainSlider.getValue()),
            static_cast<float> (pitchReleaseSlider.getValue()));

    if (! filtResponseDisplay.isDragging())
        filtResponseDisplay.setParameters (
            static_cast<float> (filtCutoffSlider.getValue()),
            static_cast<float> (filtResonanceSlider.getValue()));
}

bool SamplexpressAudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (const auto& filePath : files)
    {
        juce::File file (filePath);
        if (file.hasFileExtension ("wav;mp3;aiff;aif"))
            return true;
    }
    return false;
}

void SamplexpressAudioProcessorEditor::fileDragEnter (const juce::StringArray&, int, int)
{
    isDragOver = true;
    repaint();
}

void SamplexpressAudioProcessorEditor::fileDragExit (const juce::StringArray&)
{
    isDragOver = false;
    repaint();
}

void SamplexpressAudioProcessorEditor::filesDropped (const juce::StringArray& files, int, int)
{
    isDragOver = false;
    repaint();

    for (const auto& filePath : files)
    {
        juce::File file (filePath);
        if (file.hasFileExtension ("wav;mp3;aiff;aif"))
        {
            processorRef.loadFile (file);
            break;
        }
    }
}

void SamplexpressAudioProcessorEditor::refreshPresetList()
{
    presetComboBox.clear();
    auto list = presetManager.getPresetList();
    for (int i = 0; i < list.size(); ++i)
        presetComboBox.addItem (list[i], i + 1);
}

void SamplexpressAudioProcessorEditor::presetChanged()
{
    auto name = presetComboBox.getText();
    if (name.isEmpty()) return;

    juce::MemoryBlock stateData;
    if (presetManager.loadPreset (name, stateData))
    {
        processorRef.setStateInformation (stateData.getData(),
                                            static_cast<int> (stateData.getSize()));
        processorRef.sampleChanged.store (true);
    }
}

void SamplexpressAudioProcessorEditor::savePresetClicked()
{
    auto* aw = new juce::AlertWindow ("Save Preset", "Enter preset name:",
                                       juce::AlertWindow::QuestionIcon, this);
    aw->addTextEditor ("name", "", "Name:");
    aw->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
    aw->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));
    aw->enterModalState (true,
        juce::ModalCallbackFunction::create ([this, aw] (int result)
        {
            if (result == 1)
            {
                auto name = aw->getTextEditorContents ("name");
                if (name.isNotEmpty())
                {
                    juce::MemoryBlock stateData;
                    processorRef.getStateInformation (stateData);
                    if (presetManager.savePreset (name, stateData))
                        refreshPresetList();
                }
            }
            delete aw;
        }));
}

void SamplexpressAudioProcessorEditor::deletePresetClicked()
{
    auto name = presetComboBox.getText();
    if (name.isEmpty()) return;

    if (presetManager.deletePreset (name))
        refreshPresetList();
}
