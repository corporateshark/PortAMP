#include  <cstring>
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

#include "Decoders/MP3/MP3DataProvider.h"

std::shared_ptr<clBlob> TryMP3InsideWAV( const std::shared_ptr<clBlob>& Data )
{
	if (!Data || Data->GetDataSize() < sizeof(sWAVHeader)) return std::shared_ptr<clBlob>();

	const sWAVHeader* Header = reinterpret_cast<const sWAVHeader*>( Data->GetDataPtr() );

	const uint16_t FORMAT_MP3 = 0x0055;

	bool IsMP3  = Header->FormatTag == FORMAT_MP3;
	bool IsRIFF = memcmp( &Header->RIFF, "RIFF", 4 ) == 0;
	bool IsWAVE = memcmp( &Header->WAVE, "WAVE", 4 ) == 0;

	if ( IsRIFF && IsWAVE && IsMP3 )
	{
		std::vector<uint8_t> MP3Data( Data->GetDataPtr() + sizeof(sWAVHeader), Data->GetDataPtr() + Data->GetDataSize() );

		return std::make_shared<clBlob>( MP3Data );
	}

	return nullptr;
}

template <typename T> void ConvertClamp_IEEEToInt16( const T* Src, int16_t* Dst, size_t NumFloats )
{
	const T* End = Src + NumFloats;

	while ( Src < End )
	{
		T f = *Src++;
		int32_t v = int( f * 32167.0 );
		*Dst++ = ( v > 32167 ) ? 32167 : ( v < -32167 ) ? -32167 : v;
	}
}

clWAVDataProvider::clWAVDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_DataSize( Data ? Data->GetDataSize() : 0 )
, m_Format()
{
	if ( Data && Data->GetDataSize() > sizeof(sWAVHeader) )
	{
		const sWAVHeader* Header = reinterpret_cast<const sWAVHeader*>( Data->GetDataPtr() );

		const uint16_t FORMAT_PCM   = 0x0001;
		const uint16_t FORMAT_FLOAT = 0x0003;

		bool IsPCM   = Header->FormatTag == FORMAT_PCM;
		bool IsFloat = Header->FormatTag == FORMAT_FLOAT;
		bool IsRIFF = memcmp( &Header->RIFF, "RIFF", 4 ) == 0;
		bool IsWAVE = memcmp( &Header->WAVE, "WAVE", 4 ) == 0;

		// can only handle uncompressed .WAV files
		if ( IsRIFF && IsWAVE && (IsPCM|IsFloat) )
		{
			m_Format.m_NumChannels      = Header->Channels;
			m_Format.m_SamplesPerSecond = Header->SampleRate;
			m_Format.m_BitsPerSample    = Header->nBitsperSample;

			m_DataSize = std::min( static_cast<size_t>(Header->DataSize), Data->GetDataSize() - sizeof(sWAVHeader) );

			if ( IsFloat )
			{
				// replace the blob and convert data to 16-bit
				std::vector<uint8_t> NewData;
				NewData.resize( m_Data->GetDataSize() );
				int16_t* Dst = reinterpret_cast<int16_t*>( NewData.data()+sizeof(sWAVHeader) );

				if ( Header->nBitsperSample == 32 )
				{
					const float* Src = reinterpret_cast<const float*>( m_Data->GetDataPtr()+sizeof(sWAVHeader)+2 );
					ConvertClamp_IEEEToInt16<float>( Src, Dst, m_DataSize / 4 );
					m_DataSize = m_DataSize/2 - 3;
				}
				else if ( Header->nBitsperSample == 64 )
				{
					const double* Src = reinterpret_cast<const double*>( m_Data->GetDataPtr()+sizeof(sWAVHeader)+2 );
					ConvertClamp_IEEEToInt16<double>( Src, Dst, m_DataSize / 8 );
					m_DataSize = m_DataSize/4 - 3;
				}
				else 
				{
					Log_Error( "Unknown float format in WAV" );
					m_DataSize = 0;
				}

				m_Data = std::make_shared<clBlob>( NewData );
				m_Format.m_BitsPerSample = 16;
			}

			if ( IsVerbose() )
			{
				printf( "PCM WAVE\n" );
  
				printf( "Channels    = %i\n", Header->Channels );
				printf( "Samples/S   = %i\n", Header->SampleRate );
				printf( "Bits/Sample = %i\n", Header->nBitsperSample );

				printf( "m_DataSize = %lu\n\n", static_cast<unsigned long>(m_DataSize) );
			}

		}
		else
		{
			Log_Error( "Unsupported WAV file" );
			m_DataSize = 0;
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

