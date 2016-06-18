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
    void CreateEncodeDecode();

    struct OpusEncoder *enc = nullptr;
    struct OpusDecoder *dec = nullptr;
};
