#pragma once

#include <JuceHeader.h>
#include "BitCrusher.h"

//==============================================================================
/**
*/
class GainbitAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GainbitAudioProcessor();
    ~GainbitAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    struct ChainSettings {
        float gain{ 1.0f };
        int bitCrusherDepth{ 8 }, bitCrusherRate{ 4096 };
    };

    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    ChainSettings getChainSettingsFromAPVTS(juce::AudioProcessorValueTreeState& apvts);

    void UpdateAll();

private:
    //==============================================================================
    using MonoChain = juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::BitCrusher>;
    enum ChainPositions { Gain, BitCrusher };

    MonoChain leftChain, rightChain;

    void UpdateChainFromSettings(MonoChain& chain, ChainSettings settings);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainbitAudioProcessor)
};