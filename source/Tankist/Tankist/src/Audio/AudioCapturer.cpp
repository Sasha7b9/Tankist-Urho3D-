#include "stdafx.h"


#include <bass.h>
#include <opus.h>
#include "AudioCapturer.h"


#pragma comment(lib, "bass.lib")
#pragma comment(lib, "opus.lib")
#pragma comment(lib, "silk_common.lib")
#pragma comment(lib, "silk_float.lib")
#pragma comment(lib, "celt.lib")


#define SAMPLERATE 8000
//#define ADJUSTRATE      // Adjust the output rate (in case input and output devices are going at slightly different speeds)

static HSTREAM chan = 0;
static DWORD prebuf = 0;
static DWORD targbuf = 0;
static DWORD rate = 0;
static HRECORD rchan = 0;


struct OpusEncoder *enc = nullptr;
struct OpusDecoder *dec = nullptr;

static void CreateEncodeDecode();
// Encode voice data. bufIn - input buffer, sizeInOut - length input/output buffer. Return output buffer. If data not ready, return 0.
static void* OPUS_Encode(void *bufIn, int *sizeInOut);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DisplayDeviceInfo(BASS_DEVICEINFO *di)
{
    LOG_INFOF("%s\n\tdriver: %s\n\ttype: ", di->name, di->driver);
    switch(di->flags&BASS_DEVICE_TYPE_MASK)
    {
        case BASS_DEVICE_TYPE_NETWORK:
            LOG_INFO("Remote Network");
            break;
        case BASS_DEVICE_TYPE_SPEAKERS:
            LOG_INFO("Speakers");
            break;
        case BASS_DEVICE_TYPE_LINE:
            LOG_INFO("Line");
            break;
        case BASS_DEVICE_TYPE_HEADPHONES:
            LOG_INFO("Headphones");
            break;
        case BASS_DEVICE_TYPE_MICROPHONE:
            LOG_INFO("Microphone");
            break;
        case BASS_DEVICE_TYPE_HEADSET:
            LOG_INFO("Headset");
            break;
        case BASS_DEVICE_TYPE_HANDSET:
            LOG_INFO("Handset");
            break;
        case BASS_DEVICE_TYPE_DIGITAL:
            LOG_INFO("Digital");
            break;
        case BASS_DEVICE_TYPE_SPDIF:
            LOG_INFO("SPDIF");
            break;
        case BASS_DEVICE_TYPE_HDMI:
            LOG_INFO("HDMI");
            break;
        case BASS_DEVICE_TYPE_DISPLAYPORT:
            LOG_INFO("DisplayPort");
            break;
        default:
            LOG_INFO("Unknown");
    }
    LOG_INFO("\n\tflags:");
    if(di->flags&BASS_DEVICE_ENABLED) LOG_INFO(" enabled");
    if(di->flags&BASS_DEVICE_DEFAULT) LOG_INFO(" default");
    LOG_INFOF(" (%x)\n", di->flags);
}


