/**
	A portable audio subsystem wrapper

	Based on https://github.com/corporateshark/Android-NDK-Game-Development-Cookbook/tree/master/Chapter5
**/

#pragma once

#include <memory>

class iWaveDataProvider;

class iAudioSource
{
public:
	virtual void BindDataProvider( const std::shared_ptr<iWaveDataProvider>& Provider ) = 0;

	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual bool IsPlaying() const = 0;
};

class iAudioSubsystem
{
public:
	virtual ~iAudioSubsystem() {};

	virtual void Start() = 0;
	virtual void Stop() = 0;

	virtual std::shared_ptr<iAudioSource> CreateAudioSource() = 0;

	virtual void SetListenerGain( float Gain ) = 0;
};
