#include "OpusDataProvider.h"
#include "Utils.h"

#include <opus.h>
#include <opusfile.h>

#include <algorithm>

const int PacketSize = 960 * 5;

clOpusDataProvider::clOpusDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
{
	int Error = 0;

	m_OpusFile = op_open_memory( Data->GetDataPtr(), Data->GetDataSize(), &Error );

	if (!m_OpusFile)
	{
		Log_Error( "Unsupported OPUS file" );
		return;
	}

	m_Format.m_NumChannels      = 2;
	m_Format.m_SamplesPerSecond = 48000;
	m_Format.m_BitsPerSample    = 16;
}

clOpusDataProvider::~clOpusDataProvider()
{
	op_free( m_OpusFile );
}

const uint8_t* clOpusDataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clOpusDataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

int clOpusDataProvider::DecodeFromFile( size_t Size, size_t BytesRead )
{
	if ( m_IsEndOfStream )
	{
		return 0;
	}

	int NumSamples = op_read_stereo(
		m_OpusFile,
		(opus_int16*)( m_DecodingBuffer.data() + BytesRead ),
		PacketSize / sizeof(opus_int16)
	);

	return NumSamples > 0 ? NumSamples * 2 * sizeof(opus_int16) : 0;
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

	size_t BytesRead = 0;

	while ( BytesRead < Size-PacketSize )
	{
		int Ret = DecodeFromFile( Size, BytesRead );

		if ( Ret > 0 )
		{
			BytesRead += Ret;
		}
		else if ( Ret == 0 )
		{
			m_IsEndOfStream = true;
			break;
		}
		else
		{
			// there is no audio data in this frame, just skip it
		}
	}

	m_BufferUsed = BytesRead;

	if ( m_BufferUsed <= 0 ) m_IsEndOfStream = true;

	return m_BufferUsed;
}

void clOpusDataProvider::Seek( float Seconds )
{
	m_IsEndOfStream = false;

	op_raw_seek( m_OpusFile, 0 );
}
