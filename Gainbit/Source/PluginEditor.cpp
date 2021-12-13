#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainbitAudioProcessorEditor::GainbitAudioProcessorEditor (GainbitAudioProcessor& p) 
    : AudioProcessorEditor (&p), 
      audioProcessor (p), 
      gainAttachment(audioProcessor.apvts, "Gain", gainSlider),
      bitCrusher_bitDepthAttachment(audioProcessor.apvts, "BitCrusherDepth", bitCrusher_bitDepthSlider),
      bitCrusher_bitRateAttachment(audioProcessor.apvts, "BitCrusherRate", bitCrusher_bitRateSlider)
{
    auto& defaultLookAndFeel = getLookAndFeel();
    // Slider
    defaultLookAndFeel.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::red);
    defaultLookAndFeel.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    defaultLookAndFeel.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::darkgrey);

    #pragma region Configure Components
    /* Configure components */
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, true, 50, 20);
    gainSlider.setTextBoxIsEditable(false);
    gainSlider.setRange(-48.0f, 48.0f, 0.1f);

    bitCrusher_bitDepthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    bitCrusher_bitDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, true, 50, 20);
    bitCrusher_bitDepthSlider.setTextBoxIsEditable(false);
    bitCrusher_bitDepthSlider.setRange(2, 32, 1);

    bitCrusher_bitRateSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    bitCrusher_bitRateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, true, 50, 20);
    bitCrusher_bitRateSlider.setTextBoxIsEditable(true);
    bitCrusher_bitRateSlider.setNormalisableRange(juce::NormalisableRange<double>(8, 196000, 2, 0.333));
    #pragma endregion 

    /* Add and make visible every component */
    for (auto& component : getComponents())
        addAndMakeVisible(component);

    setSize (360, 160);
}

GainbitAudioProcessorEditor::~GainbitAudioProcessorEditor()
{
}

//==============================================================================
void GainbitAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    /* BOUNDS SHOULD MATCH resized() EXACTLY! */
    auto bounds = getLocalBounds();
    auto labelBounds = bounds.removeFromRight(bounds.getWidth() * 0.18f);
    int labelWidth = labelBounds.getWidth();
    labelBounds.removeFromLeft(labelWidth * 0.2f);

    /* Draw parameter labels */
    g.setColour(juce::Colours::white);
    juce::Font font = juce::Font();
    g.setFont(font);
    g.setFont(18);
    g.drawFittedText("Gain", labelBounds.removeFromTop(labelBounds.getHeight() * 0.333f), juce::Justification::centred, 1);
    g.drawFittedText("Bit Depth", labelBounds.removeFromTop(labelBounds.getHeight() * 0.5f), juce::Justification::centred, 2);
    g.drawFittedText("Bit Rate", labelBounds, juce::Justification::centred, 2);
}

void GainbitAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto labelBounds = bounds.removeFromRight(bounds.getWidth() * 0.18f);

    /* Set Gain Slider bounds to upper third */
    gainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.333f));
    /* Set Bit Crusher Depth Slider bounds to middle third */
    bitCrusher_bitDepthSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5f));
    /* Set Bit Crusher Rate Slider bounds to lower third*/
    bitCrusher_bitRateSlider.setBounds(bounds);
}

//==============================================================================
std::vector<juce::Component*> GainbitAudioProcessorEditor::getComponents() {
    return {
        &gainSlider,
        &bitCrusher_bitDepthSlider,
        & bitCrusher_bitRateSlider
    };
}