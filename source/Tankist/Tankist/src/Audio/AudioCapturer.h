#pragma once


class AudioCapturer
{
public:
    AudioCapturer();

    bool Start();
    void Stop();
    void PlayData(void *buffer, uint length);

private:
    bool Init();
};
