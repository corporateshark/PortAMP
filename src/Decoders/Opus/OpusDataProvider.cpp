#include "OpusDataProvider.h"
#include "Utils.h"

#include <libopus/include/opus.h>

#include <algorithm>

clOpusDataProvider::clOpusDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
//
, m_Position( 0 )
{
	int Error = 0;

	const int NumChannels = 2;
	m_OpusDecoder = opus_decoder_create( 48000, NumChannels, &Error );
	
	m_Format.m_NumChannels      = NumChannels;
	m_Format.m_SamplesPerSecond = 48000;
	m_Format.m_BitsPerSample    = 16;
}

clOpusDataProvider::~clOpusDataProvider()
{
	opus_decoder_destroy( m_OpusDecoder );
}

const uint8_t* clOpusDataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clOpusDataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

int clOpusDataProvider::DecodeFromFile( size_t Size )
{
	if ( m_IsEndOfStream )
	{
		return 0;
	}

	const size_t MaxFrameSize = 960*6;
	const int DataSize = std::min( MaxFrameSize, m_Data->GetDataSize()-m_Position );

	int DecodedSamples = opus_decode(
		m_OpusDecoder,
		m_Data->GetDataPtr() + m_Position,
		DataSize,
		reinterpret_cast<opus_int16*>( m_DecodingBuffer.data() ),
		Size / (2 * sizeof(opus_int16)),
		0
	);

	return DecodedSamples > 0 ? DecodedSamples : 0;
}

size_t clOpusDataProvider::StreamWaveData( size_t Size )
{
	if ( m_IsEndOfStream )
	{
		return 0;
	}

	size_t OldSize = m_DecodingBuffer.size();

	if ( Size > OldSize )
	{
		m_DecodingBuffer.resize( Size, 0 );
	}

	m_BufferUsed = DecodeFromFile( Size );

	if ( m_BufferUsed <= 0 ) m_IsEndOfStream = true;

	return m_BufferUsed;
}

void clOpusDataProvider::Seek( float Seconds )
{
	m_IsEndOfStream = false;

	m_Position = 0;
}
