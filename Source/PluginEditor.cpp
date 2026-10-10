/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicGainAudioProcessorEditor::BasicGainAudioProcessorEditor (BasicGainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)

{
    // Configurar el potenciómetro
    gainSlider.setSliderStyle (
        juce::Slider::RotaryHorizontalVerticalDrag);

    gainSlider.setTextBoxStyle (
        juce::Slider::TextBoxBelow, false, 80, 20);

    // Mostrar el potenciómetro
    addAndMakeVisible (gainSlider);

    // Conectar el slider al parámetro GAIN
    gainAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, "GAIN", gainSlider);

    // Tamaño de la ventana
    setSize (400, 300);
}


BasicGainAudioProcessorEditor::~BasicGainAudioProcessorEditor()
{
}

//==============================================================================
void BasicGainAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}


void BasicGainAudioProcessorEditor::resized()
{
    gainSlider.setBounds (125, 50, 150, 180);
}
