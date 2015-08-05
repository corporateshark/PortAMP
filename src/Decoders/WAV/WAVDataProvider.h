#pragma once

#include <memory>

#include "Decoders/iWaveDataProvider.h"

class clBlob;

/// a Microsoft WAVE decoder
class clWAVDataProvider: public iWaveDataProvider
{
public:
	explicit clWAVDataProvider( const std::shared_ptr<clBlob>& Data );

	virtual const sWaveDataFormat& GetWaveDataFormat() const override { return m_Format; }

	virtual const uint8_t* GetWaveData() const override;
	virtual size_t GetWaveDataSize() const override;

	virtual size_t StreamWaveData( size_t Size ) override;
	virtual void Seek( float Seconds ) override;

private:
	std::shared_ptr<clBlob> m_Data;
	size_t m_DataSize;
	sWaveDataFormat m_Format;
};

std::shared_ptr<clBlob> TryMP3InsideWAV( const std::shared_ptr<clBlob>& Data );
