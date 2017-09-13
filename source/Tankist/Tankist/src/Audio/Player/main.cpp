/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <fstream>
#include <iostream>
#include <dsound.h>


#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")


#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


typedef struct WaveHeader
{
    char  RiffSig[4];        // 'RIFF'
    int  WaveformChunkSize;
    char  WaveSig[4];        // 'WAVE'
    char  FormatSig[4];      // 'fmt ' (обратите вниманиена завершающий пробел)
    long  FormatChunkSize;   // 16
    short FormatTag;         // WAVE_FORMAT_PCM
    short Channels;          // количество каналов
    long  SampleRate;        // частота выборки
    long  BytesPerSec;       // байт на секунду
    short BlockAlign;        // выравнивание блока выборки
    short BitsPerSample;     // бит в выборке
    char  DataSig[4];        // 'data'
    long  DataSize;          // размер волновых данных
 } WaveHeader;


#define NUM_PLAY_NOTIFICATIONS  16
#define NUM_BUFFERS             16


struct DSOUND_STATE
{
    // DirectSound device guids from XInput API.
    // These are different for each controller and are used
    // to create the IDirectSound and IDirectSoundCapture devices
    GUID dsRenderGuid;
    GUID dsCaptureGuid;

    // DirectSound buffers for capture and playback
    LPDIRECTSOUND8 pDS;
    LPDIRECTSOUNDCAPTURE8 pDSCapture;
    LPDIRECTSOUNDBUFFER pDSBPrimary;
    LPDIRECTSOUNDBUFFER pDSBOutput;
    LPDIRECTSOUNDCAPTUREBUFFER pDSBCapture;
    LPDIRECTSOUNDNOTIFY8 pDSNotify;

    // DirectSound state
    DSBPOSITIONNOTIFY aPosNotify[NUM_PLAY_NOTIFICATIONS];
    HANDLE hNotificationEvent;
    DWORD dwOutputBufferSize;
    DWORD dwCaptureBufferSize;
    DWORD dwNextOutputOffset;
    DWORD dwNextCaptureOffset;
    DWORD dwNotifySize;
};

DSOUND_STATE state;
DSOUND_STATE* pState = &state;

WaveHeader waveHeader;

IDirectSound8 *pDS;
IDirectSoundBuffer8 *pDSBuffer;
IDirectSoundNotify8 *pDSNotify;

FILE file;

//---------------------------------------------------------------------------------------------------------------------------------------------------
void OutString(char *name, int offset, int numBytes);
void OutNumber(char *name, int offset, int numBytes);
HRESULT InitDirectSound();
void PlayStreamSound(char *name);


