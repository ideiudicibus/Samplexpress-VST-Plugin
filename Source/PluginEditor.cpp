#include "PluginEditor.h"

SamplexpressAudioProcessorEditor::SamplexpressAudioProcessorEditor (SamplexpressAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), visualKeyboard (p.getKeyboardState()), spectrumAnalyzer (p), waveformDisplay (p)
{
    setSize (600, 480);
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

    // Loop controls
    loopEnableButton.setButtonText ("Loop: OFF");
    loopEnableButton.setToggleState (false, juce::dontSendNotification);

    auto setupLoopSlider = [] (juce::Slider& s, juce::String /*name*/, float min, float max)
    {
        s.setSliderStyle (juce::Slider::LinearHorizontal);
        s.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 40, 20);
        s.setRange (min, max, 0.001f);
    };

    setupLoopSlider (loopStartSlider,  "Start",  0.0f, 1.0f);
    setupLoopSlider (loopEndSlider,    "End",    0.0f, 1.0f);
    setupLoopSlider (crossfadeSlider,  "XFade",  0.0f, 500.0f);

    loopStartAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "loop_start",   loopStartSlider);
    loopEndAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "loop_end",     loopEndSlider);
    crossfadeAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "crossfade_ms", crossfadeSlider);
    loopEnableAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, "loop_enable",  loopEnableButton);

    // Spectrum — added first so it sits at the bottom of the z-stack
    addAndMakeVisible (spectrumAnalyzer);
    spectrumAnalyzer.prepare (processorRef.getSampleRate());

    // Interactive displays — added after spectrum so they paint on top
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

    // Waveform + tabs
    addAndMakeVisible (waveformDisplay);
    addAndMakeVisible (tabBar);
    tabBar.setTabNames ({ "SAMPLE", "VOLUME", "FILTER", "PITCH" });
    tabBar.setTabChangedCallback ([this] (int idx) { switchToTab (idx); });

    // Preset UI (now in title bar — always visible regardless of tab)
    presetComboBox.setTextWhenNothingSelected ("Select preset...");
    presetComboBox.setVisible (true);
    addAndMakeVisible (presetComboBox);
    presetComboBox.onChange = [this] { presetChanged(); };

    savePresetButton.setButtonText ("Save");
    savePresetButton.setVisible (true);
    savePresetButton.onClick = [this] { savePresetClicked(); };
    addAndMakeVisible (savePresetButton);

    deletePresetButton.setButtonText ("Delete");
    deletePresetButton.setVisible (true);
    deletePresetButton.onClick = [this] { deletePresetClicked(); };
    addAndMakeVisible (deletePresetButton);

    // Keyboard — always visible at bottom
    addAndMakeVisible (visualKeyboard);

    // Loop controls — must be after spectrum / ADSR displays in z-order so they stay visible
    addAndMakeVisible (loopEnableButton);
    addAndMakeVisible (loopStartSlider);
    addAndMakeVisible (loopEndSlider);
    addAndMakeVisible (crossfadeSlider);

    // Tab transparency knob
    tabAlphaSlider.setSliderStyle (juce::Slider::Rotary);
    tabAlphaSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    tabAlphaSlider.setRange (0.1, 1.0, 0.01);
    tabAlphaSlider.setValue (0.2, juce::dontSendNotification);
    tabAlphaSlider.setTooltip ("Tab content opacity");
    addAndMakeVisible (tabAlphaSlider);
    tabAlphaSlider.onValueChange = [this] { updateTabAlpha(); };

    // Label background transparent so spectrum shows through
    sampleInfoLabel.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);

    updateTabAlpha();

    refreshPresetList();

    tabBar.setCurrentTab (0); // Default to SAMPLE tab (loop controls visible)
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
    auto titleBar = bounds.removeFromTop (28);
    // Alpha knob: 34 px diameter, vertically centred in the 28 px title bar by
    // extending the bounds 3 px above and 3 px below the title row.
    {
        auto knobArea = titleBar.removeFromLeft (34);
        tabAlphaSlider.setBounds (knobArea.getX(), knobArea.getY() - 3, 34, 34);
    }
    titleBar.removeFromLeft (8);
    titleLabel.setBounds (titleBar.removeFromLeft (110));
    titleBar.removeFromLeft (8);
    loadButton.setBounds (titleBar.removeFromLeft (55));
    titleBar.removeFromLeft (4);
    playButton.setBounds (titleBar.removeFromLeft (55));
    titleBar.removeFromLeft (8);
    presetComboBox.setBounds (titleBar.removeFromLeft (140));
    titleBar.removeFromLeft (6);
    savePresetButton.setBounds (titleBar.removeFromLeft (55));
    titleBar.removeFromLeft (4);
    deletePresetButton.setBounds (titleBar.removeFromLeft (55));
    titleBar.removeFromLeft (8);
    fileNameLabel.setBounds (titleBar);

    bounds.removeFromTop (4);

    // Waveform
    waveformDisplay.setBounds (bounds.removeFromTop (110));

    bounds.removeFromTop (4);

    // Tab bar
    tabBar.setBounds (bounds.removeFromTop (28));

    bounds.removeFromTop (4);

    // Content area — spectrum behind, tab controls on top
    auto content = bounds;

    // Position all tab content (only active one is visible)
    {
        auto sampleArea = content;
        sampleInfoLabel.setBounds (sampleArea.removeFromTop (20));

        auto loopBar = sampleArea.removeFromTop (26);
        loopEnableButton.setBounds (loopBar.removeFromLeft (60));
        loopBar.removeFromLeft (6);
        loopStartSlider.setBounds (loopBar.removeFromLeft (120));
        loopBar.removeFromLeft (4);
        loopEndSlider.setBounds (loopBar.removeFromLeft (120));
        loopBar.removeFromLeft (4);
        crossfadeSlider.setBounds (loopBar.removeFromLeft (120));
    }

    volAdsrDisplay.setBounds (content);

    {
        auto filterArea = content;
        filtResponseDisplay.setBounds (filterArea.removeFromTop (content.getHeight() / 2));
        filterArea.removeFromTop (2);
        filtAdsrDisplay.setBounds (filterArea);
    }

    pitchAdsrDisplay.setBounds (content);

    auto keyboardHeight = 50;
    auto contentMinusKeyboard = content.removeFromTop (content.getHeight() - keyboardHeight - 4);
    spectrumAnalyzer.setBounds (contentMinusKeyboard);
    visualKeyboard.setBounds (content);
}

