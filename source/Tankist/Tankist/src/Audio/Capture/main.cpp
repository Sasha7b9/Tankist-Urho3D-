
#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <iostream>


typedef short Sample;

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")


enum
{
    //kCaptureBufferFrameCount = 1024,
    kCaptureBufferFrameCount = 16,
    kCaptureBufferSize = kCaptureBufferFrameCount * sizeof(Sample)
};

enum
{
    kSoundInputSampleRate = 8000,
    //kAudioCaptureThreshold = 2048
    kAudioCaptureThreshold = 64
};

#define RETURN_ERROR    \
    std::cout << "Can not create direct sound capture" << std::endl;   \
    return -1;

#define NUM_PLAY_NOTIFICATIONS  16
#define NUM_BUFFERS             16

struct CONTROLER_STATE
{
    bool bConnected;
    
    GUID dsRenderGuid;
    GUID dsCaptureGuid;

    LPDIRECTSOUND8 pDS;
    LPDIRECTSOUNDCAPTURE8 pDSCapture;
    LPDIRECTSOUNDBUFFER pDSBPrimary;
    LPDIRECTSOUNDBUFFER pDSBOutput;
    LPDIRECTSOUNDCAPTUREBUFFER pDSBCapture;
    LPDIRECTSOUNDNOTIFY8 pDSNotify;

    DSBPOSITIONNOTIFY aPosNotify[NUM_PLAY_NOTIFICATIONS];
    HANDLE hNotificationEvent;
    DWORD dwOutputBufferSize;
    DWORD dwCaptureBufferSize;
    DWORD dwNextOutputOffset;
    DWORD dwNextCaptureOffset;
    DWORD dwNotifySize;
};

CONTROLER_STATE controlerState;
CONTROLER_STATE* pState = &controlerState;

static DWORD startTime = 0;
static bool first = true;


//---------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT InitDirectSound();
HRESULT StartBuffers();
HRESULT StopBuffers();
HRESULT RestoreBuffer(LPDIRECTSOUNDBUFFER pDSBuffer, BOOL *pbRestored);
HRESULT HandleNotification();
HRESULT FreeDirectSound();


