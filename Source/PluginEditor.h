#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// BASIC GAIN
// Minimal Interface - Ribes Typography
//==============================================================================

class BasicGainAudioProcessorEditor
    : public juce::AudioProcessorEditor
{
public:

    BasicGainAudioProcessorEditor (BasicGainAudioProcessor&);
    ~BasicGainAudioProcessorEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    //==========================================================================
    // CUSTOM VERTICAL FADER

    class MinimalFaderLookAndFeel
        : public juce::LookAndFeel_V4
    {
    public:

        void drawLinearSlider (
            juce::Graphics& g,
            int x,
            int y,
            int width,
            int height,
            float sliderPos,
            float minSliderPos,
            float maxSliderPos,
            const juce::Slider::SliderStyle style,
            juce::Slider& slider) override;
    };

    //==========================================================================
    // PROCESSOR

    BasicGainAudioProcessor& audioProcessor;

    //==========================================================================
    // COLOURS

    const juce::Colour backgroundColour { 0xffF1F6F0 };
    const juce::Colour textColour       { 0xff1D252B };
    const juce::Colour accentColour     { 0xffC85628 };
    const juce::Colour secondaryColour  { 0xff8A755E };
    const juce::Colour lineColour       { 0xffD6DFDE };

    //==========================================================================
    // TYPOGRAPHY

    juce::Typeface::Ptr ribesRegular;
    juce::Typeface::Ptr ribesLight;
    juce::Typeface::Ptr ribesBlack;

    //==========================================================================
    // FADER

    MinimalFaderLookAndFeel faderLookAndFeel;

    juce::Slider gainSlider;

    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> gainAttachment;

    //==========================================================================
    // FONT HELPERS

    juce::Font makeFont (
        juce::Typeface::Ptr typeface,
        float height) const;

    juce::String getGainText() const;

    //==========================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        BasicGainAudioProcessorEditor
    )
};
