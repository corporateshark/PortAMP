#include "MP3DataProvider.h"
#include "Utils.h"

#define MINIMP3_IMPLEMENTATION 1
#include "minimp3/minimp3.h"

struct DecoderData
{
	mp3dec_t m_Decoder;
	mp3dec_frame_info_t m_Info;
};

clMP3DataProvider::clMP3DataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer( MINIMP3_MAX_SAMPLES_PER_FRAME * 16 )
, m_BufferUsed( 0 )
, m_StreamPos( 0 )
, m_InitialStreamPos( 0 )
, m_IsEndOfStream( false )
, m_DecoderData(new DecoderData())
{
	mp3dec_init(&m_DecoderData->m_Decoder);

	LoadMP3Info();

	m_Format.m_NumChannels      = m_DecoderData->m_Info.channels;
	m_Format.m_SamplesPerSecond = m_DecoderData->m_Info.hz;
	m_Format.m_BitsPerSample    = 16;
}

clMP3DataProvider::~clMP3DataProvider()
{
	delete(m_DecoderData);
}

void clMP3DataProvider::LoadMP3Info()
{
	int Samples = 0;

	do
	{
		Samples = mp3dec_decode_frame(
			&m_DecoderData->m_Decoder,
			m_Data ? const_cast<uint8_t*>( m_Data->GetDataPtr() + m_StreamPos ) : nullptr,
			m_Data ? m_Data->GetDataSize() - m_StreamPos: 0,
			(signed short*)m_DecodingBuffer.data(),
			&m_DecoderData->m_Info
		);

		m_StreamPos += m_DecoderData->m_Info.frame_bytes;
	}
	while ( Samples == 0 && m_DecoderData->m_Info.frame_bytes > 0 );

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

int clMP3DataProvider::DecodeFromFile( size_t BytesRead )
{
	if ( m_IsEndOfStream || !m_Data )
	{
		return 0;
	}

	int Samples = 0;

	do
	{
		Samples = mp3dec_decode_frame(
			&m_DecoderData->m_Decoder,
			const_cast<uint8_t*>( m_Data->GetDataPtr() + m_StreamPos ),
			m_Data->GetDataSize() - m_StreamPos,
			(signed short*)( m_DecodingBuffer.data() + BytesRead ),
			&m_DecoderData->m_Info
		);
		m_StreamPos += m_DecoderData->m_Info.frame_bytes;
	}
	while ( Samples == 0 && m_DecoderData->m_Info.frame_bytes > 0 );

	m_Format.m_NumChannels      = m_DecoderData->m_Info.channels;
	m_Format.m_SamplesPerSecond = m_DecoderData->m_Info.hz;

	if ( m_StreamPos >= m_Data->GetDataSize() || !Samples )
	{
		m_IsEndOfStream = true;
	}

	return Samples * m_DecoderData->m_Info.channels * 2;
}

size_t clMP3DataProvider::StreamWaveData( size_t Size )
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

	while ( BytesRead + MINIMP3_MAX_SAMPLES_PER_FRAME * 2 < Size )
	{
		int Ret = DecodeFromFile( BytesRead );

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

	return m_BufferUsed;
}

void clMP3DataProvider::Seek( float Seconds )
{
	mp3dec_init(&m_DecoderData->m_Decoder);

	m_StreamPos = 0;
	m_IsEndOfStream = false;

	LoadMP3Info();
}
