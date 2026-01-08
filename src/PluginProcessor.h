/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
*/

class AutoGainerAudioProcessor;

struct PluginHub
{
  // Store active plugin instances !!!!!!!!!!!!!!!! STATIC LIST
  static std::vector<AutoGainerAudioProcessor*> instances;

  // Lock for thread safety
  static juce::CriticalSection instanceLock;

  // add to list 
  static void addInstance(AutoGainerAudioProcessor* p)
  {
    const juce::ScopedLock sl(instanceLock);
    instances.push_back(p);
  }

  // remove from list
  static void removeInstance(AutoGainerAudioProcessor* p)
  {
    const juce::ScopedLock sl(instanceLock);
    instances.erase(std::remove(instances.begin(), instances.end(), p), instances.end());
  }
};

class AutoGainerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AutoGainerAudioProcessor();
    ~AutoGainerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void resetAnalysis()
    {
      accumulatedRMS.store(0.0f);
      measurementCount.store(0);
      maxPeak.store(0.0f);
      isAnalyzing.store(true);
    }

    void stopAnalysis()
    {
      isAnalyzing.store(false);
    }

    // Total RMS Accumulator
    std::atomic<float> accumulatedRMS {0.0f};

    // block count 
    std::atomic<int> measurementCount{0};


    // Current Calculated RMS Level 
    std::atomic<float> currentRMS {0.0f};

    // MaxPeak
    std::atomic<float> maxPeak{0.0f};

    // Gain to apply
    std::atomic<float> gainToApply {1.0f};

    // is Analyze true
    std::atomic<bool> isAnalyzing {false};

    // Channel Name
    juce::String trackName = "Instance";



private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoGainerAudioProcessor)
};
