/*
=============================================================================
	File:	Tokenizer.cpp
	Desc:	
=============================================================================
*/
#include <Audio_PCH.h>
#pragma hdrstop
#include <Audio/API.h>

//-----------------------------------------------------------------------------
// Global defines
//-----------------------------------------------------------------------------
#define INPUTCHANNELS 1  // number of source channels
#define OUTPUTCHANNELS 8 // maximum number of destination channels supported in this sample
#define NUM_PRESETS 30

// Must match order of g_PRESET_NAMES
XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS[ NUM_PRESETS ] =
{
    XAUDIO2FX_I3DL2_PRESET_FOREST,
    XAUDIO2FX_I3DL2_PRESET_DEFAULT,
    XAUDIO2FX_I3DL2_PRESET_GENERIC,
    XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
    XAUDIO2FX_I3DL2_PRESET_ROOM,
    XAUDIO2FX_I3DL2_PRESET_BATHROOM,
    XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
    XAUDIO2FX_I3DL2_PRESET_STONEROOM,
    XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
    XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
    XAUDIO2FX_I3DL2_PRESET_CAVE,
    XAUDIO2FX_I3DL2_PRESET_ARENA,
    XAUDIO2FX_I3DL2_PRESET_HANGAR,
    XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
    XAUDIO2FX_I3DL2_PRESET_HALLWAY,
    XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
    XAUDIO2FX_I3DL2_PRESET_ALLEY,
    XAUDIO2FX_I3DL2_PRESET_CITY,
    XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
    XAUDIO2FX_I3DL2_PRESET_QUARRY,
    XAUDIO2FX_I3DL2_PRESET_PLAIN,
    XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
    XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
    XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
    XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
    XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
    XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
    XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
    XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
    XAUDIO2FX_I3DL2_PRESET_PLATE,
};



namespace Audio
{

struct AUDIO_SYSTEM_DATA
{
	// XAudio2
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	IXAudio2SourceVoice* pSourceVoice;
	IXAudio2SubmixVoice* pSubmixVoice;
	IUnknown* pReverbEffect;
	BYTE* pbSampleData;

	// 3D
	X3DAUDIO_HANDLE x3DInstance;
	int nFrameToApply3DAudio;

	DWORD dwChannelMask;
	UINT32 nChannels;

#if 0
	X3DAUDIO_DSP_SETTINGS dspSettings;
	X3DAUDIO_LISTENER listener;
	X3DAUDIO_EMITTER emitter;
	X3DAUDIO_CONE emitterCone;

	D3DXVECTOR3 vListenerPos;
	D3DXVECTOR3 vEmitterPos;
	float fListenerAngle;
	bool  fUseListenerCone;
	bool  fUseInnerRadius;
	bool  fUseRedirectToLFE;

	FLOAT32 emitterAzimuths[INPUTCHANNELS];
	FLOAT32 matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
#endif

public:
	AUDIO_SYSTEM_DATA()
	{
		ZERO_OUT(*this);
	}
	~AUDIO_SYSTEM_DATA()
	{
		if( this->pSourceVoice != nil )
		{
			this->pSourceVoice->DestroyVoice();
			this->pSourceVoice = NULL;
		}

		if( this->pSubmixVoice != nil )
		{
			this->pSubmixVoice->DestroyVoice();
			this->pSubmixVoice = NULL;
		}

		if( this->pMasteringVoice != nil )
		{
			this->pMasteringVoice->DestroyVoice();
			this->pMasteringVoice = NULL;
		}

		this->pXAudio2->StopEngine();
		SAFE_RELEASE( this->pXAudio2 );

		SAFE_RELEASE( this->pReverbEffect );

		SAFE_DELETE_ARRAY( this->pbSampleData );

		::CoUninitialize();
	}
};

static TBlob16< AUDIO_SYSTEM_DATA >		gDataStorage;
static NiftyCounter		gAudioSystemInitCounter;

#define gData	gDataStorage.Get()

//-----------------------------------------------------------------------------------------
// Releases XAudio2
//-----------------------------------------------------------------------------------------
static bool CloseXAudio2()
{
	gDataStorage.Destruct();

	return true;
}

//-----------------------------------------------------------------------------------------
// Initialize the audio by creating the XAudio2 device, mastering voice, etc.
//-----------------------------------------------------------------------------------------
static bool SetupXAudio2()
{
	gDataStorage.Construct();

	// On Windows, the application must call the CoInitializeEx method before calling XAudio2Create.
	// In addition, the rules of COM require CoInitialize or CoInitializeEx to be called in any thread that makes calls to XAudio2. 

#ifndef _XBOX
	::CoInitializeEx( NULL, COINIT_MULTITHREADED );
#endif


	// Create an instance of the XAudio2 engine.

	UINT32 flags = 0;

#if MX_DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif // MX_DEBUG

	HRESULT hr;

	if ( FAILED(hr = XAudio2Create( &gData.pXAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR ) ) )
	{
		CloseXAudio2();
		return false;
	}

	// The mastering voices encapsulates an audio device, and is the ultimate destination for all audio that passes through an audio graph.

	if ( FAILED(hr = gData.pXAudio2->CreateMasteringVoice(
		&gData.pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS,
		XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL ) ) )
	{
		CloseXAudio2();
		return false;
	}

	// Check device details to make sure it's within our sample supported parameters

	XAUDIO2_DEVICE_DETAILS details;
	if( FAILED( hr = gData.pXAudio2->GetDeviceDetails( 0, &details ) ) )
	{
		CloseXAudio2();
		return false;
	}

	if( details.OutputFormat.Format.nChannels > OUTPUTCHANNELS )
	{
		CloseXAudio2();
		return false;
	}

	gData.dwChannelMask = details.OutputFormat.dwChannelMask;
	gData.nChannels = details.OutputFormat.Format.nChannels;

	//
    // Create reverb effect
    //
    flags = 0;
#ifdef MX_DEBUG
    flags |= XAUDIO2FX_DEBUG;
#endif

    if( FAILED( hr = XAudio2CreateReverb( &gData.pReverbEffect, flags ) ) )
    {
        CloseXAudio2();
		return false;
    }

    //
    // Create a submix voice
    //

    // Performance tip: you need not run global FX with the sample number
    // of channels as the final mix.  For example, this sample runs
    // the reverb in mono mode, thus reducing CPU overhead.
    XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { gData.pReverbEffect, TRUE, 1 } };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };


    if( FAILED( hr = gData.pXAudio2->CreateSubmixVoice( &gData.pSubmixVoice, 1,
                                                         details.OutputFormat.Format.nSamplesPerSec, 0, 0,
                                                         NULL, &effectChain ) ) )
    {
        CloseXAudio2();
		return false;
    }

    // Set default FX params
    XAUDIO2FX_REVERB_PARAMETERS native;
    ReverbConvertI3DL2ToNative( &g_PRESET_PARAMS[0], &native );
    gData.pSubmixVoice->SetEffectParameters( 0, &native, sizeof( native ) );

    //
    // Initialize X3DAudio
    //  Speaker geometry configuration on the final mix, specifies assignment of channels
    //  to speaker positions, defined as per WAVEFORMATEXTENSIBLE.dwChannelMask
    //
    //  SpeedOfSound - speed of sound in user-defined world units/second, used
    //  only for doppler calculations, it must be >= FLT_MIN
    //
    const float SPEEDOFSOUND = X3DAUDIO_SPEED_OF_SOUND;

    X3DAudioInitialize( details.OutputFormat.dwChannelMask, SPEEDOFSOUND, gData.x3DInstance );


