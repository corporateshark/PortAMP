#pragma once

/**
	A wave data provider can convert anything to PCM

	Based on https://github.com/corporateshark/Android-NDK-Game-Development-Cookbook/tree/master/Chapter5
**/

#include <stdint.h>
#include <memory>

class clBlob;

class sWaveDataFormat
{
public:
	sWaveDataFormat()
	: m_NumChannels( 0 )
	, m_SamplesPerSecond( 0 )
	, m_BitsPerSample( 0 )
	{}

public:
	int m_NumChannels;
	int m_SamplesPerSecond;
	int m_BitsPerSample;
};

class iWaveDataProvider
{
public:
	virtual ~iWaveDataProvider() {};

	virtual const sWaveDataFormat& GetWaveDataFormat() const = 0;

	virtual const uint8_t* GetWaveData() const = 0;
	virtual size_t GetWaveDataSize() const = 0;

	virtual bool IsStreaming() const { return false; }
	virtual bool IsEndOfStream() const { return false; }
	virtual void Seek( float Seconds ) {}
	virtual size_t StreamWaveData( size_t Size ) { return 0; }
};

std::shared_ptr<iWaveDataProvider> CreateWaveDataProvider( const char* FileName, const std::shared_ptr<clBlob>& Data );
