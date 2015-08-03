/**
	A portable OpenAL audio subsystem implementation

	Based on https://github.com/corporateshark/Android-NDK-Game-Development-Cookbook/tree/master/Chapter5
**/

#include "AudioSubsystem_OpenAL.h"

class clAudioSource_OpenAL: public iAudioSource
{
public:
};

class clAudioSubsystem_OpenAL: public iAudioSubsystem
{
public:
	virtual void Start() override;
	virtual void Stop() override;

	virtual std::shared_ptr<iAudioSource> CreateAudioSource() override;

	virtual void SetListenerGain( float Gain ) override;
};

std::shared_ptr<iAudioSubsystem> CreateAudioSubsystem_OpenAL()
{
	return std::make_shared<clAudioSubsystem_OpenAL>();
}

void clAudioSubsystem_OpenAL::Start()
{
}

void clAudioSubsystem_OpenAL::Stop()
{
}

std::shared_ptr<iAudioSource> clAudioSubsystem_OpenAL::CreateAudioSource()
{
	return std::make_shared<clAudioSource_OpenAL>();
}

void clAudioSubsystem_OpenAL::SetListenerGain( float Gain )
{
}
