#include "cocotone_HostSyncAudioSourcePlayer.h"

namespace cctn
{

//==============================================================================
HostSyncAudioSourcePlayer::HostSyncAudioSourcePlayer()
    : sampleRateToPlay(0)
{
}

HostSyncAudioSourcePlayer::~HostSyncAudioSourcePlayer()
{
}

//==============================================================================
void HostSyncAudioSourcePlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    sampleRateToPlay = sampleRate;

    // Run offline resample process...
    doResamplingIfNeeded();
}

void HostSyncAudioSourcePlayer::releaseResources()
{
}

void HostSyncAudioSourcePlayer::processBlockWithPositionInfo(juce::AudioBuffer<float>& audioBuffer, juce::MidiBuffer& midiBuffer, const juce::AudioPlayHead::PositionInfo& positionInfo)
{
    const juce::SpinLock::ScopedTryLockType lock(mutex);
    if (!lock.isLocked())
    {
        return;
    }

    if (positionInfo.getIsPlaying())
    {
        const juce::int64 current_position_in_sampled = positionInfo.getTimeInSamples().orFallback(0.0);

        if (current_position_in_sampled != estimatedNextReadSamplePosition)
        {
            memoryAudioSourceResampled->setNextReadPosition(current_position_in_sampled);
        }

        juce::AudioSourceChannelInfo audio_source_retriever(audioBuffer);
        memoryAudioSourceResampled->getNextAudioBlock(audio_source_retriever);

        estimatedNextReadSamplePosition = memoryAudioSourceResampled->getNextReadPosition();
    }
}

//==============================================================================
void HostSyncAudioSourcePlayer::setAudioBufferToPlay(const juce::AudioBuffer<float>& sourceAudioBuffer, double sourceSampleRate)
{
    const juce::SpinLock::ScopedLockType lock(mutex);

    sourceAudioBufferCache.makeCopyOf(sourceAudioBuffer);

    memoryAudioSourceOriginal = std::make_unique<juce::MemoryAudioSource>(sourceAudioBufferCache, true, false);
    sampleRateAudioSourceOriginal = sourceSampleRate;

    makeAudioSourceResampled();
}

//==============================================================================
void HostSyncAudioSourcePlayer::doResamplingIfNeeded()
{
    if (sampleRateAudioSourceOriginal == sampleRateAudioSourceResampled)
    {
        return;
    }

    makeAudioSourceResampled();
}

void HostSyncAudioSourcePlayer::makeAudioSourceResampled()
{
    if (sampleRateToPlay > 0.0)
    {
        // Run automatic resample process...

        juce::AudioBuffer<float> audio_buffer_resampled;
        // Offline Resampling
        {
            juce::AudioBuffer<float> audio_buffer_resample_tmp;

            const double ratio = sampleRateAudioSourceOriginal / sampleRateToPlay;

            audio_buffer_resample_tmp.setSize(sourceAudioBufferCache.getNumChannels(), sourceAudioBufferCache.getNumSamples() / ratio);
            audio_buffer_resample_tmp.clear();

            for (int ch_idx = 0; ch_idx < sourceAudioBufferCache.getNumChannels(); ch_idx++)
            {
                juce::WindowedSincInterpolator resampler;
                resampler.process(ratio, sourceAudioBufferCache.getReadPointer(ch_idx), audio_buffer_resample_tmp.getWritePointer(ch_idx), audio_buffer_resample_tmp.getNumSamples());
            }

            audio_buffer_resampled.makeCopyOf(audio_buffer_resample_tmp);
        }

        memoryAudioSourceResampled = std::make_unique<juce::MemoryAudioSource>(audio_buffer_resampled, true, false);
        sampleRateAudioSourceResampled = sampleRateToPlay;
    }
    else
    {
        memoryAudioSourceResampled = nullptr;
        sampleRateAudioSourceResampled = 0.0;
    }
}

//==============================================================================
double HostSyncAudioSourcePlayer::getTimeLengthInSeconds() const
{
    if (memoryAudioSourceOriginal.get() != nullptr)
    {
        return memoryAudioSourceOriginal->getTotalLength() * sampleRateAudioSourceOriginal;
    }

    return 0.0;
}

}
