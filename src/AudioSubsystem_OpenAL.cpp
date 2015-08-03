/**
	A portable OpenAL audio subsystem implementation

	Based on https://github.com/corporateshark/Android-NDK-Game-Development-Cookbook/tree/master/Chapter5
**/

#include <atomic>
#include <thread>

#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "OpenAL/LAL.h"

static ALenum ConvertWaveDataFormatToAL( const sWaveDataFormat& F )
{
	if ( F.m_BitsPerSample == 8 )
	{
		return ( F.m_NumChannels == 2 ) ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
	}

	if ( F.m_BitsPerSample == 16 )
	{
		return ( F.m_NumChannels == 2 ) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	}

	return AL_FORMAT_MONO8;
}

class clAudioSource_OpenAL: public iAudioSource
{
public:
	clAudioSource_OpenAL()
	: m_DataProvider()
	, m_BuffersCount(0)
	, m_SourceID(0)
	, m_BufferID()
	{
		alGenSources( 1, &m_SourceID );
	}

	virtual ~clAudioSource_OpenAL()
	{
		this->Stop();
		alDeleteSources( 1, &m_SourceID );
		alDeleteBuffers( m_BuffersCount, m_BufferID );
	}

	virtual void BindDataProvider( const std::shared_ptr<iWaveDataProvider>& Provider ) override;

	virtual void Play() override;
	virtual void Stop() override;
	virtual bool IsPlaying() const override;

private:
	std::shared_ptr<iWaveDataProvider> m_DataProvider;

	// OpenAL stuff
	ALsizei m_BuffersCount;
	ALuint  m_SourceID;
	ALuint  m_BufferID[2];
};

void clAudioSource_OpenAL::BindDataProvider( const std::shared_ptr<iWaveDataProvider>& Provider )
{
	m_DataProvider = Provider;

	if ( m_DataProvider ) return;

	if ( m_BuffersCount )
	{
		alDeleteBuffers( m_BuffersCount, m_BufferID );
	}

	sWaveDataFormat Format = m_DataProvider->GetWaveDataFormat();

	if ( m_DataProvider->IsStreaming() )
	{
		m_BuffersCount = 2;
		alGenBuffers( m_BuffersCount, m_BufferID );
	}
	else
	{
		m_BuffersCount = 1;
		alGenBuffers( m_BuffersCount, m_BufferID );
		alBufferData(
			m_BufferID[0],
			ConvertWaveDataFormatToAL( Format ),
			m_DataProvider->GetWaveData(),
			m_DataProvider->GetWaveDataSize(),
			Format.m_SamplesPerSecond
		);
		alSourcei( m_SourceID, AL_BUFFER, m_BufferID[0] );
	}
}

void clAudioSource_OpenAL::Play()
{
	if ( this->IsPlaying() ) return;

	if ( !m_DataProvider ) return;

	alSourcePlay( m_SourceID );
}

void clAudioSource_OpenAL::Stop()
{
	alSourceStop( m_SourceID );

	if ( m_DataProvider )
	{
		m_DataProvider->Seek( 0.0f );
	}
}

bool clAudioSource_OpenAL::IsPlaying() const
{
	ALint State = 0;

	alGetSourcei( m_SourceID, AL_SOURCE_STATE, &State );

	return State == AL_PLAYING;
}

class clAudioSubsystem_OpenAL: public iAudioSubsystem
{
public:
	clAudioSubsystem_OpenAL();
	virtual ~clAudioSubsystem_OpenAL();

	virtual void Start() override;
	virtual void Stop() override;

	virtual std::shared_ptr<iAudioSource> CreateAudioSource() override;

	virtual void SetListenerGain( float Gain ) override;

private:
	void DebugPrintVersion();

private:
	ALCdevice*  m_Device;
	ALCcontext* m_Context;

	std::shared_ptr<std::thread> m_AudioThread;
	std::atomic<bool> m_IsPendingExit;
	std::atomic<bool> m_IsInitialized;
};

std::shared_ptr<iAudioSubsystem> CreateAudioSubsystem_OpenAL()
{
	return std::make_shared<clAudioSubsystem_OpenAL>();
}

clAudioSubsystem_OpenAL::clAudioSubsystem_OpenAL()
: m_Device( nullptr )
, m_Context( nullptr )
, m_AudioThread()
, m_IsPendingExit( false )
, m_IsInitialized( false )
{
	LoadAL();
}

clAudioSubsystem_OpenAL::~clAudioSubsystem_OpenAL()
{
	UnloadAL();
}

void clAudioSubsystem_OpenAL::DebugPrintVersion()
{
	printf( "OpenAL version : %s\n", alGetString( AL_VERSION  ) );
	printf( "OpenAL vendor  : %s\n", alGetString( AL_VENDOR   ) );
	printf( "OpenAL renderer: %s\n", alGetString( AL_RENDERER ) );
	printf( "OpenAL extensions:\n%s\n", alGetString( AL_EXTENSIONS ) );
}

void clAudioSubsystem_OpenAL::Start()
{
	m_AudioThread = std::make_shared<std::thread>(
		[this]()
		{
			m_Device = alcOpenDevice( nullptr );
			m_Context = alcCreateContext( m_Device, nullptr );
			alcMakeContextCurrent( m_Context );

			DebugPrintVersion();

			m_IsInitialized = true;

			while ( !m_IsPendingExit )
			{
			}

			alcDestroyContext( m_Context );
			alcCloseDevice( m_Device );

			m_IsInitialized = false;

		}
	);
}

void clAudioSubsystem_OpenAL::Stop()
{
	m_IsPendingExit = true;

	m_AudioThread->join();
}

std::shared_ptr<iAudioSource> clAudioSubsystem_OpenAL::CreateAudioSource()
{
	return std::make_shared<clAudioSource_OpenAL>();
}

void clAudioSubsystem_OpenAL::SetListenerGain( float Gain )
{
	alListenerf( AL_GAIN, Gain );
}
