#include "MP3DataProvider.h"

#include "minimp3/minimp3.h"

clMP3DataProvider::clMP3DataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
{
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
