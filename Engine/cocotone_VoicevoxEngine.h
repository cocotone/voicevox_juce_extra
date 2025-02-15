#pragma once

namespace cctn
{

enum VoicevoxEngineProcessType
{
    kTalk = 0,
    kHumming,
};

struct AudioBufferInfo
{
    juce::AudioBuffer<float> audioBuffer{};
    double sampleRate{ 0 };

    JUCE_LEAK_DETECTOR(AudioBufferInfo)
};

//==============================================================================
struct VoicevoxEngineLowLevelScore
{
    std::vector<std::int64_t> _note_consonant_vector{};
    std::vector<std::int64_t> _note_vowel_vector{};
    std::vector<std::int64_t> _note_length_vector{};
    std::vector<std::int64_t> _phonemes{};
    std::vector<std::int64_t> _phoneme_key_vector{};
    std::vector<std::int64_t> _phoneme_length_vector{};

    std::vector<float> f0Vector{};
    std::vector<std::int64_t> phonemeVector{};
    std::vector<float> volumeVector{};

    JUCE_LEAK_DETECTOR(VoicevoxEngineLowLevelScore)
};

//==============================================================================
struct VoicevoxEngineArtefact
{
    juce::Uuid requestId;
    std::optional<juce::MemoryBlock> wavBinary;
    std::optional<cctn::AudioBufferInfo> audioBufferInfo;

    JUCE_LEAK_DETECTOR(VoicevoxEngineArtefact)
};

//==============================================================================
struct VoicevoxEngineRequest
{
    juce::Uuid requestId;
    juce::uint32 speakerId { 0 };
    juce::String text{ "" };
    juce::String audioQuery{ "" };
    juce::String scoreJson{ "" };
    juce::uint32 sampleRate{ 0 };
    cctn::VoicevoxEngineProcessType processType{ VoicevoxEngineProcessType::kTalk };

    JUCE_LEAK_DETECTOR(VoicevoxEngineRequest)
};

//==============================================================================
class VoicevoxEngineTaskObserver
{
public:
    VoicevoxEngineTaskObserver() = default;
    virtual ~VoicevoxEngineTaskObserver() = default;

    virtual void onTaskCompleted()
    {
        juce::Logger::outputDebugString("VoicevoxEngineTaskObserver::onTaskCompleted");
    };

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoicevoxEngineTaskObserver)
};

//==============================================================================
class VoicevoxEngine final
{
public:
    //==============================================================================
    VoicevoxEngine();
    ~VoicevoxEngine();

    //==============================================================================
    void initialize();
    void shutdown();

    void start();
    void stop();

    //==============================================================================
    juce::var getMetaJson() const;
    
    juce::StringArray getTalkSpeakerIdentifierList() const;
    juce::StringArray getHummingSpeakerIdentifierList() const;
    std::map<juce::String, juce::uint32> getSpeakerIdentifierToSpeakerIdMap() const;

    //==============================================================================
    VoicevoxEngineArtefact requestSync(const VoicevoxEngineRequest& request);
    void requestAsync(const VoicevoxEngineRequest& request, std::function<void(const VoicevoxEngineArtefact&)> callback);

private:
    //==============================================================================
    std::shared_ptr<voicevox::VoicevoxClient> voicevoxClient;
    std::unique_ptr<juce::ThreadPool> taskThread;

    std::shared_ptr<VoicevoxEngineTaskObserver> taskObserver;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoicevoxEngine)
};

}
