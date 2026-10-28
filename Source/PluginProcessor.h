/*
  ==============================================================================

    PluginProcessor.h
    BasicGain - Audio Gain Plugin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class BasicGainAudioProcessor : public juce::AudioProcessor
{
public:

    //==========================================================================
    // CONSTRUCTOR AND DESTRUCTOR

    BasicGainAudioProcessor();
    ~BasicGainAudioProcessor() override;

    //==========================================================================
    // AUDIO PROCESSING

    void prepareToPlay (double sampleRate,
                        int samplesPerBlock) override;

    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations

    bool isBusesLayoutSupported (
        const BusesLayout& layouts) const override;

    #endif

    void processBlock (
        juce::AudioBuffer<float>& buffer,
        juce::MidiBuffer& midiMessages) override;

    //==========================================================================
    // EDITOR

    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    //==========================================================================
    // PLUGIN INFORMATION

    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    //==========================================================================
    // PROGRAMS

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram (int index) override;

    const juce::String getProgramName (
        int index) override;

    void changeProgramName (
        int index,
        const juce::String& newName) override;

    //==========================================================================
    // STATE MANAGEMENT

    void getStateInformation (
        juce::MemoryBlock& destData) override;

    void setStateInformation (
        const void* data,
        int sizeInBytes) override;

    //==========================================================================
    // PARAMETERS

    // Stores plugin parameters and their current values.
    // Public so the editor can create slider attachments.

    juce::AudioProcessorValueTreeState apvts;

    // Creates the gain parameter definition.

    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();

private:

    //==========================================================================
    // GAIN SMOOTHING

    // Smooths changes in linear gain to avoid audio clicks.

    juce::SmoothedValue<
        float,
        juce::ValueSmoothingTypes::Linear
    > smoothedGain;

    //==========================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        BasicGainAudioProcessor
    )
};
