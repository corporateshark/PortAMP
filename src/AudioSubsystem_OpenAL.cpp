/**
	A portable OpenAL audio subsystem implementation

	Based on https://github.com/corporateshark/Android-NDK-Game-Development-Cookbook/tree/master/Chapter5
**/

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "OpenAL/LAL.h"
#include "Utils.h"

const int BUFFER_DURATION = 250; // milliseconds
const int BUFFER_SIZE = 44100 * 2 * 2 * BUFFER_DURATION / 1000;

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

class clAudioSubsystem_OpenAL;

class clAudioSource_OpenAL: public iAudioSource, public std::enable_shared_from_this<clAudioSource_OpenAL>
{
public:
	explicit clAudioSource_OpenAL( clAudioSubsystem_OpenAL* AudioSubsystem )
	: m_AudioSubsystem( AudioSubsystem )
	, m_DataProvider()
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
	virtual void SetLooping( bool Looping ) override;

private:
	void PrepareBuffers();
	void UnqueueAllBuffers();
	void UpdateBuffers();
	void EnqueueOneBuffer();
	size_t StreamBuffer( ALuint bufferId, size_t Size );
	friend class clAudioSubsystem_OpenAL;

private:
	clAudioSubsystem_OpenAL* m_AudioSubsystem;
	std::shared_ptr<iWaveDataProvider> m_DataProvider;

	// OpenAL stuff
	ALsizei m_BuffersCount;
	ALuint  m_SourceID;
	ALuint  m_BufferID[2];
};

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
	void RegisterSource( const std::shared_ptr<clAudioSource_OpenAL>& Source );
	void UnregisterSource( clAudioSource_OpenAL* Source );
	std::vector< std::shared_ptr<clAudioSource_OpenAL> > GetLockedSources()
	{
		std::unique_lock<std::mutex> Lock( m_ActiveSourcesMutex );

		return m_ActiveSources;
	}

	friend class clAudioSource_OpenAL;

private:
	ALCdevice*  m_Device;
	ALCcontext* m_Context;

	std::shared_ptr<std::thread> m_AudioThread;
	std::atomic<bool> m_IsPendingExit;
	std::atomic<bool> m_IsInitialized;

	std::vector< std::shared_ptr<clAudioSource_OpenAL> > m_ActiveSources;
	std::mutex m_ActiveSourcesMutex;
};