//---------------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
    std::ifstream file;

    file.open("chinook.wav", std::ifstream::in, std::ifstream::binary);

    if (!file)
    {
        std::cout << "ERROR!!! can not open file" << std::endl;
    }

    file.read((char*)&waveHeader, sizeof(WaveHeader));

    OutString("chunkId", 0, 4);
    OutNumber("chunkSize", 4, 4);
    OutString("format", 8, 4);
    OutString("subchunk1Id", 12, 4);
    OutNumber("subchunk1Size", 16, 4);
    OutNumber("audioFormat", 20, 2);
    OutNumber("numChannels", 22, 2);
    OutNumber("sampleRate", 24, 4);
    OutNumber("byteRange", 28, 4);
    OutNumber("blockAlign", 32, 2);
    OutNumber("bitsPerSample", 34, 2);
    OutString("subchunk2Id", 36, 4);
    OutNumber("subchunk2Size", 40, 4);

    std::cout << waveHeader.WaveformChunkSize - sizeof(waveHeader) - waveHeader.DataSize << std::endl;
    std::cout << sizeof(WaveHeader) << std::endl;

    file.close();

    if (FAILED(DirectSoundCreate8(NULL, &pDS, NULL)))
    {
        std::cout << "Can not create DirectSound object" << std::endl;
        return -1;
    }

    if (FAILED(pDS->SetCooperativeLevel(GetConsoleWindow(), DSSCL_NORMAL)))
    {
        std::cout << "Can not set cooperative level" << std::endl;
        return -1;
    }

    PlayStreamSound("chinook.wav");

    while(1)
    {
    };

    return 0;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void PlayStreamSound(char *name)
{
    DSBPOSITIONNOTIFY   dspn[4];
    WaveHeader Hdr;
    long i;

    // Open the source file

    if(!fopen_s(&pFile, Filename, "rb"))
        return;

    // Create a 2 second buffer to stream in wave
    if((g_pDSBuffer = CreateBufferFromWAV(pFile, &Hdr)) == NULL)
    {
        fclose(pFile);
        return;
    }

    // Get streaming size and pointers
    g_Size = Hdr.DataSize;
    g_Pos = sizeof(sWaveHeader);
    g_Left = g_Size;

    // Create a thread for notifications
    if((g_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, NULL, 0, &g_ThreadID)) == NULL)
        return;

    // Create the notification interface
    if(FAILED(g_pDSBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&g_pDSNotify)))
        return;

    // Create the event handles and set the notifications
    for(i = 0; i < 4; i++)
    {
        g_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        dspn[i].dwOffset = 16384 * (i + 1) - 1;
        dspn[i].hEventNotify = g_hEvents[i];
    }
    g_pDSNotify->SetNotificationPositions(4, dspn);

    // Fill buffer completely with sound
    fseek(pFile, sizeof(sWaveHeader), SEEK_SET);
    LoadSoundData(g_pDSBuffer, 0, pFile, 65536);
    g_Left -= 65536;
    g_Pos += 65536;

    // Play sound looping
    g_pDSBuffer->SetCurrentPosition(0);
    g_pDSBuffer->SetVolume(DSBVOLUME_MAX);
    g_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

    return;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT InitDirectSound()
{
    HRESULT hr;

    pState->hNotificationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    ZeroMemory(&pState->aPosNotify, sizeof(DSBPOSITIONNOTIFY) * (NUM_PLAY_NOTIFICATIONS));
    pState->dwOutputBufferSize = 0;
    pState->dwCaptureBufferSize = 0;
    pState->dwNotifySize = 0;
    pState->dwNextOutputOffset = 0;

    // Create IDirectSound8 using the controller's render device
    if (FAILED(hr = DirectSoundCreate8(&pState->dsRenderGuid, &pState->pDS, NULL)))
        return hr;

    // Set coop level to DSSCL_PRIORITY
    if (FAILED(hr = pState->pDS->SetCooperativeLevel(0, DSSCL_NORMAL)))
        return hr;

    // Create IDirectSoundCapture using the controller's capture device
    if (FAILED(hr = DirectSoundCaptureCreate8(&pState->dsCaptureGuid, &pState->pDSCapture, NULL)))
        return hr;

    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nSamplesPerSec = 22050;
    wfx.wBitsPerSample = 16;
    wfx.nChannels = 1;
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    // Set the notification size
    pState->dwNotifySize = max(4096, wfx.nAvgBytesPerSec / 8);
    pState->dwNotifySize -= pState->dwNotifySize % wfx.nBlockAlign;

    // Set the buffer sizes 
    pState->dwOutputBufferSize = NUM_BUFFERS * pState->dwNotifySize / 2;
    pState->dwCaptureBufferSize = NUM_BUFFERS * pState->dwNotifySize;

    SAFE_RELEASE(pState->pDSBCapture);

    // Create the capture buffer
    DSCBUFFERDESC dscbd;
    ZeroMemory(&dscbd, sizeof(dscbd));
    dscbd.dwSize = sizeof(dscbd);
    dscbd.dwBufferBytes = pState->dwCaptureBufferSize;
    dscbd.lpwfxFormat = &wfx; // Set the format during creatation

    if (FAILED(hr = pState->pDSCapture->CreateCaptureBuffer(&dscbd,
        &pState->pDSBCapture,
        NULL)))
        return hr;

    // Create the direct sound buffer using the same format as the 
    // capture buffer. 
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_GLOBALFOCUS;
    dsbd.dwBufferBytes = pState->dwOutputBufferSize;
    dsbd.guid3DAlgorithm = GUID_NULL;
    dsbd.lpwfxFormat = &wfx;

    // Create the DirectSound buffer 
    if (FAILED(hr = pState->pDS->CreateSoundBuffer(&dsbd, &pState->pDSBOutput, NULL)))
        return hr;

    // Create a notification event, for when the sound stops playing
    if (FAILED(hr = pState->pDSBCapture->QueryInterface(IID_IDirectSoundNotify,
        (VOID**)&pState->pDSNotify)))
        return hr;

    // Setup the notification positions
    for (INT i = 0; i < NUM_PLAY_NOTIFICATIONS; i++)
    {
        pState->aPosNotify[i].dwOffset = (pState->dwNotifySize * i) + pState->dwNotifySize - 1;
        pState->aPosNotify[i].hEventNotify = pState->hNotificationEvent;
    }

    // Tell DirectSound when to notify us. the notification will come in the from 
    // of signaled events that are handled in WinMain()
    if (FAILED(hr = pState->pDSNotify->SetNotificationPositions(NUM_PLAY_NOTIFICATIONS,
        pState->aPosNotify)))
        return hr;

    return S_OK;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void OutString(char *name, int offset, int numBytes)
{
    char *bytes = (char*)&waveHeader + offset;
    std::cout << name;
    std::cout << " - ";
    for (int i = 0; i < numBytes; i++)
    {
        std::cout << *bytes;
        bytes++;
    }
    std::cout << std::endl;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void OutNumber(char *name, int offset, int numBytes)
{
    char *bytes = (char*)&waveHeader;
    bytes += offset;
    std::cout << name;
    std::cout << " - ";
    int value = 0;
    for (int i = 0; i < numBytes; i++)
    {
        value += *((unsigned char*)bytes) << (i * 8);
        bytes++;
    }
    std::cout << value << std::endl;
}
