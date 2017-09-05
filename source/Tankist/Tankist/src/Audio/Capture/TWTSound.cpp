#include "TWTSound.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundMgr::SoundMgr()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SoundMgr::Construct()
{
    WAVEFORMATEX format;

    if (FAILED(XAudio2Create(&xaudioObject)))
    {
        return false;
    }

    if (FAILED(xaudioObject->CreateMasteringVoice(&masteringVoice, 2, 16000)))
    {
        xaudioObject->Release();
        return false;
    }

    voiceCallback.soundMgr = this;

    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = 16000;
    format.nAvgBytesPerSec = 16000 * 4;
    format.nBlockAlign = 4;
    format.wBitsPerSample = 16;
    format.cbSize = 0;

    if (FAILED(xaudioObject->CreateSourceVoice(&sourceVoice, &format, XAUDIO2_VOICE_NOPITCH, 1.0f, &voiceCallback)))
    {
        masteringVoice->DestroyVoice();
        xaudioObject->Release();
        return false;
    }

    playBuffer[0] = new OutputSample(kOutputBufferFrameCount * 4);
    playBuffer[1] = playBuffer[0] + kOutputBufferFrameCount * 2;
    memset(playBuffer[0], 0, kOutputBufferFrameCount * 4 * sizeof(OutputSample));

    soundSignal = new Signal(3);

    sourceBuffer[0].Flags = 0;
    sourceBuffer[0].AudioBytes = 4096;
    sourceBuffer[0].pAudioData = reinterpret_cast<BYTE*>(playBuffer[0]);
    sourceBuffer[0].PlayBegin = 0;
    sourceBuffer[0].PlayLength = 0;
    sourceBuffer[0].LoopBegin = XAUDIO2_NO_LOOP_REGION;
    sourceBuffer[0].LoopLength = 0;
    sourceBuffer[0].LoopCount = 0;
    sourceBuffer[0].pContext = &sourceBuffer[0];

    sourceBuffer[1].Flags = 0;
//    sourceBuffer[1].AudioBytes = kStereoOutputBufferSize;
    sourceBuffer[1].pAudioData = reinterpret_cast<BYTE *>(playBuffer[1]);
    sourceBuffer[1].PlayBegin = 0;
    sourceBuffer[1].PlayLength = 0;
    sourceBuffer[1].LoopBegin = XAUDIO2_NO_LOOP_REGION;
    sourceBuffer[1].LoopLength = 0;
    sourceBuffer[1].LoopCount = 0;
    sourceBuffer[1].pContext = &sourceBuffer[1];

    sourceVoice->SubmitSourceBuffer(&sourceBuffer[0]);
    sourceVoice->SubmitSourceBuffer(&sourceBuffer[1]);
    sourceVoice->Start(0);

    return false;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SoundMgr::VoiceCallback::OnVoiceProcessingPassStart(UINT32) {}

void SoundMgr::VoiceCallback::OnVoiceProcessingPassEnd() {}

void SoundMgr::VoiceCallback::OnStreamEnd() {}

void SoundMgr::VoiceCallback::OnBufferStart(void *context) {}

void SoundMgr::VoiceCallback::OnBufferEnd(void *context)
{
    soundMgr->soundSignal->Trigger(context == &soundMgr->sourceBuffer[0] ? 1 : 2);
}

void SoundMgr::VoiceCallback::OnLoopEnd(void *context) {}

void SoundMgr::VoiceCallback::OnVoiceError(void *context, HRESULT) {}
