#include "WAVDataEncoder.h"

static void PatchValue( uint8_t* Ptr, uint64_t Value, size_t Size )
{
	for ( ; Size; --Size, Value >>= 8 )
	{
		*Ptr++ = static_cast<uint8_t>( Value & 0xFF );
	}
}

void clWAVDataEncoder::OutValue( uint64_t Value, size_t Size )
{
	for ( ; Size; --Size, Value >>= 8 )
	{
		m_OutputData.push_back( static_cast<uint8_t>( Value & 0xFF ) );
	}
}

void clWAVDataEncoder::OutBytes( const void* Bytes, size_t Size )
{
	m_OutputData.insert(
		m_OutputData.end(),
		reinterpret_cast<const uint8_t*>(Bytes),
		reinterpret_cast<const uint8_t*>(Bytes)+Size
	);
}

void clWAVDataEncoder::OutString( const char* Str )
{
	OutBytes( Str, strlen(Str) );
}

bool clWAVDataEncoder::ResetEncoder( int NumChannels, int NumSamplesPerSec, int BitsPerSample, float Quality )
{
	m_OutputData.clear();
	m_OutputData.reserve( 1024*1024 );

	OutString( "RIFF----WAVEfmt " );

	// write WAVEfmt header
	OutValue( 16, 4 );
	OutValue( 1, 2 );	// PCM
	OutValue( NumChannels, 2 );
	OutValue( NumSamplesPerSec, 4 );
	OutValue( NumSamplesPerSec * BitsPerSample * NumChannels / 8, 4 );
	OutValue( NumChannels * sizeof( uint16_t ), 2 ); // data block size
	OutValue( 16, 2 );	// bits per sample

	m_DataChunkPos = m_OutputData.size();

	OutString( "data----" );

	return false;
}

void clWAVDataEncoder::EncodePCMData( const void* PCMData, size_t PCMDataSizeBytes )
{
}

const std::vector<uint8_t>& clWAVDataEncoder::FinalizeAndGetResult()
{
	const size_t FileLength = m_OutputData.size();

	PatchValue( m_OutputData.data() + m_DataChunkPos + 4, FileLength - m_DataChunkPos + 8, 4 );
	PatchValue( m_OutputData.data() + 4, FileLength - 8, 4 );

	return m_OutputData;
}
