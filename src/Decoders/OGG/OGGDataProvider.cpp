#include <cstring>

#include "OGGDataProvider.h"
#include "Utils.h"

#include <vorbis/vorbisfile.h>

clOGGDataProvider::clOGGDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
, m_VorbisFile()
, m_RawPosition(0)
, m_CurrentSection(0)
{
	ov_callbacks Callbacks;

	Callbacks.read_func  = oggReadFunc;
	Callbacks.seek_func  = oggSeekFunc;
	Callbacks.close_func = oggCloseFunc;
	Callbacks.tell_func  = oggTellFunc;

	// check for "< 0"
	ov_open_callbacks( this, &m_VorbisFile, nullptr, -1, Callbacks );

	vorbis_info* VorbisInfo = ov_info( &m_VorbisFile, -1 );

	if ( VorbisInfo )
	{
		m_Format.m_NumChannels      = VorbisInfo->channels;
		m_Format.m_SamplesPerSecond = VorbisInfo->rate;
		m_Format.m_BitsPerSample    = 16;
	}
}

clOGGDataProvider::~clOGGDataProvider()
{
	ov_clear( &m_VorbisFile );
}

const uint8_t* clOGGDataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clOGGDataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

int clOGGDataProvider::DecodeFromFile( size_t Size, size_t BytesRead )
{
	if ( m_IsEndOfStream )
	{
		return 0;
	}

	return static_cast<int>(
		ov_read(
			&m_VorbisFile,
			reinterpret_cast<char*>( m_DecodingBuffer.data() + BytesRead ),
			Size - BytesRead,
			0, // LITTLE_ENDIAN
			m_Format.m_BitsPerSample >> 3,
			1,
			&m_CurrentSection
			)
	);
}

size_t clOGGDataProvider::StreamWaveData( size_t Size )
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

	while ( BytesRead < Size )
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

	return m_BufferUsed;
}

void clOGGDataProvider::Seek( float Seconds )
{
	m_IsEndOfStream = false;

	ov_time_seek( &m_VorbisFile, Seconds );
}

size_t clOGGDataProvider::oggReadFunc( void* Ptr, size_t Size, size_t NumMemBlocks, void* UserData )
{
	clOGGDataProvider* Provider = static_cast<clOGGDataProvider*>( UserData );

	size_t DataSize = Provider->m_Data ? Provider->m_Data->GetDataSize() : 0;

	size_t BytesRead = DataSize - static_cast<size_t>( Provider->m_RawPosition );
	size_t BytesSize = Size * NumMemBlocks;

	// clamp
	if ( BytesSize < BytesRead )
	{
		BytesRead = BytesSize;
	}

	if ( Provider->m_Data )
	{
		memcpy( Ptr, Provider->m_Data->GetDataPtr() + Provider->m_RawPosition, BytesRead );
	}

	Provider->m_RawPosition += BytesRead;

	return BytesRead;
}

int clOGGDataProvider::oggSeekFunc( void* UserData, ogg_int64_t Offset, int Whence )
{
	clOGGDataProvider* Provider = static_cast<clOGGDataProvider*>( UserData );

	size_t DataSize = Provider->m_Data ? Provider->m_Data->GetDataSize() : 0;

	if ( Whence == SEEK_SET )
	{
		Provider->m_RawPosition = Offset;
	}
	else if ( Whence == SEEK_CUR )
	{
		Provider->m_RawPosition += Offset;
	}
	else if ( Whence == SEEK_END )
	{
		Provider->m_RawPosition = DataSize + Offset;
	}

	// clamp
	if ( Provider->m_RawPosition > (ogg_int64_t)DataSize )
	{
		Provider->m_RawPosition = (ogg_int64_t)DataSize;
	}

	return static_cast<int>( Provider->m_RawPosition );
}

int clOGGDataProvider::oggCloseFunc( void* UserData )
{
	return 0;
}

long clOGGDataProvider::oggTellFunc( void* UserData )
{
	clOGGDataProvider* Provider = static_cast<clOGGDataProvider*>( UserData );

	return static_cast<int>( Provider->m_RawPosition );
}
