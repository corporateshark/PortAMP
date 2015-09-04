#include "FLACDataProvider.h"

struct sFLACStreamData
{
	sFLACStreamData()
	: m_Pointer( nullptr )
	, m_Decoder( FLAC__stream_decoder_new() )
	, m_CurSample( 0 )
	{}

	~sFLACStreamData()
	{
		FLAC__stream_decoder_delete( m_Decoder );
	}

	void* m_Pointer;
	FLAC__StreamDecoder* m_Decoder;
	uint64_t m_CurSample;
};

FLAC__StreamDecoderWriteStatus clFLACDataProvider::flacWrite(
	const FLAC__StreamDecoder* Decoder, const FLAC__Frame* Frame, const FLAC__int32* const Buffer[], void* UserData
)
{
	return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

FLAC__StreamDecoderReadStatus clFLACDataProvider::flacRead(
	const FLAC__StreamDecoder* Decoder, FLAC__byte Buffer[], size_t* Bytes, void* UserData
)
{
	return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
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
	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus clFLACDataProvider::flacLength(
	const FLAC__StreamDecoder* Decoder, FLAC__uint64* StreamLength, void* UserData
)
{
	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool clFLACDataProvider::flacEof(
	const FLAC__StreamDecoder* Decoder, void* UserData
)
{
	clFLACDataProvider* P = reinterpret_cast<clFLACDataProvider*>( UserData );
	return 1;
}

void clFLACDataProvider::flacError(
	const FLAC__StreamDecoder* Decoder, FLAC__StreamDecoderErrorStatus Status, void* UserData
)
{
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
