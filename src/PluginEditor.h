/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"


class InstanceListModel : public juce::ListBoxModel
{
  public:

    int getNumRows() override
    {
      const juce::ScopedLock sl(PluginHub::instanceLock);
      return PluginHub::instances.size();
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g,
                          int width, int height, bool rowIsSelected) override
    {
      const juce::ScopedLock sl(PluginHub::instanceLock);

      if(auto* processor = PluginHub::instances[rowNumber])
      {
        if(rowIsSelected) g.fillAll(juce::Colours::lightblue);

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);

        float currentdB = juce::Decibels::gainToDecibels(processor->gainToApply.load());

        juce::String text = processor->trackName + " | Gain: " + 
                                juce::String(currentdB, 1) + " dB";

            g.drawText(text, 5, 0, width, height, juce::Justification::centredLeft, true);
      }
    }

    void listBoxItemClicked(int row, const juce::MouseEvent&) override {}
};

//==============================================================================
/**
*/
class AutoGainerAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    AutoGainerAudioProcessorEditor (AutoGainerAudioProcessor&);
    ~AutoGainerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoGainerAudioProcessor& audioProcessor;

    juce::TextButton analyzeButton {"ANALYZE & MATCH (-18dB)"};
    InstanceListModel listModel;
    juce::ListBox trackList;

    bool isUIAnalyzing = false;
    int countDownFrames = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoGainerAudioProcessorEditor)
};
