#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
// FONT LOADING
//==============================================================================

namespace
{
    juce::Typeface::Ptr loadRibesFont (
        const juce::String& filename)
    {
        // Search binary resources by their original filename.
        // This avoids depending on Projucer's generated variable names.

        for (int i = 0;
             i < BinaryData::namedResourceListSize;
             ++i)
        {
            const juce::String originalName =
                BinaryData::originalFilenames[i];

            const juce::String resourceFilename =
                originalName.fromLastOccurrenceOf ("/", false, false);

            if (resourceFilename == filename)
            {
                int dataSize = 0;

                const char* data =
                    BinaryData::getNamedResource (
                        BinaryData::namedResourceList[i],
                        dataSize
                    );

                if (data != nullptr && dataSize > 0)
                {
                    return juce::Typeface::
                        createSystemTypefaceFor (
                            data,
                            static_cast<size_t> (dataSize)
                        );
                }
            }
        }

        return {};
    }
}

//==============================================================================
// CUSTOM FADER
//==============================================================================

void BasicGainAudioProcessorEditor::
MinimalFaderLookAndFeel::drawLinearSlider (
    juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    juce::ignoreUnused (
        minSliderPos,
        maxSliderPos,
        style
    );

    const float centreX =
        static_cast<float> (x) +
        static_cast<float> (width) * 0.5f;

    const float top =
        static_cast<float> (y) + 18.0f;

    const float bottom =
        static_cast<float> (y + height) - 18.0f;

    //==========================================================================
    // TRACK

    g.setColour (juce::Colour (0xffD6DFDE));

    g.fillRoundedRectangle (
        centreX - 1.0f,
        top,
        2.0f,
        bottom - top,
        1.0f
    );

    //==========================================================================
    // ACTIVE TRACK

    const float thumbY = juce::jlimit (
        top,
        bottom,
        sliderPos
    );

    g.setColour (juce::Colour (0xffC85628));

    g.fillRoundedRectangle (
        centreX - 1.0f,
        thumbY,
        2.0f,
        bottom - thumbY,
        1.0f
    );

    //==========================================================================
    // THUMB: MINIMAL HORIZONTAL BAR

    const float thumbWidth = 46.0f;
    const float thumbHeight = 10.0f;

    g.setColour (juce::Colour (0xff1D252B));

    g.fillRoundedRectangle (
        centreX - thumbWidth * 0.5f,
        thumbY - thumbHeight * 0.5f,
        thumbWidth,
        thumbHeight,
        2.0f
    );
}

//==============================================================================
// CONSTRUCTOR
//==============================================================================

