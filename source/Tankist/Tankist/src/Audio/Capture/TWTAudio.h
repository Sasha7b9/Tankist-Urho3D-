#pragma once

#pragma pack(push, 1)

#define WAVE_FORMAT_PCM		0x0001
#define WAVE_FORMAT_ADPCM	0x0002


typedef struct
{
    WORD		wFormatTag;
    WORD		nChannels;
    DWORD		nSamplesPerSec;
    DWORD		nAvgBytesPerSec;
    WORD		nBlockAlign;
    WORD		wBitsPerSample;
    WORD		cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX;

typedef struct
{
    WAVEFORMATEX	Format;

    union
    {
        WORD	wValidBitsPerSample;
        WORD	wSamplesPerBlock;
        WORD	wReserved;
    } Samples;

    DWORD		dwChannelMask;
    GUID		SubFormat;
} WAVEFORMATEXTENSIBLE;


DEFINE_CLSID(XAudio2, 5a508685, a254, 4fba, 9b, 82, 9a, 24, b0, 03, 06, af);
DEFINE_CLSID(XAudio2_Debug, db05ea35, 0329, 4d4b, a5, 3a, 6d, ea, d0, 3d, 38, 52);
DEFINE_IID(IXAudio2, 8bcf1f58, 9fe7, 4583, 8a, c6, e2, ad, c4, 65, c8, bb);

#define XAUDIO2_NO_LOOP_REGION			0
#define XAUDIO2_COMMIT_NOW				0
#define XAUDIO2_DEBUG_ENGINE			0x0001
#define XAUDIO2_VOICE_NOPITCH			0x0002
#define XAUDIO2_DEFAULT_FREQ_RATIO		2.0F
#define XAUDIO2_DEFAULT_CHANNELS		0
#define XAUDIO2_DEFAULT_SAMPLERATE		0

typedef enum XAUDIO2_WINDOWS_PROCESSOR_SPECIFIER
{
    XAUDIO2_ANY_PROCESSOR = 0xFFFFFFFF,
    XAUDIO2_DEFAULT_PROCESSOR = XAUDIO2_ANY_PROCESSOR
} XAUDIO2_WINDOWS_PROCESSOR_SPECIFIER, XAUDIO2_PROCESSOR;

struct IXAudio2Voice;
struct IXAudio2SourceVoice;
struct IXAudio2SubmixVoice;
struct IXAudio2MasteringVoice;
struct IXAudio2EngineCallback;
struct IXAudio2VoiceCallback;

typedef enum XAUDIO2_DEVICE_ROLE
{
    NotDefaultDevice = 0x0,
    DefaultConsoleDevice = 0x1,
    DefaultMultimediaDevice = 0x2,
    DefaultCommunicationsDevice = 0x4,
    DefaultGameDevice = 0x8,
    GlobalDefaultDevice = 0xf,
    InvalidDeviceRole = ~GlobalDefaultDevice
} XAUDIO2_DEVICE_ROLE;

typedef struct XAUDIO2_DEVICE_DETAILS
{
    WCHAR					DeviceID[256];
    WCHAR					DisplayName[256];
    XAUDIO2_DEVICE_ROLE		Role;
    WAVEFORMATEXTENSIBLE	OutputFormat;
} XAUDIO2_DEVICE_DETAILS;

typedef struct XAUDIO2_SEND_DESCRIPTOR
{
    UINT32				Flags;
    IXAudio2Voice		*pOutputVoice;
} XAUDIO2_SEND_DESCRIPTOR;

typedef struct XAUDIO2_VOICE_DETAILS
{
    UINT32		CreationFlags;
    UINT32		InputChannels;
    UINT32		InputSampleRate;
} XAUDIO2_VOICE_DETAILS;

typedef struct XAUDIO2_VOICE_SENDS
{
    UINT32						SendCount;
    XAUDIO2_SEND_DESCRIPTOR		*pSends;
} XAUDIO2_VOICE_SENDS;

typedef struct XAUDIO2_EFFECT_DESCRIPTOR
{
    IUnknown	*pEffect;
    BOOL		InitialState;
    UINT32		OutputChannels;
} XAUDIO2_EFFECT_DESCRIPTOR;

typedef struct XAUDIO2_EFFECT_CHAIN
{
    UINT32						EffectCount;
    XAUDIO2_EFFECT_DESCRIPTOR	*pEffectDescriptors;
} XAUDIO2_EFFECT_CHAIN;

typedef struct XAUDIO2_VOICE_STATE
{
    void		*pCurrentBufferContext;
    UINT32		BuffersQueued;
    UINT64		SamplesPlayed;
} XAUDIO2_VOICE_STATE;

typedef struct XAUDIO2_PERFORMANCE_DATA
{
    UINT64		AudioCyclesSinceLastQuery;
    UINT64		TotalCyclesSinceLastQuery;
    UINT32		MinimumCyclesPerQuantum;
    UINT32		MaximumCyclesPerQuantum;
    UINT32		MemoryUsageInBytes;
    UINT32		CurrentLatencyInSamples;
    UINT32		GlitchesSinceEngineStarted;
    UINT32		ActiveSourceVoiceCount;
    UINT32		TotalSourceVoiceCount;
    UINT32		ActiveSubmixVoiceCount;
    UINT32		ActiveResamplerCount;
    UINT32		ActiveMatrixMixCount;
    UINT32		ActiveXmaSourceVoices;
    UINT32		ActiveXmaStreams;
} XAUDIO2_PERFORMANCE_DATA;

typedef struct XAUDIO2_DEBUG_CONFIGURATION
{
    UINT32		TraceMask;
    UINT32		BreakMask;
    BOOL		LogThreadID;
    BOOL		LogFileline;
    BOOL		LogFunctionName;
    BOOL		LogTiming;
} XAUDIO2_DEBUG_CONFIGURATION;

typedef enum XAUDIO2_FILTER_TYPE
{
    LowPassFilter,
    BandPassFilter,
    HighPassFilter,
    NotchFilter
} XAUDIO2_FILTER_TYPE;

typedef struct XAUDIO2_FILTER_PARAMETERS
{
    XAUDIO2_FILTER_TYPE		Type;
    float					Frequency;
    float					OneOverQ;
} XAUDIO2_FILTER_PARAMETERS;

typedef struct XAUDIO2_BUFFER
{
    UINT32		Flags;
    UINT32		AudioBytes;
    BYTE		*pAudioData;
    UINT32		PlayBegin;
    UINT32		PlayLength;
    UINT32		LoopBegin;
    UINT32		LoopLength;
    UINT32		LoopCount;
    void		*pContext;
} XAUDIO2_BUFFER;

typedef struct XAUDIO2_BUFFER_WMA
{
    UINT32		*pDecodedPacketCumulativeBytes;
    UINT32		PacketCount;
} XAUDIO2_BUFFER_WMA;


#undef INTERFACE
#define INTERFACE IXAudio2
DECLARE_INTERFACE_(IXAudio2, IUnknown)
{
    STDMETHOD(QueryInterface)(REFIID riid, void **ppvInterface) = 0;
    STDMETHOD_(ULONG, AddRef)(void) = 0;
    STDMETHOD_(ULONG, Release)(void) = 0;
    STDMETHOD(GetDeviceCount)(UINT32 *pCount) = 0;
    STDMETHOD(GetDeviceDetails)(UINT32 Index, XAUDIO2_DEVICE_DETAILS *pDeviceDetails) = 0;
    STDMETHOD(Initialize)(UINT32 Flags = 0, XAUDIO2_PROCESSOR XAudio2Processor = XAUDIO2_DEFAULT_PROCESSOR) = 0;
    STDMETHOD(RegisterForCallbacks)(IXAudio2EngineCallback *pCallback) = 0;
    STDMETHOD_(void, UnregisterForCallbacks)(IXAudio2EngineCallback *pCallback) = 0;
    STDMETHOD(CreateSourceVoice)(IXAudio2SourceVoice **ppSourceVoice, const WAVEFORMATEX *pSourceFormat, UINT32 Flags = 0, float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO, IXAudio2VoiceCallback *pCallback = NULL, const XAUDIO2_VOICE_SENDS *pSendList = NULL, const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL) = 0;
    STDMETHOD(CreateSubmixVoice)(IXAudio2SubmixVoice **ppSubmixVoice, UINT32 InputChannels, UINT32 InputSampleRate, UINT32 Flags = 0, UINT32 ProcessingStage = 0, const XAUDIO2_VOICE_SENDS *pSendList = NULL, const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL) = 0;
    STDMETHOD(CreateMasteringVoice)(IXAudio2MasteringVoice **ppMasteringVoice, UINT32 InputChannels = XAUDIO2_DEFAULT_CHANNELS, UINT32 InputSampleRate = XAUDIO2_DEFAULT_SAMPLERATE, UINT32 Flags = 0, UINT32 DeviceIndex = 0, const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL) = 0;
    STDMETHOD(StartEngine)(void) = 0;
    STDMETHOD_(void, StopEngine)(void) = 0;
    STDMETHOD(CommitChanges)(UINT32 OperationSet) = 0;
    STDMETHOD_(void, GetPerformanceData)(XAUDIO2_PERFORMANCE_DATA *pPerfData) = 0;
    STDMETHOD_(void, SetDebugConfiguration)(const XAUDIO2_DEBUG_CONFIGURATION *pDebugConfiguration, void *pReserved = NULL) = 0;
};

#undef INTERFACE
#define INTERFACE IXAudio2Voice
DECLARE_INTERFACE(IXAudio2Voice)
{
#define Declare_IXAudio2Voice_Methods() \
	STDMETHOD_(void, GetVoiceDetails)(XAUDIO2_VOICE_DETAILS *pVoiceDetails) = 0; \
	STDMETHOD(SetOutputVoices)(const XAUDIO2_VOICE_SENDS *pSendList) = 0; \
	STDMETHOD(SetEffectChain)(const XAUDIO2_EFFECT_CHAIN *pEffectChain) = 0; \
	STDMETHOD(EnableEffect)(UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD(DisableEffect)(UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD_(void, GetEffectState)(UINT32 EffectIndex, BOOL *pEnabled) = 0; \
	STDMETHOD(SetEffectParameters)(UINT32 EffectIndex, const void *pParameters, UINT32 ParametersByteSize, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD(GetEffectParameters)(UINT32 EffectIndex, void *pParameters, UINT32 ParametersByteSize) = 0; \
	STDMETHOD(SetFilterParameters)(const XAUDIO2_FILTER_PARAMETERS *pParameters, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD_(void, GetFilterParameters)(XAUDIO2_FILTER_PARAMETERS *pParameters) = 0; \
	STDMETHOD(SetOutputFilterParameters)(IXAudio2Voice *pDestinationVoice, const XAUDIO2_FILTER_PARAMETERS *pParameters, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD_(void, GetOutputFilterParameters)(IXAudio2Voice *pDestinationVoice, XAUDIO2_FILTER_PARAMETERS *pParameters) = 0; \
	STDMETHOD(SetVolume)(float Volume, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD_(void, GetVolume)(float *pVolume) = 0; \
	STDMETHOD(SetChannelVolumes)(UINT32 Channels, const float *pVolumes, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD_(void, GetChannelVolumes)(UINT32 Channels, float *pVolumes) = 0; \
	STDMETHOD(SetOutputMatrix)(IXAudio2Voice *pDestinationVoice, UINT32 SourceChannels, UINT32 DestinationChannels, const float *pLevelMatrix,  UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0; \
	STDMETHOD_(void, GetOutputMatrix)(IXAudio2Voice *pDestinationVoice, UINT32 SourceChannels, UINT32 DestinationChannels, float *pLevelMatrix) = 0; \
	STDMETHOD_(void, DestroyVoice)(void) = 0

    Declare_IXAudio2Voice_Methods();
};

#undef INTERFACE
#define INTERFACE IXAudio2MasteringVoice
DECLARE_INTERFACE_(IXAudio2MasteringVoice, IXAudio2Voice)
{
    Declare_IXAudio2Voice_Methods();
};

#undef INTERFACE
#define INTERFACE IXAudio2SourceVoice
DECLARE_INTERFACE_(IXAudio2SourceVoice, IXAudio2Voice)
{
    Declare_IXAudio2Voice_Methods();

    STDMETHOD(Start)(UINT32 Flags = 0, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0;
    STDMETHOD(Stop)(UINT32 Flags = 0, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0;
    STDMETHOD(SubmitSourceBuffer)(const XAUDIO2_BUFFER *pBuffer, const XAUDIO2_BUFFER_WMA *pBufferWMA = NULL) = 0;
    STDMETHOD(FlushSourceBuffers)(void) = 0;
    STDMETHOD(Discontinuity)(void) = 0;
    STDMETHOD(ExitLoop)(UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0;
    STDMETHOD_(void, GetState)(XAUDIO2_VOICE_STATE *pVoiceState) = 0;
    STDMETHOD(SetFrequencyRatio)(float Ratio, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) = 0;
    STDMETHOD_(void, GetFrequencyRatio)(float *pRatio) = 0;
    STDMETHOD(SetSourceSampleRate)(UINT32 NewSourceSampleRate) = 0;
};

#undef INTERFACE
#define INTERFACE IXAudio2VoiceCallback
DECLARE_INTERFACE(IXAudio2VoiceCallback)
{
    STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32 BytesRequired) = 0;
    STDMETHOD_(void, OnVoiceProcessingPassEnd)(void) = 0;
    STDMETHOD_(void, OnStreamEnd)(void) = 0;
    STDMETHOD_(void, OnBufferStart)(void *pBufferContext) = 0;
    STDMETHOD_(void, OnBufferEnd)(void *pBufferContext) = 0;
    STDMETHOD_(void, OnLoopEnd)(void *pBufferContext) = 0;
    STDMETHOD_(void, OnVoiceError)(void *pBufferContext, HRESULT Error) = 0;
};

inline HRESULT XAudio2Create(IXAudio2 **ppXAudio2, UINT32 Flags = 0, XAUDIO2_PROCESSOR XAudio2Processor = XAUDIO2_DEFAULT_PROCESSOR)
{
    IXAudio2 *pXAudio2;

    HRESULT hr = CoCreateInstance((Flags & XAUDIO2_DEBUG_ENGINE) ? __uuidof(XAudio2_Debug) : __uuidof(XAudio2), NULL, CLSCTX_INPROC_SERVER, __uuidof(IXAudio2), (void **)&pXAudio2);
    if (SUCCEEDED(hr))
    {
        hr = pXAudio2->Initialize(Flags, XAudio2Processor);

        if (SUCCEEDED(hr)) *ppXAudio2 = pXAudio2;
        else  pXAudio2->Release();
    }

    return (hr);
}


#pragma pack(pop)


#undef INTERFACE



