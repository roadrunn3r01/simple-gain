/*
  ==============================================================================

    PluginProcessor.cpp
    BasicGain - Audio Gain Plugin

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// CONSTRUCTOR

BasicGainAudioProcessor::BasicGainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
        #if ! JucePlugin_IsMidiEffect
            #if ! JucePlugin_IsSynth
                .withInput ("Input", juce::AudioChannelSet::stereo(), true)
            #endif
                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
        #endif
    ),
    apvts (*this, nullptr, "Parameters", createParameterLayout())
#else
    : apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

BasicGainAudioProcessor::~BasicGainAudioProcessor()
{
}

//==============================================================================
// PLUGIN INFORMATION

const juce::String BasicGainAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicGainAudioProcessor::acceptsMidi() const
{
    #if JucePlugin_WantsMidiInput
        return true;
    #else
        return false;
    #endif
}

bool BasicGainAudioProcessor::producesMidi() const
{
    #if JucePlugin_ProducesMidiOutput
        return true;
    #else
        return false;
    #endif
}

bool BasicGainAudioProcessor::isMidiEffect() const
{
    #if JucePlugin_IsMidiEffect
        return true;
    #else
        return false;
    #endif
}

double BasicGainAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
// PROGRAMS

int BasicGainAudioProcessor::getNumPrograms()
{
    return 1;
}

int BasicGainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicGainAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String BasicGainAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void BasicGainAudioProcessor::changeProgramName (
    int index,
    const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
// AUDIO INITIALIZATION

void BasicGainAudioProcessor::prepareToPlay (
    double sampleRate,
    int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    // Smooth gain changes over 20 milliseconds.
    smoothedGain.reset (sampleRate, 0.02);

    // Read the current gain parameter in decibels.
    const float gainDB =
        apvts.getRawParameterValue ("gain")->load();

    // Convert decibels to linear amplitude.
    const float gainLinear =
        juce::Decibels::decibelsToGain (gainDB, -60.0f);

    // Initialize the gain without a transition.
    smoothedGain.setCurrentAndTargetValue (gainLinear);
}

void BasicGainAudioProcessor::releaseResources()
{
}

//==============================================================================
// CHANNEL CONFIGURATION

#ifndef JucePlugin_PreferredChannelConfigurations

bool BasicGainAudioProcessor::isBusesLayoutSupported (
    const BusesLayout& layouts) const
{
    #if JucePlugin_IsMidiEffect

        juce::ignoreUnused (layouts);
        return true;

    #else

        // Support mono and stereo output.
        if (layouts.getMainOutputChannelSet() !=
                juce::AudioChannelSet::mono()
            &&
            layouts.getMainOutputChannelSet() !=
                juce::AudioChannelSet::stereo())
        {
            return false;
        }

        // Input and output must have the same layout.
        #if ! JucePlugin_IsSynth
            if (layouts.getMainOutputChannelSet() !=
                layouts.getMainInputChannelSet())
            {
                return false;
            }
        #endif

        return true;

    #endif
}

#endif

//==============================================================================
// AUDIO PROCESSING

void BasicGainAudioProcessor::processBlock (
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    juce::ignoreUnused (midiMessages);

    const int totalNumInputChannels =
        getTotalNumInputChannels();

    const int totalNumOutputChannels =
        getTotalNumOutputChannels();

    const int numSamples = buffer.getNumSamples();

    // Clear output channels without corresponding input.
    for (int channel = totalNumInputChannels;
         channel < totalNumOutputChannels;
         ++channel)
    {
        buffer.clear (channel, 0, numSamples);
    }

    // Read gain in dB.
    const float gainDB =
        apvts.getRawParameterValue ("gain")->load();

    // Convert dB to linear amplitude.
    const float gainLinear =
        juce::Decibels::decibelsToGain (gainDB, -60.0f);

    // Set the new target gain.
    smoothedGain.setTargetValue (gainLinear);

    // Process every audio sample.
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get the next smoothed gain value.
        const float currentGain =
            smoothedGain.getNextValue();

        // Apply the same gain to every input channel.
        for (int channel = 0;
             channel < totalNumInputChannels;
             ++channel)
        {
            float* channelData =
                buffer.getWritePointer (channel);

            channelData[sample] *= currentGain;
        }
    }
}

//==============================================================================
// PARAMETER LAYOUT

juce::AudioProcessorValueTreeState::ParameterLayout
BasicGainAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Gain parameter: -60 dB to +12 dB.
    // Default value: 0 dB.
    // Step size: 0.1 dB.

    layout.add (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "gain", 1 },
            "Gain",
            juce::NormalisableRange<float>(
                -60.0f,
                12.0f,
                0.1f
            ),
            0.0f,
            juce::AudioParameterFloatAttributes()
                .withLabel ("dB")
        )
    );

    return layout;
}

//==============================================================================
// EDITOR

bool BasicGainAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor*
BasicGainAudioProcessor::createEditor()
{
    return new BasicGainAudioProcessorEditor (*this);
}

//==============================================================================
// SAVE AND RESTORE PLUGIN STATE

void BasicGainAudioProcessor::getStateInformation (
    juce::MemoryBlock& destData)
{
    // Copy the current parameter state.
    auto state = apvts.copyState();

    // Convert the state to XML.
    std::unique_ptr<juce::XmlElement> xml (
        state.createXml()
    );

    // Save the XML as binary data.
    copyXmlToBinary (*xml, destData);
}

void BasicGainAudioProcessor::setStateInformation (
    const void* data,
    int sizeInBytes)
{
    // Read XML from binary data.
    std::unique_ptr<juce::XmlElement> xml (
        getXmlFromBinary (data, sizeInBytes)
    );

    // Restore saved parameters.
    if (xml != nullptr &&
        xml->hasTagName (apvts.state.getType()))
    {
        apvts.replaceState (
            juce::ValueTree::fromXml (*xml)
        );
    }
}

//==============================================================================
// PLUGIN INSTANCE

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicGainAudioProcessor();
}
