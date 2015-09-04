#include <cstring>
#include <algorithm>

#include "FLACDataProvider.h"
#include "Utils.h"

struct sFLACStreamData
{
	sFLACStreamData()
	: m_Decoder( FLAC__stream_decoder_new() )
	, m_Position( 0 )
	, m_TotalSamples( 0 )
	{}

	~sFLACStreamData()
	{
		FLAC__stream_decoder_delete( m_Decoder );
	}

	FLAC__StreamDecoder* m_Decoder;
	int64_t m_Position;
	uint64_t m_TotalSamples;
};

FLAC__StreamDecoderWriteStatus clFLACDataProvider::flacWrite(
	const FLAC__StreamDecoder* Decoder, const FLAC__Frame* Frame, const FLAC__int32* const Buffer[], void* UserData
)
{
	unsigned int NumSamples = Frame->header.blocksize;
	
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );

	int NumChannels    = P->m_Format.m_NumChannels;
	int BytesPerSample = P->m_Format.m_BitsPerSample / 8;
	int NumBytes = NumSamples * NumChannels * BytesPerSample;

	// only 16-bit format is supported
	if ( BytesPerSample != 2 ) return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

	if ( Buffer )
	{
		if ( P->m_BufferUsed + NumBytes > P->m_DecodingBuffer.size() )
		{
			P->m_DecodingBuffer.resize( P->m_BufferUsed + NumBytes );
		}

		// 16-bit only
		int16_t* Target = reinterpret_cast<int16_t*>( P->m_DecodingBuffer.data() + P->m_BufferUsed );
		for ( int i = 0; i != NumSamples; i++ )
		{
			for ( int c = 0; c != NumChannels; c++ )
			{
				*Target++ = ((int32_t*)Buffer[c])[i];
			}
		}
	}

	P->m_BufferUsed += NumBytes;
	
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderReadStatus clFLACDataProvider::flacRead(
	const FLAC__StreamDecoder* Decoder, FLAC__byte Buffer[], size_t* Bytes, void* UserData
)
{
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );

	if ( P->m_StreamData->m_Position >= P->m_Data->GetDataSize() ) return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

	int BytesLeft = P->m_Data->GetDataSize() - P->m_StreamData->m_Position;
	int BytesRead = std::min( static_cast<int>(*Bytes), BytesLeft );

	if ( BytesRead )
	{
		memcpy( Buffer, P->m_Data->GetDataPtr() + P->m_StreamData->m_Position, BytesRead );
	}

	*Bytes = BytesRead;
	P->m_StreamData->m_Position += BytesRead;

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderSeekStatus clFLACDataProvider::flacSeek(
	const FLAC__StreamDecoder* Decoder, FLAC__uint64 Absolute_byte_offset, void* UserData
)
{
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );

	P->m_StreamData->m_Position = Absolute_byte_offset;
	
	return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

FLAC__StreamDecoderTellStatus clFLACDataProvider::flacTell(
	const FLAC__StreamDecoder* Decoder, FLAC__uint64* Absolute_byte_offset, void* UserData
)
{
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );

	*Absolute_byte_offset = P->m_StreamData->m_Position;

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus clFLACDataProvider::flacLength(
	const FLAC__StreamDecoder* Decoder, FLAC__uint64* StreamLength, void* UserData
)
{
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );

	*StreamLength = P->m_Data->GetDataSize();
	
	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool clFLACDataProvider::flacEof(
	const FLAC__StreamDecoder* Decoder, void* UserData
)
{
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );

	return P->m_StreamData->m_Position >= P->m_Data->GetDataSize();
}

void clFLACDataProvider::flacError(
	const FLAC__StreamDecoder* Decoder, FLAC__StreamDecoderErrorStatus Status, void* UserData
)
{
	const char* S = "Unknown error";

	switch ( Status )
	{
	case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC:
		S = "An error in the stream caused the decoder to lose synchronization.";
		break;
	case FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER:
		S = "The decoder encountered a corrupted frame header.";
		break;
	case FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH:
		S = "The frame's data did not match the CRC in the footer.";
		break;
	case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM:
		S = "The decoder encountered reserved fields in use in the stream.";
		break;
	}

	printf( "FLAC error: %s\n", S );
}

void clFLACDataProvider::flacMeta(
	const FLAC__StreamDecoder* Decoder, const FLAC__StreamMetadata* MetaData, void* UserData
)
{
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );

	if ( MetaData->type == FLAC__METADATA_TYPE_STREAMINFO )
	{
		P->m_StreamData->m_TotalSamples = MetaData->data.stream_info.total_samples;
		P->m_Format.m_SamplesPerSecond = MetaData->data.stream_info.sample_rate;
		P->m_Format.m_NumChannels      = MetaData->data.stream_info.channels;
		P->m_Format.m_BitsPerSample    = MetaData->data.stream_info.bits_per_sample;
	}
}

clFLACDataProvider::clFLACDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_StreamData( std::make_shared<sFLACStreamData>() )
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
{
	{
		FLAC__StreamDecoderInitStatus Status = FLAC__stream_decoder_init_stream(
			m_StreamData->m_Decoder,
			&flacRead,
			&flacSeek,
			&flacTell,
			&flacLength,
			&flacEof,
			&flacWrite,
			&flacMeta,
			&flacError,
			this
		);
	}

	FLAC__bool Status = FLAC__stream_decoder_process_until_end_of_metadata( m_StreamData->m_Decoder );

	if ( !Status ) return;

	if ( !FLAC__stream_decoder_process_single( m_StreamData->m_Decoder ) ) return;

	m_Format.m_NumChannels = FLAC__stream_decoder_get_channels( m_StreamData->m_Decoder );
	m_Format.m_SamplesPerSecond = FLAC__stream_decoder_get_sample_rate( m_StreamData->m_Decoder );
	m_Format.m_BitsPerSample = FLAC__stream_decoder_get_bits_per_sample( m_StreamData->m_Decoder );

	int BufSize = FLAC__stream_decoder_get_blocksize( m_StreamData->m_Decoder ) * m_Format.m_BitsPerSample * 8 * m_Format.m_NumChannels;

	// store two blocks
	m_DecodingBuffer.resize( BufSize * 2 );

	int TotalSamples = FLAC__stream_decoder_get_total_samples( m_StreamData->m_Decoder );
}

clFLACDataProvider::~clFLACDataProvider()
{
}

const uint8_t* clFLACDataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clFLACDataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

size_t clFLACDataProvider::StreamWaveData( size_t Size )
{
	m_BufferUsed = 0;

	auto State = FLAC__stream_decoder_get_state( m_StreamData->m_Decoder );

	while( State < FLAC__STREAM_DECODER_END_OF_STREAM && m_BufferUsed < Size )
	{
		FLAC__bool Result = FLAC__stream_decoder_process_single( m_StreamData->m_Decoder );
		State = FLAC__stream_decoder_get_state( m_StreamData->m_Decoder );
	}

	m_IsEndOfStream = State == FLAC__STREAM_DECODER_END_OF_STREAM;

	return m_BufferUsed;
}

void clFLACDataProvider::Seek( float Seconds )
{
	m_IsEndOfStream = false;

	FLAC__stream_decoder_seek_absolute(
		m_StreamData->m_Decoder,
		static_cast<uint64_t>( static_cast<double>(Seconds) * m_Format.m_SamplesPerSecond )
	);
}
