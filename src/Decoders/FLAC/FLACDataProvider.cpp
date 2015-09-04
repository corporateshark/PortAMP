#include "FLACDataProvider.h"

#include "flac/stream_decoder.h"

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

clFLACDataProvider::clFLACDataProvider( const std::shared_ptr<clBlob>& Data )
: m_StreamData( std::make_shared<sFLACStreamData>() )
{
	FLAC__StreamDecoderInitStatus err = FLAC__stream_decoder_init_stream(
		m_StreamData->m_Decoder,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		m_StreamData.get()
	);
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
