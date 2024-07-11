#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace cctn
{

//==============================================================================
class HostSyncAudioSourcePlayer final
{
public:
    //==============================================================================
    HostSyncAudioSourcePlayer();
    ~HostSyncAudioSourcePlayer();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void processBlockWithPositionInfo(juce::AudioBuffer<float>& audioBuffer, juce::MidiBuffer& midiBuffer, const juce::AudioPlayHead::PositionInfo& positionInfo);

    //==============================================================================
    void setAudioBufferToPlay(const juce::AudioBuffer<float>& sourceAudioBuffer, double sourceSampleRate);
    void clearAudioBufferToPlay();

    //==============================================================================
    void doResamplingIfNeeded();
    void makeAudioSourceResampled();

    //==============================================================================
    double getTimeLengthInSeconds() const;

private:
    //==============================================================================
    // Audio source related.
    juce::AudioBuffer<float> sourceAudioBufferCache;

    std::unique_ptr<juce::MemoryAudioSource> memoryAudioSourceOriginal { nullptr };
    double sampleRateAudioSourceOriginal { 0.0 };

    std::unique_ptr<juce::MemoryAudioSource> memoryAudioSourceResampled { nullptr };
    double sampleRateAudioSourceResampled { 0.0 };
    
    juce::int64 estimatedNextReadSamplePosition { 0 };

    // PlayerConfig
    double sampleRateToPlay { 0.0 };

    juce::SpinLock mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HostSyncAudioSourcePlayer)
};

}