AudioCapturer::AudioCapturer()
{
    /*
    BASS_DEVICEINFO di;
    int a;
    LOG_INFO("Output Devices\n");
    for(a = 1; BASS_GetDeviceInfo(a, &di); a++)
    {
        LOG_INFOF("%d: ", a);
        DisplayDeviceInfo(&di);
    }
    LOG_INFO("\nInput Devices\n");
    for(a = 0; BASS_RecordGetDeviceInfo(a, &di); a++)
    {
        LOG_INFOF("%d: ", a);
        DisplayDeviceInfo(&di);
    }
    */

    if(HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
        LOG_ERRORF("An incorrect version of bass.dll. Required %d, available %d", BASSVERSION, BASS_GetVersion());
    }

    CreateEncodeDecode();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncodeDecode()
{
    int error = 0;

    enc = opus_encoder_create(8000, 2, OPUS_APPLICATION_AUDIO, &error);

    opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(10));
    opus_encoder_ctl(enc, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));

    dec = opus_decoder_create(8000, 2, &error);

    opus_decoder_ctl(dec, OPUS_SET_COMPLEXITY(10));
    opus_decoder_ctl(dec, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void* AudioCapturer::OPUS_Decode(void *bufIn, int *sizeInOut)
{
    static uint8 buffer[10000];
    int numBytes = opus_decode(dec, (uint8*)bufIn, *sizeInOut, (opus_int16*)buffer, 320, 0);
    
    *sizeInOut = numBytes;
    return buffer;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void* OPUS_Encode(void *buffIn, int *sizeInOut)
{
    static const int SIZE_BUFFER = 160;

    static uint8 bufferIn[10000];
    static uint8 bufferOut[10000];

    static int bytesInInput = 0;
    uint8 *pointerIn = bufferIn;

    memcpy(bufferIn + bytesInInput, buffIn, (size_t)*sizeInOut);
    bytesInInput += *sizeInOut;

    int encodeBytes = 0;

    while(bytesInInput >= (SIZE_BUFFER * 4))
    {
        encodeBytes += opus_encode(enc, (opus_int16*)pointerIn, SIZE_BUFFER, bufferOut + encodeBytes, 5000);
        pointerIn += SIZE_BUFFER * 4;
        bytesInInput -= SIZE_BUFFER * 4;
    }
    
    if(encodeBytes <= 0)
    {
        return 0;
    }

    if(bytesInInput)
    {
        memcpy(bufferIn, pointerIn, (size_t)bytesInInput);
    }

    *sizeInOut = encodeBytes;

    return bufferOut;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static BOOL CALLBACK RecordingCallback(HRECORD /*handle*/, const void *buffer, DWORD length, void * /*user*/)
{
    static int counter = 0;
    static int compressedBytes = 0;
    static float prevTime = 0.0f;
    static uint maxSize = 0;
    static uint minSize = 0xffffffff;
    static uint allBytes = 0;
    static float timeRun = 0.0f;

    float timeEnter = gTime->GetElapsedTime();

    counter++;
    allBytes += length;
    if(length > maxSize)
    {   
        maxSize = length;
    }
    else if(length < minSize)
    {
        minSize = length;
    }

    if((gTime->GetElapsedTime() - prevTime) >= 1.0f)
    {
        prevTime = gTime->GetElapsedTime();
        LOG_INFOF("counter = %d, max size = %d, min size = %d, bytes - all/compressed = %d/%d = %f, timeRun = %f s", counter, maxSize, minSize, allBytes, compressedBytes, (float)allBytes / compressedBytes, timeRun);
        counter = 0;
        maxSize = 0;
        minSize = 0xffffffff;
        allBytes = 0;
        compressedBytes = 0;
    }

    if(gChat)
    {
        uint8 *pointer = (uint8*)buffer;

        do
        {
            uint sendBytes = length;
            if(sendBytes > 1000)
            {
                sendBytes = 1000;
            }

            int numBytes = (int)sendBytes;

            void* data = OPUS_Encode(pointer, &numBytes);

            if(data != 0)
            {
                compressedBytes += numBytes;
                gChat->SendAudioData(data, (uint)numBytes);
            }

            pointer += sendBytes;
            length -= sendBytes;

        } while(length);
    }

    timeRun += (gTime->GetElapsedTime() - timeEnter);

    return true;

    /*
    DWORD bl;

    BASS_StreamPutData(chan, buffer, length);                   // Feed recorded data to output stream

    bl = BASS_ChannelGetData(chan, NULL, BASS_DATA_AVAILABLE);  // Get output buffer level

    if(prebuf)                              // Prebuffering
    {
        if(bl >= prebuf + length)           // Gone 1 block past the prebuffering target
        {
            prebuf = 0;                     // Finished prebuffering
            BASS_ChannelPlay(chan, FALSE);  // Start the output
        }
    }
    return true;
    */
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void AudioCapturer::PlayData(void *buffer, uint length)
{
    DWORD bl;

    BASS_StreamPutData(chan, buffer, length);

    bl = BASS_ChannelGetData(chan, NULL, BASS_DATA_AVAILABLE);

    if(prebuf)
    {
        if(bl >= prebuf + length)
        {
            prebuf = 0;
            BASS_ChannelPlay(chan, FALSE);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool AudioCapturer::Start()
{
    Init();

    if(gTypeApplication == Type_Client)
    {
        if(!BASS_RecordInit(-1))
        {
            LOG_ERRORF("Can't initialize recording with error %d", BASS_ErrorGetCode());
            BASS_RecordFree();
            BASS_Free();
            return false;
        }

        rchan = BASS_RecordStart(SAMPLERATE, 2, MAKELONG(0, 10), RecordingCallback, 0);

        if(rchan == 0)
        {
            LOG_ERRORF("Can't initialize recording with error %d", BASS_ErrorGetCode());
            BASS_RecordFree();
            BASS_Free();
            return false;
        }
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool AudioCapturer::Init()
{
    BASS_INFO bi;

    BASS_SetConfig(BASS_CONFIG_VISTA_TRUEPOS, 0);       // Allosw lower latency on Vista and newer

    if(!BASS_Init(-1, SAMPLERATE, BASS_DEVICE_LATENCY, 0, NULL))
    {
        LOG_ERROR("Can't initialize audio output");
        return false;
    }

    BASS_GetInfo(&bi);

    chan = BASS_StreamCreate(SAMPLERATE, 2, 0, STREAMPROC_PUSH, 0);

    if(chan == 0)
    {
        LOG_ERRORF("Can't create stream with error code %d", BASS_ErrorGetCode());
        return false;
    }

    rate = SAMPLERATE;

    prebuf = (DWORD)BASS_ChannelSeconds2Bytes(chan, bi.minbuf / 1000.0f);  // prebuffer at least "minbuf" worth of data

    if(prebuf == -1)
    {
        LOG_ERRORF("Error %d", BASS_ErrorGetCode());
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void AudioCapturer::Stop()
{
    BASS_RecordFree();
    BASS_Free();
}
