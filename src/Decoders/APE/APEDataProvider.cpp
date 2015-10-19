#include "APEDataProvider.h"
#include "Utils.h"

#if !defined( NOMINMAX )
#	define NOMINMAX
#endif

#include "Shared/All.h"
#include "Shared/IO.h"
#include "Shared/MACUtils.h"
#include "APEDecompress.h"

#include <algorithm>

class clAPEIO: public CIO
{
public:
	clAPEIO( clAPEDataProvider* Provider, const std::shared_ptr<clBlob>& Data )
	: m_Provider( Provider )
	, m_Data( Data )
	, m_Position( 0 )
	{
	}

	virtual int Open(const wchar_t * pName) override
	{
		return 0;
	}
	virtual int Close() override
	{
		return 0;
	}

	// read / write
	virtual int Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead) override
	{
		if ( pBuffer )
		{
			size_t BytesLeft = m_Data->GetDataSize() - m_Position;
			size_t NumActualBytes = std::min( BytesLeft, static_cast<size_t>( nBytesToRead ) );
			memcpy( pBuffer, m_Data->GetDataPtr()+m_Position, NumActualBytes );
			if ( pBytesRead ) *pBytesRead = NumActualBytes;
			m_Position += NumActualBytes;
		}
		else
		{
			if ( pBytesRead ) *pBytesRead = 0;
		}

		return 0;
	}
	virtual int Write(const void * pBuffer, unsigned int nBytesToWrite, unsigned int * pBytesWritten) override
	{
		return ERROR_IO_WRITE;
	}

	// seek
	virtual int Seek(int nDistance, unsigned int nMoveMode) override
	{
		switch (nMoveMode)
		{
		case FILE_BEGIN:
			m_Position = nDistance;
			return 0;
		case FILE_CURRENT:
			m_Position += nDistance;
			return 0;
		case FILE_END:
			m_Position = m_Data->GetDataSize() - nDistance;
			return 0;
		}
		return -1;
	}

	// creation / destruction
	virtual int Create(const wchar_t * pName) override
	{
		return 0;
	}
	virtual int Delete() override
	{
		return 0;
	}

	// other functions
	virtual int SetEOF() override
	{
		return 0;
	}

	// attributes
	virtual int GetPosition() override
	{
		return static_cast<int>( m_Position );
	}
	virtual int GetSize() override
	{
		return static_cast<int>( m_Data->GetDataSize() );
	}
	virtual int GetName(wchar_t * pBuffer) override
	{
		return 0;
	}

private:
	clAPEDataProvider* m_Provider;
	std::shared_ptr<clBlob> m_Data;
	size_t m_Position;
};

clAPEDataProvider::clAPEDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
//
, m_APEErrorCode( 0 )
, m_APEIO( std::make_shared<clAPEIO>( this, Data ) )
, m_APEBlockSize( 1 )
{
	m_Format.m_NumChannels      = 2;
	m_Format.m_SamplesPerSecond = 44100;
	m_Format.m_BitsPerSample    = 16;

	m_APEDecompress = CreateIAPEDecompressEx( m_APEIO.get(), &m_APEErrorCode );

	if ( m_APEDecompress )
	{
		m_Format.m_NumChannels      = m_APEDecompress->GetInfo( APE_INFO_CHANNELS, 0, 0 );
		m_Format.m_SamplesPerSecond = m_APEDecompress->GetInfo( APE_INFO_SAMPLE_RATE, 0, 0 );
		m_Format.m_BitsPerSample    = m_APEDecompress->GetInfo( APE_INFO_BITS_PER_SAMPLE, 0, 0 );
		m_APEBlockSize = m_APEDecompress->GetInfo( APE_INFO_BLOCK_ALIGN, 0, 0 );
	}
	else
	{
		Log_Error( "Unsupported APE file" );
	}
}

clAPEDataProvider::~clAPEDataProvider()
{
	delete( m_APEDecompress );
}

const uint8_t* clAPEDataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clAPEDataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

int clAPEDataProvider::DecodeFromFile( size_t Size )
{
	if ( m_IsEndOfStream || !m_APEDecompress )
	{
		return 0;
	}

	int BlocksRead = 0;

	if ( m_APEDecompress->GetData(
		const_cast<char*>( reinterpret_cast<const char*>( m_DecodingBuffer.data() ) ), Size / m_APEBlockSize, &BlocksRead ) != ERROR_SUCCESS
	) return 0;

	return BlocksRead * m_APEBlockSize;
}

size_t clAPEDataProvider::StreamWaveData( size_t Size )
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

	m_BufferUsed = DecodeFromFile( Size );

	if ( m_BufferUsed <= 0 ) m_IsEndOfStream = true;

	return m_BufferUsed;
}

void clAPEDataProvider::Seek( float Seconds )
{
	m_IsEndOfStream = false;

//	ModPlug_Seek( m_ModPlugFile, static_cast<int>( Seconds * 1000.0f ) );
}
