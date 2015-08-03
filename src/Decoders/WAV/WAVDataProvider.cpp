#include <algorithm>

#include "WAVDataProvider.h"
#include "Utils.h"

/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd757713(v=vs.85).aspx
#pragma pack(push, 1)
#if !defined(_MSC_VER)
#	define PACKED_STRUCT(n) __attribute__((packed,aligned(n)))
#else
#	define PACKED_STRUCT(n) __declspec(align(n))
#endif
struct PACKED_STRUCT(1) sWAVHeader
{
	// RIFF header
	uint8_t  RIFF[4];
	uint32_t FileSize;
	uint8_t  WAVE[4];
	uint8_t  FMT[4];
	uint32_t SizeFmt;
	// WAVEFORMATEX structure
	uint16_t FormatTag;
	uint16_t Channels;
	uint32_t SampleRate;
	uint32_t AvgBytesPerSec;
	uint16_t nBlockAlign;
	uint16_t nBitsperSample;
	uint8_t  Reserved[4];
	uint32_t DataSize;
};
#pragma pack(pop)

clWAVDataProvider::clWAVDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_DataSize( Data ? Data->GetDataSize() : 0 )
, m_Format()
{
	if ( Data && Data->GetDataSize() > sizeof(sWAVHeader) )
	{
		const sWAVHeader* Header = reinterpret_cast<const sWAVHeader*>( Data->GetDataPtr() );

		const uint16_t FORMAT_PCM = 1;

		bool IsPCM  = Header->FormatTag == FORMAT_PCM;
		bool IsRIFF = memcmp( &Header->RIFF, "RIFF", 4 ) == 0;
		bool IsWAVE = memcmp( &Header->WAVE, "WAVE", 4 ) == 0;

		// can only handle uncompressed .WAV files
		if ( IsRIFF && IsWAVE && IsPCM )
		{
			m_Format.m_NumChannels      = Header->Channels;
			m_Format.m_SamplesPerSecond = Header->SampleRate;
			m_Format.m_BitsPerSample    = Header->nBitsperSample;

			m_DataSize = std::min( Header->DataSize, Data->GetDataSize() - sizeof(sWAVHeader) );
		}
	}
}

const uint8_t* clWAVDataProvider::GetWaveData() const
{
	return m_Data ? m_Data->GetDataPtr() + sizeof( sWAVHeader ) : nullptr;
}

size_t clWAVDataProvider::GetWaveDataSize() const
{
	return m_DataSize;
}

size_t clWAVDataProvider::StreamWaveData( size_t size )
{
	return 0;
}

void clWAVDataProvider::Seek( float Seconds )
{
	// TODO:
}