BasicGainAudioProcessorEditor::
BasicGainAudioProcessorEditor (
    BasicGainAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p)
{
    //==========================================================================
    // LOAD RIBES FONTS

    ribesRegular = loadRibesFont ("Ribes-Regular.otf");
    ribesLight   = loadRibesFont ("Ribes-Light.otf");
    ribesBlack   = loadRibesFont ("Ribes-Black.otf");

    //==========================================================================
    // GAIN SLIDER

    gainSlider.setSliderStyle (
        juce::Slider::LinearVertical
    );

    gainSlider.setTextBoxStyle (
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    gainSlider.setLookAndFeel (&faderLookAndFeel);

    // Double-click returns to unity gain.
    gainSlider.setDoubleClickReturnValue (
        true,
        0.0
    );

    // Repaint when the slider or DAW automation changes.
    gainSlider.onValueChange = [this]
    {
        repaint();
    };

    addAndMakeVisible (gainSlider);

    //==========================================================================
    // PARAMETER ATTACHMENT

    gainAttachment =
        std::make_unique<SliderAttachment> (
            audioProcessor.apvts,
            "gain",
            gainSlider
        );

    //==========================================================================
    // WINDOW

    setSize (360, 420);
}

//==============================================================================
// DESTRUCTOR
//==============================================================================

BasicGainAudioProcessorEditor::
~BasicGainAudioProcessorEditor()
{
    // Detach the parameter before destroying the slider.
    gainAttachment.reset();

    // Remove the custom LookAndFeel before it is destroyed.
    gainSlider.setLookAndFeel (nullptr);
}

//==============================================================================
// FONT HELPERS
//==============================================================================

juce::Font BasicGainAudioProcessorEditor::makeFont (
    juce::Typeface::Ptr typeface,
    float height) const
{
    if (typeface != nullptr)
    {
        return juce::Font (
            juce::FontOptions (typeface)
                .withHeight (height)
        );
    }

    // Fallback if a font resource is missing.
    return juce::Font (
        juce::FontOptions (height)
    );
}

juce::String BasicGainAudioProcessorEditor::getGainText() const
{
    const double value = gainSlider.getValue();

    return juce::String (value, 1) + " dB";
}

//==============================================================================
// PAINT
//==============================================================================

void BasicGainAudioProcessorEditor::paint (
    juce::Graphics& g)
{
    //==========================================================================
    // BACKGROUND

    g.fillAll (backgroundColour);

    //==========================================================================
    // HEADER

    g.setColour (textColour);

    g.setFont (
        makeFont (ribesRegular, 23.0f)
    );

    g.drawText (
        "BASIC GAIN",
        25,
        23,
        getWidth() - 50,
        36,
        juce::Justification::centredLeft
    );

    // Header index.

    g.setColour (secondaryColour);

    g.setFont (
        makeFont (ribesLight, 12.0f)
    );

    g.drawText (
        "01 / AUDIO",
        getWidth() - 118,
        30,
        93,
        24,
        juce::Justification::centredRight
    );

    //==========================================================================
    // DIVIDER

    g.setColour (lineColour);

    g.drawLine (
        25.0f,
        77.0f,
        static_cast<float> (getWidth() - 25),
        77.0f,
        1.0f
    );

    //==========================================================================
    // GAIN VALUE

    g.setColour (textColour);

    g.setFont (
        makeFont (ribesBlack, 42.0f)
    );

    g.drawFittedText (
        getGainText(),
        25,
        91,
        getWidth() - 50,
        58,
        juce::Justification::centred,
        1
    );

    //==========================================================================
    // SCALE LABELS

    g.setColour (secondaryColour);

    g.setFont (
        makeFont (ribesLight, 12.0f)
    );

    // The slider occupies y = 164 to 344.
    // Its track extends 18 px inward on each end.

    const float trackTop = 182.0f;
    const float trackBottom = 326.0f;

    const float zeroY =
        trackTop +
        (12.0f / 72.0f) *
        (trackBottom - trackTop);

    g.drawText (
        "+12",
        241,
        static_cast<int> (trackTop - 12.0f),
        60,
        24,
        juce::Justification::centredLeft
    );

    g.drawText (
        "0",
        241,
        static_cast<int> (zeroY - 12.0f),
        60,
        24,
        juce::Justification::centredLeft
    );

    g.drawText (
        "-60",
        241,
        static_cast<int> (trackBottom - 12.0f),
        60,
        24,
        juce::Justification::centredLeft
    );

    //==========================================================================
    // GAIN LABEL

    g.setColour (textColour);

    g.setFont (
        makeFont (ribesRegular, 15.0f)
    );

    g.drawText (
        "GAIN",
        0,
        344,
        getWidth(),
        24,
        juce::Justification::centred
    );

    //==========================================================================
    // FOOTER

    g.setColour (lineColour);

    g.drawLine (
        25.0f,
        386.0f,
        static_cast<float> (getWidth() - 25),
        386.0f,
        1.0f
    );

    g.setColour (secondaryColour);

    g.setFont (
        makeFont (ribesLight, 11.0f)
    );

    g.drawText (
        "BASIC GAIN",
        25,
        391,
        160,
        19,
        juce::Justification::centredLeft
    );

    g.drawText (
        "V 1.0",
        getWidth() - 105,
        391,
        80,
        19,
        juce::Justification::centredRight
    );
}

//==============================================================================
// LAYOUT
//==============================================================================

void BasicGainAudioProcessorEditor::resized()
{
    gainSlider.setBounds (
        getWidth() / 2 - 52,
        164,
        104,
        180
    );
}
