#include "MP3DataProvider.h"
#include "Utils.h"

extern "C"
{
#include "minimp3/minimp3.h"
}

clMP3DataProvider::clMP3DataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_StreamPos( 0 )
, m_InitialStreamPos( 0 )
, m_IsEndOfStream( false )
{
	LoadMP3Info();

	m_Format.m_NumChannels      = m_MP3Info.channels;
	m_Format.m_SamplesPerSecond = m_MP3Info.sample_rate;
	m_Format.m_BitsPerSample    = 16;
}

void clMP3DataProvider::LoadMP3Info()
{
	int byteCount = mp3_decode(
		(mp3_decoder_t*)m_MP3Decoder,
		m_Data ? const_cast<uint8_t*>( m_Data->GetDataPtr() ) : nullptr,
		m_Data ? m_Data->GetDataSize() : 0,
		(signed short*)m_DecodingBuffer.data(),
		&m_MP3Info
	);

	m_StreamPos += byteCount;
	m_InitialStreamPos = m_StreamPos;
}

const uint8_t* clMP3DataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clMP3DataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

size_t clMP3DataProvider::StreamWaveData( size_t Size )
{
	return 0;
}

void clMP3DataProvider::Seek( float Seconds )
{
}
