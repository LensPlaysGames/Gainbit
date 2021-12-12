/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <algorithm>

namespace juce {
    namespace dsp {
        class BitCrusher
        {
        public:
            BitCrusher() noexcept = default;

            //==============================================================================
            void setBitDepth(int newBitDepth) { bitDepth = newBitDepth; }

            int getBitDepth() { return bitDepth; }

            //==============================================================================
            /** Called before processing starts. */
            void prepare(const ProcessSpec& spec) noexcept
            {
                sampleRate = spec.sampleRate;
                reset();
            }

            /** Resets the internal state of the gain */
            void reset() noexcept
            {
            }

            //==============================================================================
            /** Returns the result of processing a single sample. */
            template <typename SampleType>
            SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType s) noexcept
            {
                int max = pow(2, bitDepth) - 1;
                return round(((s) + 1.0f) * max) / max - 1.0f;
            }

            /** Processes the input and output buffers supplied in the processing context. */
            template <typename ProcessContext>
            void process(const ProcessContext& context) noexcept
            {
                auto&& inBlock = context.getInputBlock();
                auto&& outBlock = context.getOutputBlock();

                jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
                jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

                auto len = inBlock.getNumSamples();
                auto numChannels = inBlock.getNumChannels();

                // If bypassed, don't process anything, just copy input block to output block.
                if (context.isBypassed)
                {
                    if (context.usesSeparateInputAndOutputBlocks())
                        outBlock.copyFrom(inBlock); 
                    return;
                }

                // Single channel
                if (numChannels == 1)
                {
                    // Apply to single channel
                    auto* src = inBlock.getChannelPointer(0);
                    auto* dst = outBlock.getChannelPointer(0);
                    // Apply to every sample within channel.
                    for (size_t i = 0; i < len; i++) {
                        dst[i] = processSample(src[i]);
                    }
                }
                else
                {
                    // Apply to all channels
                    for (size_t chan = 0; chan < numChannels; ++chan) {
                        auto* src = inBlock.getChannelPointer(chan);
                        auto* dst = outBlock.getChannelPointer(chan);
                        // Apply to every sample within channel.
                        for (size_t i = 0; i < len; i++) {
                            dst[i] = processSample(src[i]);
                        }
                    }
                }
            }

        private:
            double sampleRate = 0;
            int bitDepth = 8;
        };
    }
}

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
        float bitCrusherGain { 1.0f };
        int bitCrusherDepth{ 8 };
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