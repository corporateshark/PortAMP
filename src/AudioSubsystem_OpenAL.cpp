/**
	A portable OpenAL audio subsystem implementation

	Based on https://github.com/corporateshark/Android-NDK-Game-Development-Cookbook/tree/master/Chapter5
**/

#include <atomic>
#include <thread>

#include "AudioSubsystem_OpenAL.h"
#include "OpenAL/LAL.h"

class clAudioSource_OpenAL: public iAudioSource
{
public:
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
}
