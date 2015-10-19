#include "OpusDataProvider.h"
#include "Utils.h"

#include <libopus/include/opus.h>

clOpusDataProvider::clOpusDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
{
	int Error = 0;

	m_OpusDecoder = opus_decoder_create( 48000, 2, &Error );

	m_Format.m_NumChannels      = 2;
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

//	return ModPlug_Read( m_ModPlugFile, m_DecodingBuffer.data(), Size );
	return 0;
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

//	ModPlug_Seek( m_ModPlugFile, static_cast<int>( Seconds * 1000.0f ) );	
}
