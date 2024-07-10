#pragma once

namespace cctn
{

//==============================================================================
class VoicevoxEngineTask
    : public juce::ThreadPoolJob
{
public:
    //==============================================================================
    explicit VoicevoxEngineTask(std::shared_ptr<voicevox::VoicevoxClient> client_, VoicevoxEngineRequest request_, std::function<void(const VoicevoxEngineArtefact&)> callback_);
    ~VoicevoxEngineTask() override;
    
    //==============================================================================
    virtual juce::ThreadPoolJob::JobStatus runJob() override;

private:
    //==============================================================================
    std::weak_ptr<voicevox::VoicevoxClient> clientPtr;
    VoicevoxEngineRequest request;
    std::function<void(const VoicevoxEngineArtefact&)> callbackIfComplete;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoicevoxEngineTask)
};

}