#if 0
    gData.vListenerPos = D3DXVECTOR3( 0, 0, 0 );
    gData.vEmitterPos = D3DXVECTOR3( 0, 0, float( ZMAX ) );

    gData.fListenerAngle = 0;
    gData.fUseListenerCone = TRUE;
    gData.fUseInnerRadius = TRUE;
    gData.fUseRedirectToLFE = ((details.OutputFormat.dwChannelMask & SPEAKER_LOW_FREQUENCY) != 0);

    //
    // Setup 3D audio structs
    //
    gData.listener.Position = gData.vListenerPos;
    gData.listener.OrientFront = D3DXVECTOR3( 0, 0, 1 );
    gData.listener.OrientTop = D3DXVECTOR3( 0, 1, 0 );
    gData.listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

    gData.emitter.pCone = &gData.emitterCone;
    gData.emitter.pCone->InnerAngle = 0.0f;
    // Setting the inner cone angles to X3DAUDIO_2PI and
    // outer cone other than 0 causes
    // the emitter to act like a point emitter using the
    // INNER cone settings only.
    gData.emitter.pCone->OuterAngle = 0.0f;
    // Setting the outer cone angles to zero causes
    // the emitter to act like a point emitter using the
    // OUTER cone settings only.
    gData.emitter.pCone->InnerVolume = 0.0f;
    gData.emitter.pCone->OuterVolume = 1.0f;
    gData.emitter.pCone->InnerLPF = 0.0f;
    gData.emitter.pCone->OuterLPF = 1.0f;
    gData.emitter.pCone->InnerReverb = 0.0f;
    gData.emitter.pCone->OuterReverb = 1.0f;

    gData.emitter.Position = gData.vEmitterPos;
    gData.emitter.OrientFront = D3DXVECTOR3( 0, 0, 1 );
    gData.emitter.OrientTop = D3DXVECTOR3( 0, 1, 0 );
    gData.emitter.ChannelCount = INPUTCHANNELS;
    gData.emitter.ChannelRadius = 1.0f;
    gData.emitter.pChannelAzimuths = gData.emitterAzimuths;

    // Use of Inner radius allows for smoother transitions as
    // a sound travels directly through, above, or below the listener.
    // It also may be used to give elevation cues.
    gData.emitter.InnerRadius = 2.0f;
    gData.emitter.InnerRadiusAngle = X3DAUDIO_PI/4.0f;;

    gData.emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
    gData.emitter.pLFECurve    = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
    gData.emitter.pLPFDirectCurve = NULL; // use default curve
    gData.emitter.pLPFReverbCurve = NULL; // use default curve
    gData.emitter.pReverbCurve    = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
    gData.emitter.CurveDistanceScaler = 14.0f;
    gData.emitter.DopplerScaler = 1.0f;

    gData.dspSettings.SrcChannelCount = INPUTCHANNELS;
    gData.dspSettings.DstChannelCount = gData.nChannels;
    gData.dspSettings.pMatrixCoefficients = gData.matrixCoefficients;
#endif

	return true;
}

//-----------------------------------------------------------------------------------------
bool Initialize()
{
	if( gAudioSystemInitCounter.IncRef() )
	{
		return SetupXAudio2();
	}
	return true;
}
//-----------------------------------------------------------------------------------------
bool Shutdown()
{
	if( gAudioSystemInitCounter.DecRef() )
	{
		return CloseXAudio2();
	}
	return false;
}

}//namespace Audio

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
