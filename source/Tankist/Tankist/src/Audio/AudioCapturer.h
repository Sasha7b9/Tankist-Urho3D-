#pragma once


class AudioCapturer
{
public:
    AudioCapturer();

    bool Start();
    void Pause(bool pause);
    bool InPause();
    void Stop();
    void PlayData(void *buffer, uint length);

    void *OPUS_Decode(void *buffer, int *sizeInOut);

private:
    bool Init();
    bool pause = true;
};
