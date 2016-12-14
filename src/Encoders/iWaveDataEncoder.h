#pragma once

#include <vector>

class iWaveDataEncoder
{
public:
	virtual ~iWaveDataEncoder() {};

	virtual bool ResetEncoder( int NumChannels, int NumSamplesPerSec, int BitsPerSample, float Quality ) = 0;
	virtual void EncodePCMData( const void* PCMData, size_t PCMDataSizeBytes ) = 0;
	virtual const std::vector<uint8_t>& FinalizeAndGetResult() = 0;
};