//---------------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
    if(FAILED(InitDirectSound()))
    {
        return -1;
    }

    if(FAILED(StartBuffers()))
    {
        return -1;
    };

    HANDLE hNotificationArray = controlerState.hNotificationEvent;

    bool running = true;

    //DWORD startTime = GetTickCount();

    while(running)
    {
        DWORD result = MsgWaitForMultipleObjects(1, &hNotificationArray, FALSE, INFINITE, QS_ALLEVENTS);
        switch(result)
        {
            case WAIT_OBJECT_0:
            {
                // g_hNotificationEvent is signaled
                // This means that DirectSound just finished playing a piece of the buffer, so we need to fill the 
                // circular with new sound from wav file
                if(FAILED(HandleNotification()))
                {
                    running = false;
                }
                //static int counter = 0;
                //std::cout << "WAIT_OBJECT_0   " << counter++ << "    " << (GetTickCount() - startTime) / 1000 << "    " << (GetTickCount() - startTime) / counter << std::endl;
                break;
            }
            case WAIT_TIMEOUT:
                std::cout << "WAIT_TIMEOUT" << std::endl;
                break;
            default:
                break;
                DWORD resultCopy = result;
                break;
        }
    }

    FreeDirectSound();

    return 0;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT InitDirectSound()
{
    pState->hNotificationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    ZeroMemory(&pState->aPosNotify, sizeof(DSBPOSITIONNOTIFY) * (NUM_PLAY_NOTIFICATIONS));
    pState->dwOutputBufferSize = 0;
    pState->dwCaptureBufferSize = 0;
    pState->dwNotifySize = 0;
    pState->dwNextCaptureOffset = 0;

    if(DirectSoundCreate8(&pState->dsRenderGuid, &pState->pDS, NULL) != DS_OK)
    {
        RETURN_ERROR;
    }

    if(FAILED(pState->pDS->SetCooperativeLevel(GetConsoleWindow(), DSSCL_NORMAL)))
    {
        std::cout << "Can not set cooperative level" << std::endl;
        return -1;
    }

    if(DirectSoundCaptureCreate8(&pState->dsCaptureGuid, &pState->pDSCapture, NULL) != DS_OK)
    {
        RETURN_ERROR;
    }

    WAVEFORMATEX        format;
    ZeroMemory(&format, sizeof(format));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nSamplesPerSec = kSoundInputSampleRate;
    format.wBitsPerSample = 16;
    format.nChannels = 1;
    format.nBlockAlign = format.nChannels * (format.wBitsPerSample / 8);
    format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;

    pState->dwNotifySize = max(4096, format.nAvgBytesPerSec / 8);
    pState->dwNotifySize -= pState->dwNotifySize % format.nBlockAlign;

    pState->dwOutputBufferSize = NUM_BUFFERS * pState->dwNotifySize / 2;
    pState->dwCaptureBufferSize = NUM_BUFFERS * pState->dwNotifySize;

    SAFE_RELEASE(pState->pDSBCapture);

    DSCBUFFERDESC        desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwBufferBytes = pState->dwCaptureBufferSize;
    desc.lpwfxFormat = &format;

    HRESULT hr = DS_OK;

    hr = pState->pDSCapture->CreateCaptureBuffer(&desc, &pState->pDSBCapture, NULL);
    if(hr != DS_OK)
    {
        RETURN_ERROR;
    }

    // Create the direct sound buffer using the same format as the capture buffer
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(dsbd));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_GLOBALFOCUS;
    dsbd.dwBufferBytes = pState->dwOutputBufferSize;
    dsbd.guid3DAlgorithm = GUID_NULL;
    dsbd.lpwfxFormat = &format;

    // Create the DirectSound buffer
    if(FAILED(hr = pState->pDS->CreateSoundBuffer(&dsbd, &pState->pDSBOutput, NULL)))
    {
        RETURN_ERROR;
    }

    // Create a notification event, for when the sound stops playing
    if(FAILED(pState->pDSBCapture->QueryInterface(IID_IDirectSoundNotify, (void**)&pState->pDSNotify)))
    {
        RETURN_ERROR;
    }

    // Setup the notification positions
    for(int i = 0; i < NUM_PLAY_NOTIFICATIONS; i++)
    {
        pState->aPosNotify[i].dwOffset = (pState->dwNotifySize * i) + pState->dwNotifySize - 1;
        pState->aPosNotify[i].hEventNotify = pState->hNotificationEvent;
    }

    // Tell DirectSound when to notify us. The notification will come in the from of signaled events that are
    // handled in WinMain(0
    if(FAILED(hr = pState->pDSNotify->SetNotificationPositions(NUM_PLAY_NOTIFICATIONS, pState->aPosNotify)))
    {
        RETURN_ERROR;
    }

    return S_OK;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT StartBuffers()
{
    void* pDSLockedBuffer = NULL;
    DWORD dwDSLockedBufferSize;
    HRESULT hr;

    if(pState->pDS == NULL)
    {
        RETURN_ERROR;
    }

    // Restore lost buffers
    if(FAILED(hr = RestoreBuffer(pState->pDSBOutput, NULL)))
    {
        RETURN_ERROR;
    }

    // Reset the buffers
    pState->dwNextOutputOffset = 0;
    pState->pDSBOutput->SetCurrentPosition(0);

    // Find out where the capture buffer is right now, then write data 
    // some extra amount forward to make sure we're ahead of the write cursor
    pState->pDSBCapture->GetCurrentPosition(&pState->dwNextCaptureOffset, NULL);
    pState->dwNextCaptureOffset -= pState->dwNextCaptureOffset % pState->dwNotifySize;

    pState->dwNextOutputOffset = pState->dwNextCaptureOffset + (2 * pState->dwNotifySize);
    pState->dwNextOutputOffset %= pState->dwOutputBufferSize;   // Circular buffer

    // Tell the capture buffer to start recording
    pState->pDSBCapture->Start(DSCBSTART_LOOPING);

    if(first)
    {
        startTime = GetTickCount();
        first = false;
    }

    // Rewind the output buffer, fill it with silence, and play it
    pState->pDSBOutput->SetCurrentPosition(pState->dwNextCaptureOffset);

    // Get the format of the output buffer
    WAVEFORMATEX wfxOutput;
    ZeroMemory(&wfxOutput, sizeof(wfxOutput));
    pState->pDSBOutput->GetFormat(&wfxOutput, sizeof(wfxOutput), NULL);

    // Fill the output buffer with silence at first. As capture data arrives, HandleNotifications() will fill
    // the output buffer with wave data.
    if(FAILED(hr = pState->pDSBOutput->Lock(0, pState->dwOutputBufferSize, &pDSLockedBuffer, &dwDSLockedBufferSize, NULL, NULL, 0)))
    {
        RETURN_ERROR;
    }

    FillMemory((BYTE*)pDSLockedBuffer, dwDSLockedBufferSize, (BYTE)(wfxOutput.wBitsPerSample == 8 ? 128 : 0));
    pState->pDSBOutput->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, NULL);

    // Play the output buffer
    pState->pDSBOutput->Play(0, 0, DSBPLAY_LOOPING);

    return S_OK;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT RestoreBuffer(LPDIRECTSOUNDBUFFER pDSBuffer, BOOL *pbRestored)
{
    HRESULT hr;

    if(pbRestored != NULL)
    {
        *pbRestored = FALSE;
    }

    if(NULL == pDSBuffer)
    {
        return S_FALSE;
    }

    DWORD dwStatus;
    if(FAILED(hr = pDSBuffer->GetStatus(&dwStatus)))
    {
        return hr;
    }

    if(dwStatus & DSBSTATUS_BUFFERLOST)
    {
        // Since the app could have just been activated, then DirectSound may not be giving us control yet, so
        // the restoring the buffer may fail. If it does, sleep until DirectSound gives us control.
        do
        {
            hr = pDSBuffer->Restore();
            if(hr == DSERR_BUFFERLOST)
            {
                Sleep(10);
            }
        } while((hr = pDSBuffer->Restore()) == DSERR_BUFFERLOST);

        if(pbRestored != NULL)
        {
            *pbRestored = TRUE;
        }
        return S_OK;
    }

    return S_FALSE;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT StopBuffers()
{
    if(pState->pDSBCapture)
    {
        pState->pDSBCapture->Stop();
    }
    if(pState->pDSBOutput)
    {
        pState->pDSBOutput->Stop();
    }
    return S_OK;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
// Handle the notification that tells us to copy data from the capture buffer to the output buffer
HRESULT HandleNotification()
{
    HRESULT hr;
    VOID* pDSCaptureLockedBuffer = NULL;
    VOID* pDSOutputLockedBuffer = NULL;
    DWORD dwDSCaptureLockedBufferSize;
    DWORD dwDSOutputLockedBufferSize;

    DWORD dwStatus;

    // Make sure buffers were not lost, if the were we need 
    // to start the capture again
    if(FAILED(hr = pState->pDSBOutput->GetStatus(&dwStatus)))
        return hr;

    if(dwStatus & DSBSTATUS_BUFFERLOST)
    {
        if(FAILED(hr = StartBuffers()))
            return hr;

        return S_OK;
    }

    // Lock the capture buffer down
    if(FAILED(hr = pState->pDSBCapture->Lock(pState->dwNextCaptureOffset, pState->dwNotifySize,
        &pDSCaptureLockedBuffer,
        &dwDSCaptureLockedBufferSize,
        NULL, NULL, 0L)))
        return hr;

    // Lock the output buffer down
    if(FAILED(hr = pState->pDSBOutput->Lock(pState->dwNextOutputOffset, pState->dwNotifySize,
        &pDSOutputLockedBuffer,
        &dwDSOutputLockedBufferSize,
        NULL, NULL, 0L)))
        return hr;

    // These should be equal
    if(dwDSOutputLockedBufferSize != dwDSCaptureLockedBufferSize)
        return E_FAIL;  // Sanity check unhandled case 

    // This sample just copies the memory from the capture buffer to the playback buffer 
    // Typically the capture data would be sent across the network to another headset
    CopyMemory(pDSOutputLockedBuffer, pDSCaptureLockedBuffer, dwDSOutputLockedBufferSize);

    // Unlock the play buffer
    pState->pDSBOutput->Unlock(pDSOutputLockedBuffer, dwDSOutputLockedBufferSize,
                               NULL, 0);

    int numColumns = 15;

    int wordInCol = dwDSOutputLockedBufferSize / numColumns;
    signed short *inBuffer = (signed short*)pDSOutputLockedBuffer;

    static int numBytes = 0;

    numBytes += dwDSOutputLockedBufferSize;

    for(int col = 0; col < 1; col++)
    {
        signed int sum = 0;
        for(int word = col * numColumns; word < col * numColumns + wordInCol; word++)
        {
            sum += inBuffer[word];
        }

        int value = sum / wordInCol;

        if(value < 0)
        {
            value = -value;
        }

        DWORD time = GetTickCount() - startTime;

        if(time)
        {
            std::cout << (int)(numBytes / ((time) / 1000.0f)) << " " << (numBytes / 1024 / 1024) << " MB " << time / 1000 / 60 << " min ";
        }

        for(int i = 0; i < value + 1; i++)
        {
            std::cout << "0";
        }

        std::cout << std::endl;
    }


    // Unlock the capture buffer
    pState->pDSBCapture->Unlock(pDSCaptureLockedBuffer, dwDSCaptureLockedBufferSize,
                                NULL, 0);

    // Move the capture offset along
    pState->dwNextCaptureOffset += dwDSCaptureLockedBufferSize;
    pState->dwNextCaptureOffset %= pState->dwCaptureBufferSize; // Circular buffer

    // Move the playback offset along
    pState->dwNextOutputOffset += dwDSOutputLockedBufferSize;
    pState->dwNextOutputOffset %= pState->dwOutputBufferSize; // Circular buffer

    return S_OK;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT FreeDirectSound()
{
    SAFE_RELEASE(pState->pDSNotify);
    SAFE_RELEASE(pState->pDSBPrimary);
    SAFE_RELEASE(pState->pDSBOutput);
    SAFE_RELEASE(pState->pDSBCapture);
    SAFE_RELEASE(pState->pDSCapture);
    SAFE_RELEASE(pState->pDS);
    CloseHandle(pState->hNotificationEvent);

    return S_OK;
}
