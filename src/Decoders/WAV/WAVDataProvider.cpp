#include  <cstring>
#include <algorithm>

#include "WAVDataProvider.h"
#include "Utils.h"
//#include "aw-ima/aw-ima.h"

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

void ConvertClamp_Int24ToInt16(const uint8_t* Src, int16_t* Dst, size_t NumBytes)
{
	const uint8_t* End = Src + NumBytes;

	while (Src < End)
	{
		uint8_t b0 = *Src++;
		uint8_t b1 = *Src++;
		uint8_t b2 = *Src++;
		int v = (((b2 << 8) | b1) << 8) | b0;

		if (v & 0x800000) v |= ~0xFFFFFF;
		*Dst++ = (v << 8) & 0xFFFF;
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

struct sADPCMDecoderStatus_MS
{
	sADPCMDecoderStatus_MS()
	: m_Predictor( 0 )
	, m_Sample1( 0 )
	, m_Sample2( 0 )
	, m_Coeff1( 0 )
	, m_Coeff2( 0 )
	, m_iDelta( 0 )
	{}

	int m_Predictor;
	int m_Sample1;
	int m_Sample2;
	int m_Coeff1;
	int m_Coeff2;
	int m_iDelta;
};

static const int AdaptationTable[] = {
	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230
};

static const int AdaptCoeff1[] = {
	256, 512, 0, 192, 240, 460, 392
};

static const int AdaptCoeff2[] = {
	0, -256, 0, 64, 0, -208, -232
};

int Clamp( int i, int a, int b )
{
	if ( i < a ) return a;
	if ( i > b ) return b;
	return i;
}

static int16_t ConvertNibble_MSADPCM( sADPCMDecoderStatus_MS* Status, int Nibble )
{
	const int SignedNibble = Nibble - ( Nibble & 0x08 ? 0x10 : 0 );

	int Predictor = ( Status->m_Sample1 * Status->m_Coeff1 + Status->m_Sample2 * Status->m_Coeff2 ) / 256 + 	SignedNibble * Status->m_iDelta;
	Predictor = Clamp( Predictor, -32768, 32767 );

	Status->m_Sample2 = Status->m_Sample1;
	Status->m_Sample1 = Predictor;

	Status->m_iDelta = ( AdaptationTable[ Nibble ] * Status->m_iDelta ) / 256;
	if ( Status->m_iDelta < 16 ) Status->m_iDelta = 16;

	return Predictor;
}

#define GetWord( w ) { \
	w = Src[0] | Src[1] << 8; \
	if ( w & 0x8000 ) w -= 0x010000; \
	Src += 2; }

int16_t* Decode_MSADPCM_Block( const uint8_t* Src, int16_t* Dst, size_t NumBytes, bool IsStereo )
{
	const uint8_t* End = Src + NumBytes;

	sADPCMDecoderStatus_MS StatusLeft;
	sADPCMDecoderStatus_MS StatusRight;

	// read the block header
	const int PredictorL = Clamp( *Src++, 0, 6 );
	const int PredictorR = IsStereo ? Clamp( *Src++, 0, 6 ) : 0;
	GetWord( StatusLeft.m_iDelta );
	if ( IsStereo ) GetWord( StatusRight.m_iDelta );
	StatusLeft.m_Coeff1 = AdaptCoeff1[ PredictorL ];
	StatusLeft.m_Coeff2 = AdaptCoeff2[ PredictorL ];
	StatusRight.m_Coeff1 = AdaptCoeff1[ PredictorR ];
	StatusRight.m_Coeff2 = AdaptCoeff2[ PredictorR ];

	// read initial samples
	GetWord( StatusLeft.m_Sample1 );
	if ( IsStereo ) GetWord( StatusRight.m_Sample1 );
	GetWord( StatusLeft.m_Sample2 );
	if ( IsStereo ) GetWord( StatusRight.m_Sample2 );

	// output initial samples
	*Dst++ = StatusLeft.m_Sample1;
	if ( IsStereo ) *Dst++ = StatusRight.m_Sample1;
	*Dst++ = StatusLeft.m_Sample2;
	if ( IsStereo ) *Dst++ = StatusRight.m_Sample2;

	while ( Src < End )
	{
		*Dst++ = ConvertNibble_MSADPCM( &StatusLeft, ( Src[0] >> 4 ) & 0x0F );
		*Dst++ = ConvertNibble_MSADPCM( IsStereo ? &StatusRight : &StatusLeft, Src[0] & 0x0F );
		Src++;
	}

	return Dst;
}

void ConvertClamp_MSADPCMToInt16( const uint8_t* Src, int16_t* Dst, size_t NumBytes, int BlockAlign, bool IsStereo )
{
	const size_t NumBlocks = NumBytes / BlockAlign;

	for ( size_t i = 0; i != NumBlocks; i++ )
	{
		Dst = Decode_MSADPCM_Block( Src, Dst, BlockAlign, IsStereo );
		Src += BlockAlign;
	}
}

struct sADPCMDecoderStatus_IMA
{
	sADPCMDecoderStatus_IMA()
	: m_Predictor( 0 )
	, m_StepIndex( 0 )
	{}

	int m_Predictor;
	int m_StepIndex;
};

// http://wiki.multimedia.cx/index.php?title=IMA_ADPCM
static int adpcm_index_table[16] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

// http://wiki.multimedia.cx/index.php?title=IMA_ADPCM
int adpcm_step_table[89] =
{
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
	2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

uint16_t ConvertNibble_IMAADPCM( sADPCMDecoderStatus_IMA* Status, int Nibble )
{
	int Diff = adpcm_step_table[ Status->m_StepIndex ] >> 3;
	if ( Nibble & 0x04 ) Diff += adpcm_step_table[ Status->m_StepIndex ];
	if ( Nibble & 0x02 ) Diff += adpcm_step_table[ Status->m_StepIndex ] >> 1;
	if ( Nibble & 0x01 ) Diff += adpcm_step_table[ Status->m_StepIndex ] >> 2;
	if ( Nibble & 0x08 ) Status->m_Predictor -= Diff; else Status->m_Predictor += Diff;
	Status->m_Predictor = Clamp( Status->m_Predictor, -32768, 32767 );
	Status->m_StepIndex = Clamp( Status->m_StepIndex + adpcm_index_table[Nibble], 0, 88 );

	return Status->m_Predictor & 0xFFFF;
}

int16_t* Decode_IMAADPCM_Block( const uint8_t* Src, int16_t* Dst, size_t NumBytes, bool IsStereo )
{
	const uint8_t* End = Src + NumBytes;

	sADPCMDecoderStatus_IMA StatusLeft;
	sADPCMDecoderStatus_IMA StatusRight;

	GetWord( StatusLeft.m_Predictor );
	StatusLeft.m_StepIndex = Clamp( *Src++, 0, 88 );
	Src++; // skip 1 byte
	if ( IsStereo )
	{
		GetWord( StatusRight.m_Predictor );
		StatusRight.m_StepIndex = Clamp( *Src++, 0, 88 );
		Src++; // skip 1 byte
	}

	if ( IsStereo )
	{
		while ( Src < End )
		{
			for ( int i = 0; i < 4; i++ )
			{
				Dst[i * 4 + 0] = ConvertNibble_IMAADPCM( &StatusLeft, Src[i] & 0x0f );
				Dst[i * 4 + 2] = ConvertNibble_IMAADPCM( &StatusLeft, Src[i] >> 4 );
			}
			Src += 4;

			for ( int i = 0; i < 4; i++ )
			{
				Dst[i * 4 + 1] = ConvertNibble_IMAADPCM( &StatusRight, Src[i] & 0x0f );
				Dst[i * 4 + 3] = ConvertNibble_IMAADPCM( &StatusRight, Src[i] >> 4 );
			}
			Src += 4;
			Dst += 16;
		}
	}
	else
	{
		while ( Src < End )
		{
			*Dst++ = ConvertNibble_IMAADPCM( &StatusLeft, *Src & 0x0f );
			*Dst++ = ConvertNibble_IMAADPCM( &StatusLeft, *Src >> 4 );
			Src++;
		}
	}

	return Dst;
}

void ConvertClamp_IMAADPCMToInt16( const uint8_t* Src, int16_t* Dst, size_t NumBytes, int BlockAlign, bool IsStereo )
{
	const size_t NumBlocks = NumBytes / BlockAlign;

	for ( size_t i = 0; i != NumBlocks; i++ )
	{
		Dst = Decode_IMAADPCM_Block( Src, Dst, BlockAlign, IsStereo );
		Src += BlockAlign;
	}
}

int16_t MuLaw_Decode( int8_t N )
{
	uint8_t Sign = 0;
	N = ~N;
	if ( N & 0x80 )
	{
		N &= ~( 1 << 7 );
		Sign = -1;
	}
	const uint16_t Shift = 33;
	uint8_t Position = ( ( N & 0xF0 ) >> 4 ) + 5;
	int16_t Decoded = ( ( 1 << Position ) | ( ( N & 0x0F ) << ( Position - 4 ) ) | ( 1 << ( Position - 5 ) ) ) - Shift;
	return Sign ? -Decoded : Decoded;
}

void ConvertClamp_MuLawToInt16( const uint8_t* Src, int16_t* Dst, size_t NumBytes )
{
	const uint8_t* End = Src + NumBytes;

	while ( Src < End )
	{
		*Dst++ = MuLaw_Decode( *Src++ );
	}
}

int16_t ALaw_Decode( int8_t N )
{
	uint8_t Sign = 0;
	N ^= 0x55;
	if ( N & 0x80 )
	{
		N &= ~( 1 << 7 );
		Sign = -1;
	}
	uint8_t Position = ( ( N & 0xF0 ) >> 4 ) + 4;
	int16_t Decoded = 	( Position != 4 ) ?
		( ( 1 << Position ) | ( ( N & 0x0F ) << ( Position - 4 ) ) | ( 1 << ( Position - 5 ) ) ) :
		( N << 1 ) | 1;

	return !Sign ? -Decoded : Decoded;
}

void ConvertClamp_ALawToInt16( const uint8_t* Src, int16_t* Dst, size_t NumBytes )
{
	const uint8_t* End = Src + NumBytes;

	while ( Src < End )
	{
		*Dst++ = ALaw_Decode( *Src++ );
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
		const uint16_t FORMAT_ADPCM_MS  = 0x0002;
		const uint16_t FORMAT_ADPCM_IMA = 0x0011;
		const uint16_t FORMAT_ALAW  = 0x0006;
		const uint16_t FORMAT_MULAW = 0x0007;

		const bool IsPCM   = Header->FormatTag == FORMAT_PCM;
		const bool IsExtFormat = Header->FormatTag == FORMAT_EXT;
		bool IsFloat = Header->FormatTag == FORMAT_FLOAT;
		const bool IsRIFF = memcmp( &Header->RIFF, "RIFF", 4 ) == 0;
		const bool IsWAVE = memcmp( &Header->WAVE, "WAVE", 4 ) == 0;
		const bool IsADPCM_MS = Header->FormatTag == FORMAT_ADPCM_MS;
		const bool IsADPCM_IMA = Header->FormatTag == FORMAT_ADPCM_IMA;
		bool IsALaw = Header->FormatTag == FORMAT_ALAW;
		bool IsMuLaw = Header->FormatTag == FORMAT_MULAW;

		if ( IsRIFF && IsWAVE && ( !IsPCM || IsADPCM_MS || IsADPCM_IMA || IsALaw || IsMuLaw ) && IsVerbose() )
		{
			printf( "Channels       : %i\n", Header->Channels );
			printf( "Sample rate    : %i\n", Header->SampleRate );
			printf( "Bits per sample: %i\n", Header->nBitsperSample );
			printf( "Format tag     : %x\n", Header->FormatTag );
		}

		const bool IsSupportedCodec = IsPCM || IsFloat || IsExtFormat || IsADPCM_MS || IsADPCM_IMA || IsALaw || IsMuLaw;

		if ( IsRIFF && IsWAVE && IsSupportedCodec )
		{
			m_Format.m_NumChannels      = Header->Channels;
			m_Format.m_SamplesPerSecond = Header->SampleRate;
			m_Format.m_BitsPerSample    = Header->nBitsperSample;

			const size_t HeaderSize = sizeof(sWAVHeader);
			const size_t ExtraParamSize = IsPCM ? 0 : Header->cbSize;
			const size_t ChunkHeaderSize = sizeof(sWAVChunkHeader);

			if ( IsExtFormat )
			{
				// http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/wave.html
				const uint16_t SubFormatTag = *reinterpret_cast<const uint16_t*>(Data->GetDataPtr() + HeaderSize + 6);

				if ( SubFormatTag == FORMAT_PCM ) IsFloat = false;
				if ( SubFormatTag == FORMAT_FLOAT ) IsFloat = true;
				if ( SubFormatTag == FORMAT_ALAW ) IsALaw = true;
				if ( SubFormatTag == FORMAT_MULAW ) IsMuLaw = true;
			}

			size_t Offset = HeaderSize + ExtraParamSize;

			if ( IsPCM )
			{
				// http://soundfile.sapp.org/doc/WaveFormat/
				const int Subchunk1Size = 16;
				Offset += int( Header->SizeFmt ) - Subchunk1Size - sizeof( Header->cbSize );
			}

			const sWAVChunkHeader* ChunkHeader = nullptr;

			for (;;)
			{
				const sWAVChunkHeader* LocalChunkHeader = reinterpret_cast<const sWAVChunkHeader*>( Data->GetDataPtr() + Offset );

				if ( memcmp( LocalChunkHeader->ID, "data", 4 ) == 0 )
				{
					ChunkHeader = LocalChunkHeader;
					break;
				}
				else if (
					memcmp( LocalChunkHeader->ID, "fact", 4 ) == 0 ||
					memcmp( LocalChunkHeader->ID, "LIST", 4 ) == 0 ||
					memcmp( LocalChunkHeader->ID, "PAD ", 4 ) == 0 ||
					memcmp( LocalChunkHeader->ID, "JUNK", 4 ) == 0 ||
					memcmp( LocalChunkHeader->ID, "junk", 4 ) == 0 ||
					memcmp( LocalChunkHeader->ID, "INFO", 4 ) == 0 ||
					memcmp( LocalChunkHeader->ID, "CSET", 4 ) == 0
				)
				{
					Offset += ChunkHeaderSize;
					Offset += LocalChunkHeader->Size;
				}
				else
				{
					printf( "Unknown chunk ID: %c%c%c%c\n", LocalChunkHeader->ID[0], LocalChunkHeader->ID[1], LocalChunkHeader->ID[2], LocalChunkHeader->ID[3] );
					m_DataSize = 0;
					break;
				}
			}

			m_DataSize = ChunkHeader ? ChunkHeader->Size : 0;

			if ( IsALaw )
			{
				std::vector<uint8_t> NewData;
				NewData.resize( m_DataSize * 2 + sizeof(sWAVHeader) );
				int16_t* Dst = reinterpret_cast< int16_t* >( NewData.data() + sizeof(sWAVHeader) );
				const uint8_t* Src = reinterpret_cast< const uint8_t* >( m_Data->GetDataPtr() + Offset + ChunkHeaderSize );
				ConvertClamp_ALawToInt16( Src, Dst, m_DataSize );
				m_Data = std::make_shared<clBlob>( NewData );
				m_Format.m_BitsPerSample = 16;
				m_DataSize = m_DataSize * 2;
			}
			else if ( IsMuLaw )
			{
				std::vector<uint8_t> NewData;
				NewData.resize( m_DataSize * 2 + sizeof(sWAVHeader) );
				int16_t* Dst = reinterpret_cast< int16_t* >( NewData.data() + sizeof(sWAVHeader) );
				const uint8_t* Src = reinterpret_cast< const uint8_t* >( m_Data->GetDataPtr() + Offset + ChunkHeaderSize );
				ConvertClamp_MuLawToInt16( Src, Dst, m_DataSize );
				m_Data = std::make_shared<clBlob>( NewData );
				m_Format.m_BitsPerSample = 16;
				m_DataSize = m_DataSize * 2;
			}
			else if ( IsADPCM_MS )
			{
				std::vector<uint8_t> NewData;
				NewData.resize( m_DataSize * 4 + sizeof(sWAVHeader) );
				int16_t* Dst = reinterpret_cast<int16_t*>( NewData.data() + sizeof(sWAVHeader) );
				const uint8_t* Src = reinterpret_cast<const uint8_t*>( m_Data->GetDataPtr( ) + Offset + ChunkHeaderSize );
				ConvertClamp_MSADPCMToInt16( Src, Dst, m_DataSize, Header->nBlockAlign, Header->Channels == 2 );
				m_Data = std::make_shared<clBlob>( NewData );
				m_Format.m_BitsPerSample = 16;
				m_DataSize = m_DataSize * 4;
			}
			else if ( IsADPCM_IMA )
			{
				std::vector<uint8_t> NewData;
				NewData.resize( m_DataSize * 4 + sizeof(sWAVHeader) );
				int16_t* Dst = reinterpret_cast< int16_t* >( NewData.data() + sizeof(sWAVHeader) );
				const uint8_t* Src = reinterpret_cast< const uint8_t* >( m_Data->GetDataPtr() + Offset + ChunkHeaderSize );
				ConvertClamp_IMAADPCMToInt16( Src, Dst, m_DataSize, Header->nBlockAlign, Header->Channels == 2 );
				m_Data = std::make_shared<clBlob>( NewData );
				m_Format.m_BitsPerSample = 16;
				m_DataSize = m_DataSize * 4;
			}
 			else if ( IsFloat )
			{
				// replace the blob and convert data to 16-bit
				std::vector<uint8_t> NewData;
				NewData.resize( m_Data->GetDataSize() );
				int16_t* Dst = reinterpret_cast<int16_t*>( NewData.data() + sizeof(sWAVHeader) );

				if ( Header->nBitsperSample == 32 )
				{
					const float* Src = reinterpret_cast<const float*>( m_Data->GetDataPtr()+Offset+ChunkHeaderSize );
					ConvertClamp_IEEEToInt16<float>( Src, Dst, m_DataSize / 4 );
					m_DataSize = m_DataSize/2;
				}
				else if ( Header->nBitsperSample == 64 )
				{
					const double* Src = reinterpret_cast<const double*>( m_Data->GetDataPtr()+Offset+ChunkHeaderSize );
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
			else if ( Header->nBitsperSample == 24 )
			{
				// replace the blob and convert data to 16-bit
				std::vector<uint8_t> NewData;
				NewData.resize(m_Data->GetDataSize());
				int16_t* Dst = reinterpret_cast<int16_t*>(NewData.data() + sizeof(sWAVHeader));

				const uint8_t* Src = reinterpret_cast<const uint8_t*>(m_Data->GetDataPtr() + Offset + ChunkHeaderSize - 4);
				ConvertClamp_Int24ToInt16(Src, Dst, m_DataSize);
				m_DataSize = m_DataSize / 3 * 2;

				m_Data = std::make_shared<clBlob>(NewData);
				m_Format.m_BitsPerSample = 16;
			}
			else if ( Header->nBitsperSample == 32 )
			{
				// replace the blob and convert data to 16-bit
				std::vector<uint8_t> NewData;
				NewData.resize(m_Data->GetDataSize());
				int16_t* Dst = reinterpret_cast<int16_t*>(NewData.data() + sizeof(sWAVHeader));

				const int32_t* Src = reinterpret_cast<const int32_t*>(m_Data->GetDataPtr() + Offset + ChunkHeaderSize);
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
				printf( "Format tag  = %x\n", Header->FormatTag );
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

