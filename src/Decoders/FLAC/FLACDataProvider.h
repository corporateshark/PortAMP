#pragma once

#include <vector>

#include "Decoders/iWaveDataProvider.h"

#include "FLAC/stream_decoder.h"

class clBlob;
struct sFLACStreamData;

/// FLAC decoder
class clFLACDataProvider: public iWaveDataProvider
{
public:
	explicit clFLACDataProvider( const std::shared_ptr<clBlob>& Data );
	virtual ~clFLACDataProvider();

	virtual const sWaveDataFormat& GetWaveDataFormat() const override { return m_Format; }

	virtual const uint8_t* GetWaveData() const override;
	virtual size_t GetWaveDataSize() const override;

	virtual size_t StreamWaveData( size_t Size ) override;
	virtual bool IsStreaming() const override { return true; }
	virtual bool IsEndOfStream() const override { return m_IsEndOfStream; }
	virtual void Seek( float Seconds ) override;

private:
	int DecodeFromFile( size_t Size );

	static FLAC__StreamDecoderWriteStatus flacWrite( const FLAC__StreamDecoder* Decoder, const FLAC__Frame* Frame, const FLAC__int32* const Buffer[], void* UserData );
	static FLAC__StreamDecoderReadStatus flacRead( const FLAC__StreamDecoder* Decoder, FLAC__byte Buffer[], size_t* Bytes, void* UserData );
	static FLAC__StreamDecoderSeekStatus flacSeek( const FLAC__StreamDecoder* Decoder, FLAC__uint64 Absolute_byte_offset, void* UserData );
	static FLAC__StreamDecoderTellStatus flacTell( const FLAC__StreamDecoder* Decoder, FLAC__uint64* Absolute_byte_offset, void* UserData );
	static FLAC__StreamDecoderLengthStatus flacLength( const FLAC__StreamDecoder* Decoder, FLAC__uint64* StreamLength, void* UserData );
	static FLAC__bool flacEof(const FLAC__StreamDecoder* Decoder, void* UserData );
	static void flacError( const FLAC__StreamDecoder* Decoder, FLAC__StreamDecoderErrorStatus Status, void* UserData );
	static void flacMeta( const FLAC__StreamDecoder* Decoder, const FLAC__StreamMetadata* MetaData, void* UserData );

private:
	std::shared_ptr<clBlob> m_Data;
	sWaveDataFormat m_Format;

	std::vector<uint8_t> m_DecodingBuffer;
	size_t m_BufferUsed;

	bool m_IsEndOfStream;

	std::shared_ptr<sFLACStreamData> m_StreamData;
};
