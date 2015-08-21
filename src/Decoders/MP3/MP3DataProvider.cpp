#include "MP3DataProvider.h"
#include "Utils.h"
#include "id3v2lib.h"

extern "C"
{
#include "minimp3/minimp3.h"
}

clMP3DataProvider::clMP3DataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer( MP3_MAX_SAMPLES_PER_FRAME * 16 )
, m_BufferUsed( 0 )
, m_StreamPos( 0 )
, m_InitialStreamPos( 0 )
, m_IsEndOfStream( false )
, m_MP3Decoder( mp3_create() )
{
	LoadMP3Info();

	m_Format.m_NumChannels      = m_MP3Info.channels;
	m_Format.m_SamplesPerSecond = m_MP3Info.sample_rate;
	m_Format.m_BitsPerSample    = 16;
}

clMP3DataProvider::~clMP3DataProvider()
{
	mp3_done( (mp3_decoder_t*)m_MP3Decoder );
}

void clMP3DataProvider::SkipTags()
{
	ID3v2_header* ID3TagHeader = get_tag_header_with_buffer(
		reinterpret_cast<const char*>( m_Data->GetDataPtr() + m_StreamPos ),
		static_cast<int>( m_Data->GetDataSize() - m_StreamPos )
	);

	if ( ID3TagHeader )
	{
		m_StreamPos += ID3TagHeader->tag_size;
		delete_header( ID3TagHeader );
	}
}

void clMP3DataProvider::LoadMP3Info()
{
	SkipTags();

	int byteCount = mp3_decode(
		(mp3_decoder_t*)m_MP3Decoder,
		m_Data ? const_cast<uint8_t*>( m_Data->GetDataPtr() + m_StreamPos ) : nullptr,
		m_Data ? m_Data->GetDataSize() - m_StreamPos: 0,
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

int clMP3DataProvider::DecodeFromFile( size_t BytesRead )
{
	if ( m_IsEndOfStream || !m_Data )
	{
		return 0;
	}

	size_t ByteCount = mp3_decode(
		(mp3_decoder_t*)m_MP3Decoder,
		const_cast<uint8_t*>( m_Data->GetDataPtr() + m_StreamPos ),
		m_Data->GetDataSize() - m_StreamPos,
		(signed short*)( m_DecodingBuffer.data() + BytesRead ),
		&m_MP3Info
	);

	m_Format.m_NumChannels      = m_MP3Info.channels;
	m_Format.m_SamplesPerSecond = m_MP3Info.sample_rate;

	m_StreamPos += ByteCount;

	if ( m_StreamPos >= m_Data->GetDataSize() || !ByteCount )
	{
		m_IsEndOfStream = true;
	}

	return m_MP3Info.audio_bytes;
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

	while ( BytesRead + MP3_MAX_SAMPLES_PER_FRAME * 2 < Size )
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
	mp3_done( (mp3_decoder_t*)m_MP3Decoder );
	m_MP3Decoder = mp3_create();

	m_StreamPos = 0;
	m_IsEndOfStream = false;

	LoadMP3Info();
}
