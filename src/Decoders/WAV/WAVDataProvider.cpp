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
	uint16_t cbSize;
};

struct PACKED_STRUCT(1) sWAVChunkHeader
{
	uint8_t  ID[4];	// "data"
	uint32_t Size;
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

void ConvertClamp_Int32ToInt16(const int32_t* Src, int16_t* Dst, size_t NumInts)
{
	const int32_t* End = Src + NumInts;

	while (Src < End)
	{
		int32_t v = *Src++;
		*Dst++ = (int16_t((v >> 16) & 0xFFFF) + (int16_t(v & 0xFFFF) << 16)) & 0xFFFF;
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
		const uint16_t FORMAT_EXT   = 0xFFFE;

		bool IsPCM   = Header->FormatTag == FORMAT_PCM;
		bool IsExtFormat = Header->FormatTag == FORMAT_EXT;
		bool IsFloat = Header->FormatTag == FORMAT_FLOAT;
		bool IsRIFF = memcmp( &Header->RIFF, "RIFF", 4 ) == 0;
		bool IsWAVE = memcmp( &Header->WAVE, "WAVE", 4 ) == 0;

		if ( IsRIFF && IsWAVE && !IsPCM && IsVerbose() )
		{
			printf( "Channels       : %i\n", Header->Channels );
			printf( "Sample rate    : %i\n", Header->SampleRate );
			printf( "Bits per sample: %i\n", Header->nBitsperSample );
			printf( "Format tag     : %x\n", Header->FormatTag );
		}

		// can only handle uncompressed .WAV files
		if ( IsRIFF && IsWAVE && (IsPCM|IsFloat|IsExtFormat) )
		{
			m_Format.m_NumChannels      = Header->Channels;
			m_Format.m_SamplesPerSecond = Header->SampleRate;
			m_Format.m_BitsPerSample    = Header->nBitsperSample;

			const size_t HeaderSize = sizeof(sWAVHeader);
			const size_t ExtraParamSize = IsPCM ? 0 : Header->cbSize;
			const size_t ChunkHeaderSize = sizeof(sWAVChunkHeader);
		
			size_t Offset = HeaderSize + ExtraParamSize;;

			if ( IsPCM ) Offset -= sizeof(Header->cbSize);

			const sWAVChunkHeader* ChunkHeader = reinterpret_cast<const sWAVChunkHeader*>( Data->GetDataPtr() + Offset );

			m_DataSize = ChunkHeader->Size;

 			if ( IsFloat )
			{
				// replace the blob and convert data to 16-bit
				std::vector<uint8_t> NewData;
				NewData.resize( m_Data->GetDataSize() );
				int16_t* Dst = reinterpret_cast<int16_t*>( NewData.data()+sizeof(sWAVHeader) );

				if ( Header->nBitsperSample == 32 )
				{
					const float* Src = reinterpret_cast<const float*>( m_Data->GetDataPtr()+Offset+sizeof(ChunkHeader) );
					ConvertClamp_IEEEToInt16<float>( Src, Dst, m_DataSize / 4 );
					m_DataSize = m_DataSize/2;
				}
				else if ( Header->nBitsperSample == 64 )
				{
					const double* Src = reinterpret_cast<const double*>( m_Data->GetDataPtr()+Offset+sizeof(ChunkHeader)+4);
					ConvertClamp_IEEEToInt16<double>( Src, Dst, m_DataSize / 8 );
					m_DataSize = m_DataSize/4;
				}
				else 
				{
					Log_Error( "Unknown float format in WAV" );
					m_DataSize = 0;
				}

				m_Data = std::make_shared<clBlob>( NewData );
				m_Format.m_BitsPerSample = 16;
			}
			else if ( Header->nBitsperSample == 32 )
			{
				// replace the blob and convert data to 16-bit
				std::vector<uint8_t> NewData;
				NewData.resize(m_Data->GetDataSize());
				int16_t* Dst = reinterpret_cast<int16_t*>(NewData.data() + sizeof(sWAVHeader));

				const int32_t* Src = reinterpret_cast<const int32_t*>(m_Data->GetDataPtr() + Offset + sizeof(ChunkHeader));
				ConvertClamp_Int32ToInt16(Src, Dst, m_DataSize / 4);
				m_DataSize = m_DataSize / 2;

				m_Data = std::make_shared<clBlob>(NewData);
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

