#include <algorithm>

#include "FLACDataProvider.h"
#include "Utils.h"

struct sFLACStreamData
{
	sFLACStreamData()
	: m_Decoder( FLAC__stream_decoder_new() )
	, m_CurSample( 0 )
	, m_Position( 0 )
	{}

	~sFLACStreamData()
	{
		FLAC__stream_decoder_delete( m_Decoder );
	}

	FLAC__StreamDecoder* m_Decoder;
	uint64_t m_CurSample;
	uint64_t m_Position;
};

FLAC__StreamDecoderWriteStatus clFLACDataProvider::flacWrite(
	const FLAC__StreamDecoder* Decoder, const FLAC__Frame* Frame, const FLAC__int32* const Buffer[], void* UserData
)
{
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

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderSeekStatus clFLACDataProvider::flacSeek(
	const FLAC__StreamDecoder* Decoder, FLAC__uint64 Absolute_byte_offset, void* UserData
)
{
	return FLAC__STREAM_DECODER_SEEK_STATUS_UNSUPPORTED;
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
}

clFLACDataProvider::clFLACDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_StreamData( std::make_shared<sFLACStreamData>() )
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
	return nullptr;
}

size_t clFLACDataProvider::GetWaveDataSize() const
{
	return 0;
}

size_t clFLACDataProvider::StreamWaveData( size_t Size )
{
	return 0;
}

void clFLACDataProvider::Seek( float Seconds )
{
}
