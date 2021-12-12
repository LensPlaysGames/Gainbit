/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GainbitAudioProcessorEditor   : public juce::AudioProcessorEditor
{
public:
    GainbitAudioProcessorEditor (GainbitAudioProcessor&);
    ~GainbitAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainbitAudioProcessor& audioProcessor;

    /* Return all components within editor */
    std::vector<juce::Component*> getComponents();

    /* Component Definitions */
    juce::Slider gainSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;

    juce::Slider bitCrusher_bitDepthSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment bitCrusher_bitDepthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainbitAudioProcessorEditor)
};
