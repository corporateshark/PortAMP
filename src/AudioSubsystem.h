#pragma once

#include <memory>

class iAudioSource
{
public:
};

class iAudioSubsystem
{
public:
	virtual void Start() = 0;
	virtual void Shutdown() = 0;

	virtual std::shared_ptr<iAudioSource> CreateAudioSource() = 0;

	virtual void SetListenerGain( float Gain ) = 0;
};
