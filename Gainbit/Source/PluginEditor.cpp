/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainbitAudioProcessorEditor::GainbitAudioProcessorEditor (GainbitAudioProcessor& p) 
    : AudioProcessorEditor (&p), 
      audioProcessor (p), 
      gainAttachment(audioProcessor.apvts, "Gain", gainSlider),
      bitCrusher_bitDepthAttachment(audioProcessor.apvts, "BitCrusherDepth", bitCrusher_bitDepthSlider)
{
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, true, 50, 20);
    gainSlider.setTextBoxIsEditable(false);
    gainSlider.setRange(-48.0f, 48.0f, 0.1f);

    bitCrusher_bitDepthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    bitCrusher_bitDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, true, 50, 20);
    gainSlider.setTextBoxIsEditable(true);
    bitCrusher_bitDepthSlider.setRange(2, 32, 1);

    /* Add and make visible every component */
    for (auto& component : getComponents())
        addAndMakeVisible(component);

    setSize (360, 120);
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
    auto labelBounds = bounds.removeFromRight(bounds.getWidth() * 0.2f);
    int labelWidth = labelBounds.getWidth();
    labelBounds.removeFromLeft(labelWidth * 0.2f);

    /* Draw parameter labels */
    g.setColour(juce::Colours::white);
    juce::Font font = juce::Font();
    g.setFont(font);
    g.setFont(18);
    g.drawFittedText("Gain", labelBounds.removeFromTop(labelBounds.getHeight() * 0.5f), juce::Justification::centred, 1);
    g.drawFittedText("Bit Depth", labelBounds, juce::Justification::centred, 2);
}

void GainbitAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto labelBounds = bounds.removeFromRight(bounds.getWidth() * 0.2f);

    /* Set Gain Slider bounds to upper half */
    gainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5f));
    /* Set Bit Crusher Depth Slider bounds to lower half */
    bitCrusher_bitDepthSlider.setBounds(bounds);
}

//==============================================================================
std::vector<juce::Component*> GainbitAudioProcessorEditor::getComponents() {
    return {
        &gainSlider,
        &bitCrusher_bitDepthSlider
    };
}