void SamplexpressAudioProcessorEditor::switchToTab (int tabIndex)
{
    sampleInfoLabel.setVisible (false);
    loopEnableButton.setVisible (false);
    loopStartSlider.setVisible (false);
    loopEndSlider.setVisible (false);
    crossfadeSlider.setVisible (false);
    volAdsrDisplay.setVisible (false);
    filtResponseDisplay.setVisible (false);
    filtAdsrDisplay.setVisible (false);
    pitchAdsrDisplay.setVisible (false);
    // presetComboBox, savePresetButton, deletePresetButton live in the title bar — always visible
    // spectrumAnalyzer stays visible on every tab

    switch (tabIndex)
    {
        case 0:
            sampleInfoLabel.setVisible (true);
            loopEnableButton.setVisible (true);
            loopStartSlider.setVisible (true);
            loopEndSlider.setVisible (true);
            crossfadeSlider.setVisible (true);
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

    updateLoopMarkers();
}

void SamplexpressAudioProcessorEditor::updateLoopMarkers()
{
    bool enabled = loopEnableButton.getToggleState();
    loopEnableButton.setButtonText (enabled ? "Loop: ON" : "Loop: OFF");
    waveformDisplay.setLoopEnabled (enabled);
    if (enabled)
    {
        waveformDisplay.setLoopStart (static_cast<float> (loopStartSlider.getValue()));
        waveformDisplay.setLoopEnd   (static_cast<float> (loopEndSlider.getValue()));
    }
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

void SamplexpressAudioProcessorEditor::updateTabAlpha()
{
    auto alpha = static_cast<float> (tabAlphaSlider.getValue());
    volAdsrDisplay.setPanelAlpha (alpha);
    filtAdsrDisplay.setPanelAlpha (alpha);
    pitchAdsrDisplay.setPanelAlpha (alpha);
    filtResponseDisplay.setPanelAlpha (alpha);
}