void clAudioSource_OpenAL::BindDataProvider( const std::shared_ptr<iWaveDataProvider>& Provider )
{
	m_DataProvider = Provider;

	if ( !m_DataProvider ) return;

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

void clAudioSource_OpenAL::PrepareBuffers()
{
	if ( !m_DataProvider ) return;

	int State;
	alGetSourcei( m_SourceID, AL_SOURCE_STATE, &State );

	if ( State != AL_PAUSED && m_DataProvider->IsStreaming() )
	{
		UnqueueAllBuffers();

		int BuffersToQueue = 2;

		StreamBuffer( m_BufferID[0], BUFFER_SIZE );
		if ( StreamBuffer( m_BufferID[1], BUFFER_SIZE ) == 0 )
		{
			if ( IsLooping() )
			{
				m_DataProvider->Seek(0);
				StreamBuffer(m_BufferID[1], BUFFER_SIZE);
			}
			else
			{
				BuffersToQueue = 1;
			}
		}

		alSourceQueueBuffers( m_SourceID, BuffersToQueue, &m_BufferID[0] );

		m_AudioSubsystem->RegisterSource( shared_from_this() );
	}
}

void clAudioSource_OpenAL::UnqueueAllBuffers()
{
	int NumQueued;
	alGetSourcei( m_SourceID, AL_BUFFERS_QUEUED, &NumQueued );

	if ( NumQueued > 0 )
	{
		alSourceUnqueueBuffers( m_SourceID, NumQueued, &m_BufferID[0] );
	}
}

size_t clAudioSource_OpenAL::StreamBuffer( ALuint BufferID, size_t Size )
{
	if ( !m_DataProvider ) return 0;

	size_t ActualSize = m_DataProvider->StreamWaveData( Size );

	if ( !ActualSize ) return 0;

	const uint8_t* Data = m_DataProvider->GetWaveData();
	size_t DataSize = m_DataProvider->GetWaveDataSize();
	const sWaveDataFormat& Format = m_DataProvider->GetWaveDataFormat();

	alBufferData(
		BufferID,
		ConvertWaveDataFormatToAL( Format ),
		Data,
		DataSize,
		Format.m_SamplesPerSecond
	);

	return ActualSize;
}

void clAudioSource_OpenAL::EnqueueOneBuffer()
{
	ALuint BufID;
	alSourceUnqueueBuffers( m_SourceID, 1, &BufID );

	size_t Size = this->StreamBuffer( BufID, BUFFER_SIZE );

	if ( m_DataProvider->IsEndOfStream() )
	{
		if ( this->IsLooping() )
		{
			m_DataProvider->Seek( 0 );
			if ( !Size ) Size = StreamBuffer( BufID, BUFFER_SIZE );
		}
	}

	if ( Size )
	{
		alSourceQueueBuffers(m_SourceID, 1, &BufID);
	}
	else
	{
		// there is nothing more to play, but don't stop now if something is still playing
		if ( !this->IsPlaying() ) this->Stop();
	}
}

void clAudioSource_OpenAL::UpdateBuffers()
{
	if ( !m_DataProvider ) return;

	if ( m_DataProvider->IsStreaming() )
	{
		int NumProcessed = 0;
		alGetSourcei( m_SourceID, AL_BUFFERS_PROCESSED, &NumProcessed );

		switch ( NumProcessed )
		{
		case 0:
			// nothing was processed
			break;
		case 1:
		{
			EnqueueOneBuffer();
			break;
		}
		case 2:
		{
			EnqueueOneBuffer();
			EnqueueOneBuffer();
			alSourcePlay( m_SourceID );
			break;
		}
		default:
			break;
		}
	}
}

void clAudioSource_OpenAL::Play()
{
	if ( this->IsPlaying() ) return;

	if ( !m_DataProvider ) return;

	PrepareBuffers();

	alSourcePlay( m_SourceID );
}

void clAudioSource_OpenAL::Stop()
{
	alSourceStop( m_SourceID );

	UnqueueAllBuffers();

	m_AudioSubsystem->UnregisterSource( this );

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

void clAudioSource_OpenAL::SetLooping( bool Looping )
{
	iAudioSource::SetLooping( Looping );

	bool IsStreaming = m_DataProvider && m_DataProvider->IsStreaming();

	if ( m_DataProvider && !IsStreaming )
	{
		alSourcei( m_SourceID, AL_LOOPING, Looping ? AL_TRUE : AL_FALSE );
	}
}

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
	std::lock_guard<std::mutex> Lock( m_ActiveSourcesMutex );

	m_ActiveSources.clear();

	UnloadAL();
}

void clAudioSubsystem_OpenAL::DebugPrintVersion()
{
	printf( "OpenAL version : %s\n", alGetString( AL_VERSION  ) );
	printf( "OpenAL vendor  : %s\n", alGetString( AL_VENDOR   ) );
	printf( "OpenAL renderer: %s\n", alGetString( AL_RENDERER ) );
	printf( "OpenAL extensions:\n%s\n\n", alGetString( AL_EXTENSIONS ) );
}

void clAudioSubsystem_OpenAL::Start()
{
	m_AudioThread = std::make_shared<std::thread>(
		[this]()
		{
			m_Device = alcOpenDevice( nullptr );
			m_Context = alcCreateContext( m_Device, nullptr );
			alcMakeContextCurrent( m_Context );

			if ( IsVerbose() ) DebugPrintVersion();

			m_IsInitialized = true;

			while ( !m_IsPendingExit )
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));

				{
					auto sources = this->GetLockedSources();

					for (auto i : sources)
					{
						i->UpdateBuffers();
					}
				}
			}

			// A dirty workaround for a possible bug in OpenAL Soft
			// http://openal.org/pipermail/openal/2015-January/000312.html
#if !defined(_WIN32)
			alcDestroyContext( m_Context );
			alcCloseDevice( m_Device );
#endif

			m_IsInitialized = false;

		}
	);

	// wait
	while ( !m_IsInitialized );
}

void clAudioSubsystem_OpenAL::RegisterSource( const std::shared_ptr<clAudioSource_OpenAL>& Source )
{
	std::unique_lock<std::mutex> Lock( m_ActiveSourcesMutex );

	if ( std::find( m_ActiveSources.begin(), m_ActiveSources.end(), Source) != m_ActiveSources.end() ) return;

	m_ActiveSources.push_back( Source );
}

void clAudioSubsystem_OpenAL::UnregisterSource( clAudioSource_OpenAL* Source )
{
	std::unique_lock<std::mutex> Lock( m_ActiveSourcesMutex );

	m_ActiveSources.erase(
		std::remove_if( m_ActiveSources.begin(), m_ActiveSources.end(),
			[Source](const std::shared_ptr<clAudioSource_OpenAL>& Src){ return Src.get() == Source; } ),
		m_ActiveSources.end()
	);
}

void clAudioSubsystem_OpenAL::Stop()
{
	m_IsPendingExit = true;

	m_AudioThread->join();
}

std::shared_ptr<iAudioSource> clAudioSubsystem_OpenAL::CreateAudioSource()
{
	return std::make_shared<clAudioSource_OpenAL>( this );
}

void clAudioSubsystem_OpenAL::SetListenerGain( float Gain )
{
	alListenerf( AL_GAIN, Gain );
}
