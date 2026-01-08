/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// initialize static variables
std::vector<AutoGainerAudioProcessor*> PluginHub::instances;
juce::CriticalSection PluginHub::instanceLock;

//==============================================================================
AutoGainerAudioProcessor::AutoGainerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    PluginHub::addInstance(this);

    // Instance 1, Instance 2, Instance 3
    trackName = "Channel " + juce::String(PluginHub::instances.size());
}

AutoGainerAudioProcessor::~AutoGainerAudioProcessor()
{
    PluginHub::removeInstance(this);
}

//==============================================================================
const juce::String AutoGainerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutoGainerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AutoGainerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AutoGainerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AutoGainerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutoGainerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AutoGainerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutoGainerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AutoGainerAudioProcessor::getProgramName (int index)
{
    return {};
}

void AutoGainerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AutoGainerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AutoGainerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutoGainerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AutoGainerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if(isAnalyzing.load())
    {
        float currentLevel = buffer.getRMSLevel(0, 0, buffer.getNumSamples());

        float currentAcc = accumulatedRMS.load();
        accumulatedRMS.store(currentAcc + currentLevel);

        measurementCount++;

        float currentMagnitude = buffer.getMagnitude(0, 0, buffer.getNumSamples());
        float storedPeak = maxPeak.load();
        if(currentMagnitude > storedPeak)
        {
            maxPeak.store(currentMagnitude);
        }

        currentRMS.store(currentLevel);
    }

    buffer.applyGain(gainToApply.load());
}

//==============================================================================
bool AutoGainerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AutoGainerAudioProcessor::createEditor()
{
    return new AutoGainerAudioProcessorEditor (*this);
}

//==============================================================================
void AutoGainerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AutoGainerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoGainerAudioProcessor();
}
