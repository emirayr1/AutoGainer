/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AutoGainerAudioProcessorEditor::AutoGainerAudioProcessorEditor (AutoGainerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    trackList.setModel(&listModel);
    trackList.setColour(juce::ListBox::backgroundColourId, juce::Colours::black);
    trackList.setRowHeight(30);
    addAndMakeVisible(trackList);
    
    addAndMakeVisible(analyzeButton);

    analyzeButton.onClick = [this]
    {
      if(isUIAnalyzing)
      {
        isUIAnalyzing = false;
        analyzeButton.setButtonText("ANALYZE & MATCH (-18dB)");

        const juce::ScopedLock sl(PluginHub::instanceLock);
        for(auto* p : PluginHub::instances)
        {
          p->stopAnalysis();
        }
        return;
      }

      // START
      isUIAnalyzing = true;
      countDownFrames = 90; // 3 seconds 
      analyzeButton.setButtonText("Analyzing... (Wait 3s)");

      const juce::ScopedLock sl(PluginHub::instanceLock);
      for(auto* p : PluginHub::instances)
      {
        p->resetAnalysis();
      }
    };

    setSize (400, 500);

    startTimerHz(30);
}


void AutoGainerAudioProcessorEditor::timerCallback()
{
  trackList.updateContent();
  trackList.repaint();

  if(isUIAnalyzing)
  {
    countDownFrames--;

    float secondsLeft = countDownFrames / 30.0f;
    analyzeButton.setButtonText("Analyzing..." + juce::String(secondsLeft, 1) + "s");

    if(countDownFrames <= 0)
    {
      isUIAnalyzing = false;
      analyzeButton.setButtonText("ANALYZE & MATCH (-18dB)");

      const float targetLinear = juce::Decibels::decibelsToGain(-18.0f);

      const juce::ScopedLock sl(PluginHub::instanceLock);

      for(auto* plugin : PluginHub::instances)
      {
        plugin->stopAnalysis();

        float totalRMS = plugin->accumulatedRMS.load();
        int count = plugin->measurementCount.load();

        if(count > 0 && totalRMS > 0.0001f)
        {
          float averageRMS = totalRMS / (float)count;

          float neededGain = targetLinear / averageRMS;

          // Safety
          neededGain = juce::jmin(neededGain, juce::Decibels::decibelsToGain(24.0f));

          plugin->gainToApply.store(neededGain);
        }
      }
    }
  }
}


AutoGainerAudioProcessorEditor::~AutoGainerAudioProcessorEditor()
{
  stopTimer();
}

//==============================================================================
void AutoGainerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkgrey);

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawText("Auto Gainer", getLocalBounds().removeFromTop(40), juce::Justification::centred);
}

void AutoGainerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    area.removeFromTop(40);
    analyzeButton.setBounds(area.removeFromBottom(60).reduced(10));

    trackList.setBounds(area.reduced(10));
}
