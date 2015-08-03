/**
	A portable audio subsystem wrapper

	Based on https://github.com/corporateshark/Android-NDK-Game-Development-Cookbook/tree/master/Chapter5
**/

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
	virtual void Stop() = 0;

	virtual std::shared_ptr<iAudioSource> CreateAudioSource() = 0;

	virtual void SetListenerGain( float Gain ) = 0;
};
