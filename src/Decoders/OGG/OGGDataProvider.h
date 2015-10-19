#pragma once

#include <memory>
#include <vector>

#include "Decoders/iWaveDataProvider.h"

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

class clBlob;

/// OGG Vorbis decoder
class clOGGDataProvider: public iWaveDataProvider
{
public:
	explicit clOGGDataProvider( const std::shared_ptr<clBlob>& Data );
	virtual ~clOGGDataProvider();

	virtual const sWaveDataFormat& GetWaveDataFormat() const override { return m_Format; }

	virtual const uint8_t* GetWaveData() const override;
	virtual size_t GetWaveDataSize() const override;

	virtual size_t StreamWaveData( size_t Size ) override;
	virtual bool IsStreaming() const override { return true; }
	virtual bool IsEndOfStream() const override { return m_IsEndOfStream; }
	virtual void Seek( float Seconds ) override;

private:
	int DecodeFromFile( size_t Size, size_t BytesRead );

	static size_t oggReadFunc( void* Ptr, size_t Size, size_t NumMemBlocks, void* UserData );
	static int oggSeekFunc( void* UserData, ogg_int64_t Offset, int Whence );
	static int oggCloseFunc( void* UserData );
	static long oggTellFunc( void* UserData );

private:
	std::shared_ptr<clBlob> m_Data;
	sWaveDataFormat m_Format;

	std::vector<uint8_t> m_DecodingBuffer;
	size_t m_BufferUsed;

	bool m_IsEndOfStream;

	// Vorbis stuff
	OggVorbis_File m_VorbisFile;
	ogg_int64_t m_RawPosition;
	int m_CurrentSection;
};
