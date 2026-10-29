# BasicGain

*A minimal audio gain plugin built with C++ and JUCE.*

BasicGain is my first audio plugin development project. It is a simple VST3 effect that adjusts the amplitude of an incoming audio signal through a single gain control.

I wanted to start with one of the most fundamental operations in digital audio processing and understand how to implement it from the ground up — from individual audio samples to parameter automation, real-time processing, and graphical interface design.

The project is intentionally simple. Its purpose is to establish a solid foundation for developing more complex audio effects and instruments.

## Interface

![BasicGain plugin interface](Assets/basicgain-interface.png)

The interface features a custom vertical fader, a live decibel readout, and Ribes typography. I designed it around a minimal visual language, combining functionality with a distinct graphic identity.

## Features

- Adjustable gain from **−60 dB to +12 dB**, in 0.1 dB increments.
- Mono and stereo audio processing.
- 20 ms gain smoothing to reduce abrupt amplitude changes.
- DAW parameter automation.
- Parameter state saving and restoration.
- Custom graphical interface using JUCE.
- Double-click to reset the fader to 0 dB.

## How it works

At its core, BasicGain multiplies each incoming audio sample by a gain factor:

**y[n] = g[n] × x[n]**

Where `x[n]` represents the input sample, `g[n]` the linear gain factor, and `y[n]` the resulting output sample.

The user controls gain in decibels, which the plugin converts into a linear amplitude multiplier:

**g = 10^(dB / 20)**

At 0 dB, the multiplier is 1, meaning the signal passes through at its original amplitude.

To avoid abrupt changes when adjusting the fader, the plugin uses `juce::SmoothedValue` to interpolate gain changes over 20 milliseconds.

The processing is implemented in `processBlock()`, where the gain is applied to each sample across the available audio channels.

## Architecture

The plugin follows JUCE's `AudioProcessor` / `AudioProcessorEditor` architecture.

| Component | Responsibility |
|---|---|
| `PluginProcessor.cpp` | Audio processing, gain conversion, smoothing, and state management |
| `PluginEditor.cpp` | Interface rendering and interaction |
| `AudioProcessorValueTreeState` | Parameter storage, automation, and synchronization |
| `SmoothedValue` | Smooth transitions between gain values |
| `BinaryData` | Embedded font resources |

The audio processing and interface are separated, allowing the plugin to handle audio independently of graphical updates.

## Building

Developed using:

- C++
- JUCE 9
- Projucer
- Xcode
- macOS
- VST3

To build from source:

1. Clone the repository.
2. Open `BasicGain.jucer` in Projucer.
3. Configure the JUCE module paths for your installation.
4. Generate the Xcode project.
5. Open the project in Xcode and build the VST3 target.
6. Load the compiled plugin into a compatible DAW.

The project has been developed and tested on macOS using Ableton Live.

## What I learned

Although gain adjustment is one of the simplest DSP operations, implementing it as a functioning plugin introduced me to several important concepts:

- Working directly with digital audio buffers and samples.
- Understanding the relationship between decibels and linear amplitude.
- Managing parameters within a real-time audio application.
- Smoothing parameter changes to avoid audible discontinuities.
- Connecting DSP parameters to custom GUI controls.
- Saving and restoring plugin state.
- Building and debugging a VST3 plugin.

More importantly, this project helped me begin understanding how the different components of an audio plugin fit together.

BasicGain is the starting point of a series of projects through which I hope to explore increasingly complex approaches to audio processing, synthesis, and musical instrument design.

---

**Developed by [Martina Dulanto Ostornol](https://github.com/roadrunn3r01)**

*First audio plugin project · 2026*
