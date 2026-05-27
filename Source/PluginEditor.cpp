#include "PluginEditor.h"

SamplexpressAudioProcessorEditor::SamplexpressAudioProcessorEditor (SamplexpressAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), spectrumAnalyzer (p)
{
    setSize (600, 500);
    setLookAndFeel (&customLookAndFeel);

    titleLabel.setText ("Samplexpress", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setFont (juce::Font (juce::FontOptions { 16.0f, juce::Font::bold }));
    addAndMakeVisible (titleLabel);

    loadButton.setButtonText ("Load");
    loadButton.onClick = [this] { loadButtonClicked(); };
    addAndMakeVisible (loadButton);

    playButton.setButtonText ("Play");
    playButton.onClick = [this] { processorRef.previewPlayRequested.store (true); };
    addAndMakeVisible (playButton);

    fileNameLabel.setText ("No sample loaded", juce::dontSendNotification);
    addAndMakeVisible (fileNameLabel);

    sampleInfoLabel.setText ("", juce::dontSendNotification);
    sampleInfoLabel.setFont (juce::Font (juce::FontOptions { 11.0f }));
    addAndMakeVisible (sampleInfoLabel);

    auto& apvts = processorRef.getAPVTS();

    auto setupInvisibleSlider = [] (juce::Slider& slider)
    {
        slider.setSliderStyle (juce::Slider::Rotary);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        // invisible — not added to component tree
    };

    for (auto* s : { &volAttackSlider, &volDecaySlider, &volSustainSlider, &volReleaseSlider,
                     &filtAttackSlider, &filtDecaySlider, &filtSustainSlider, &filtReleaseSlider,
                     &pitchAttackSlider, &pitchDecaySlider, &pitchSustainSlider, &pitchReleaseSlider })
        setupInvisibleSlider (*s);

    // Attachments
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

    // Interactive displays
    addAndMakeVisible (volAdsrDisplay);
    addAndMakeVisible (filtAdsrDisplay);
    addAndMakeVisible (pitchAdsrDisplay);

    // Filter response display
    addAndMakeVisible (filtResponseDisplay);
    filtResponseDisplay.setValueChangedCallback ([this] (float cut, float res)
    {
        filtCutoffSlider.setValue    (cut, juce::sendNotificationSync);
        filtResonanceSlider.setValue (res, juce::sendNotificationSync);
    });

    // Callbacks: display drag → update invisible sliders → APVTS
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

    // Make filter sliders invisible (still wired to APVTS)
    for (auto* s : { &filtCutoffSlider, &filtResonanceSlider })
        setupInvisibleSlider (*s);

    // Spectrum analyzer
    addAndMakeVisible (spectrumAnalyzer);
    spectrumAnalyzer.prepare (processorRef.getSampleRate());

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

    titleLabel.setBounds (bounds.removeFromTop (24));

    auto ctrlBar = bounds.removeFromTop (30);
    loadButton.setBounds (ctrlBar.removeFromLeft (70));
    ctrlBar.removeFromLeft (8);
    playButton.setBounds (ctrlBar.removeFromLeft (70));
    ctrlBar.removeFromLeft (8);
    presetComboBox.setBounds (ctrlBar.removeFromLeft (120));
    ctrlBar.removeFromLeft (4);
    savePresetButton.setBounds (ctrlBar.removeFromLeft (50));
    ctrlBar.removeFromLeft (4);
    deletePresetButton.setBounds (ctrlBar.removeFromLeft (50));
    ctrlBar.removeFromLeft (8);
    fileNameLabel.setBounds (ctrlBar);

    auto infoRow = bounds.removeFromTop (18);
    sampleInfoLabel.setBounds (infoRow);

    // Volume ADSR
    {
        auto area = bounds.removeFromTop (64);
        volAdsrDisplay.setBounds (area);
    }

    // Filter: response curve + ADSR display
    {
        auto area = bounds.removeFromTop (136);
        filtResponseDisplay.setBounds (area.removeFromTop (70));
        filtAdsrDisplay.setBounds (area);
    }

    // Pitch ADSR
    {
        auto area = bounds.removeFromTop (64);
        pitchAdsrDisplay.setBounds (area);
    }

    // Spectrum analyzer
    {
        auto area = bounds.removeFromTop (120);
        spectrumAnalyzer.setBounds (area);
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
    if (name.isEmpty())
        return;

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
    if (name.isEmpty())
        return;

    if (presetManager.deletePreset (name))
        refreshPresetList();
}
