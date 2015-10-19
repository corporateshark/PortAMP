#pragma once

#include <memory>
#include <vector>

#include "Decoders/iWaveDataProvider.h"

extern "C"
{
#include "minimp3/minimp3.h"
}

class clBlob;

/// MPEG Layer-III decoder
class clMP3DataProvider: public iWaveDataProvider
{
public:
	explicit clMP3DataProvider( const std::shared_ptr<clBlob>& Data );
	virtual ~clMP3DataProvider();

	virtual const sWaveDataFormat& GetWaveDataFormat() const override { return m_Format; }

	virtual const uint8_t* GetWaveData() const override;
	virtual size_t GetWaveDataSize() const override;

	virtual size_t StreamWaveData( size_t Size ) override;
	virtual bool IsStreaming() const override { return true; }
	virtual bool IsEndOfStream() const override { return m_IsEndOfStream; }
	virtual void Seek( float Seconds ) override;

private:
	int DecodeFromFile( size_t BytesRead );
	void LoadMP3Info();
	void SkipTags();

private:
	std::shared_ptr<clBlob> m_Data;
	sWaveDataFormat m_Format;

	std::vector<uint8_t> m_DecodingBuffer;
	size_t m_BufferUsed;

	size_t m_StreamPos;
	size_t m_InitialStreamPos;
	bool m_IsEndOfStream;

	// minimp3 stuff
	mp3_decoder_t m_MP3Decoder;
	mp3_info_t m_MP3Info;
};
