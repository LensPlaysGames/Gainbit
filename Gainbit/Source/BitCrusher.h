#pragma once

/* BitCrusher DSP Audio Processor Definition */
namespace juce {
    namespace dsp {
        class BitCrusher
        {
        public:
            BitCrusher() noexcept = default;

            //==============================================================================
            void setBitDepth(int newBitDepth) { bitDepth = newBitDepth; }
            void setBitRate(int newBitRate) { bitRate = newBitRate; }

            int getBitDepth() { return bitDepth; }
            int getBitRate() { return bitRate; }

            //==============================================================================
            /** Called before processing starts. */
            void prepare(const ProcessSpec& spec) noexcept
            {
                sampleRate = spec.sampleRate;
                reset();
            }

            /** Resets the internal state. */
            void reset() noexcept
            {
            }

            //==============================================================================
            /** Returns the result of processing a single sample. */
            template <typename SampleType>
            SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType s) noexcept
            {
                int max = pow(2, bitDepth) - 1;
                return round(((s)+1.0f) * max) / max - 0.999999f;
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

                // Apply to all channels
                for (size_t chan = 0; chan < numChannels; ++chan) {
                    // I don't technically need these declared here, but it makes everything more readable.
                    auto* src = inBlock.getChannelPointer(chan);
                    auto* dst = outBlock.getChannelPointer(chan);

                    /* Apply to every sample within channel.
                    *   To reduce sample rate, a new sample is only calculated every `rate` samples.
                    *   `rate` is a value that indicates how many incoming audio samples corroborate to a single processed sample.
                    *   It will not always be perfect due to the integer cast, but that's the whole point of bit crushers ;p.
                    */
                    int rate = std::max(1, (int)(sampleRate / bitRate));

                    auto sample = processSample(src[0]);
                    for (size_t i = 0; i < len; i++) {
                        if (i % rate == 0) { sample = processSample(src[i]); }
                        dst[i] = sample;
                    }
                }
            }

        private:
            double sampleRate = 0.0;
            int bitDepth = 8;
            int bitRate = 4096;
        };
    }
}