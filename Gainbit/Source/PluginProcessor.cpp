/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
using ChainSettings = GainbitAudioProcessor::ChainSettings;

//==============================================================================
GainbitAudioProcessor::GainbitAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GainbitAudioProcessor::~GainbitAudioProcessor()
{
}

//==============================================================================
const juce::String GainbitAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainbitAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainbitAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainbitAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainbitAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainbitAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainbitAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainbitAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GainbitAudioProcessor::getProgramName (int index)
{
    return {};
}

void GainbitAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GainbitAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    /* Initialize DSP Chain */
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void GainbitAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainbitAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GainbitAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    UpdateAll();

    /* Process Contexts */
    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool GainbitAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainbitAudioProcessor::createEditor()
{
    return new GainbitAudioProcessorEditor (*this);
}

//==============================================================================
void GainbitAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void GainbitAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
        UpdateAll();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainbitAudioProcessor();
}

//==============================================================================
GainbitAudioProcessor::ChainSettings GainbitAudioProcessor::getChainSettingsFromAPVTS(juce::AudioProcessorValueTreeState& apvts) {
    ChainSettings settings;

    settings.gain = apvts.getRawParameterValue("Gain")->load();

    settings.bitCrusherDepth = apvts.getRawParameterValue("BitCrusherDepth")->load();
    settings.bitCrusherRate = apvts.getRawParameterValue("BitCrusherRate")->load();

    return settings;
}

void GainbitAudioProcessor::UpdateAll()
{
    ChainSettings settings = getChainSettingsFromAPVTS(apvts);
    UpdateChainFromSettings(leftChain, settings);
    UpdateChainFromSettings(rightChain, settings);
}

void GainbitAudioProcessor::UpdateChainFromSettings(MonoChain& chain, ChainSettings settings)
{
    chain.get<ChainPositions::Gain>().setGainDecibels(settings.gain);
    auto& bitCrusher = chain.get<ChainPositions::BitCrusher>();
    bitCrusher.setBitDepth(settings.bitCrusherDepth);
    bitCrusher.setBitRate(settings.bitCrusherRate);
}

juce::AudioProcessorValueTreeState::ParameterLayout GainbitAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    /* Plugin Output Gain*/
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain", "Gain", juce::NormalisableRange<float>(-48.0f, 48.0f, 0.1f, 1.0f), 0.0f));

    /* Bit Crusher Settings */
    layout.add(std::make_unique<juce::AudioParameterInt>("BitCrusherDepth", "Bit Crusher Depth", 2, 32, 8));
    layout.add(std::make_unique<juce::AudioParameterInt>("BitCrusherRate", "Bit Crusher Rate", 8, 196000, 4096));

    return layout;
}