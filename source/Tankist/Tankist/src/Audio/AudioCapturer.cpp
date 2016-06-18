#include "stdafx.h"


#include "AudioCapturer.h"
#include "bass.h"


#pragma comment(lib, "bass.lib")


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
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool AudioCapturer::Start()
{
    int device = BASS_RecordGetDevice();

    LOG_INFOF("device bass = %d", device);

    return false;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void AudioCapturer::Stop()
{

}
