#include "APEDataProvider.h"
#include "Utils.h"

#include "Shared/All.h"
#include "Shared/IO.h"
#include "Shared/MACUtils.h"
#include "APEDecompress.h"

clAPEDataProvider::clAPEDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
//
, m_APEErrorCode( 0 )
{
	m_Format.m_NumChannels      = 2;
	m_Format.m_SamplesPerSecond = 44100;
	m_Format.m_BitsPerSample    = 16;

	m_APEDecompress = std::shared_ptr<IAPEDecompress>( CreateIAPEDecompressEx( m_APEIO.get(), &m_APEErrorCode ) );
}

clAPEDataProvider::~clAPEDataProvider()
{
}

const uint8_t* clAPEDataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clAPEDataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

int clAPEDataProvider::DecodeFromFile( size_t Size )
{
	if ( m_IsEndOfStream )
	{
		return 0;
	}

//	return ModPlug_Read( m_ModPlugFile, m_DecodingBuffer.data(), Size );
	return 0;
}

size_t clAPEDataProvider::StreamWaveData( size_t Size )
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

void clAPEDataProvider::Seek( float Seconds )
{
	m_IsEndOfStream = false;

//	ModPlug_Seek( m_ModPlugFile, static_cast<int>( Seconds * 1000.0f ) );
}
