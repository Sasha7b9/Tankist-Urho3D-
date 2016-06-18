#pragma once


class AudioCapturer
{
public:
    AudioCapturer();

    bool Start();
    void Stop();
    void PlayData(void *buffer, uint length);

    void *OPUS_Decode(void *buffer, int *sizeInOut);

private:
    bool Init();
};
