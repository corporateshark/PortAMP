#pragma once

#include "Encoders/iWaveDataEncoder.h"

class clWAVDataEncoder: public iWaveDataEncoder
{
public:
	virtual bool ResetEncoder( int NumChannels, int NumSamplesPerSec, int BitsPerSample, float Quality ) override;
	virtual void EncodePCMData( const void* PCMData, size_t PCMDataSizeBytes ) override;
	virtual const std::vector<uint8_t>& FinalizeAndGetResult() override;

private:
	void OutValue( uint64_t Value, size_t Size );
	void OutBytes( const void* Bytes, size_t Size );
	void OutString( const char* Str );

private:
	size_t m_DataChunkPos;
	std::vector<uint8_t> m_OutputData;
};
