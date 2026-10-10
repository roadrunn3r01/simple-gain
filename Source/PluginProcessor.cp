/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
BasicGainAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "GAIN", 1 },
        "Gain",
        juce::NormalisableRange<float> (-60.0f, 12.0f, 0.1f),
        0.0f
    ));

    return layout;
}



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

int BasicGainAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicGainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicGainAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicGainAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicGainAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicGainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BasicGainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicGainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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


void BasicGainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Limpiar canales de salida sin entrada
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Leer el valor del parámetro GAIN en decibelios
    auto gainDb = apvts.getRawParameterValue ("GAIN")->load();

    // Convertir decibelios a ganancia lineal
    auto gainLinear = juce::Decibels::decibelsToGain (gainDb);

    // Aplicar ganancia a cada muestra de cada canal
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= gainLinear;
        }
    }
}



//==============================================================================
bool BasicGainAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicGainAudioProcessor::createEditor()
{
    return new BasicGainAudioProcessorEditor (*this);
}

//==============================================================================
void BasicGainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicGainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicGainAudioProcessor();
}
