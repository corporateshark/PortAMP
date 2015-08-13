#include "ModPlugDataProvider.h"
#include "Utils.h"

#include <modplug.h>

static bool g_ModPlugLoaded = false;

static void LoadModPlug()
{
	if ( g_ModPlugLoaded ) return;

	ModPlug_Settings Settings;
	ModPlug_GetSettings( &Settings );

	// all "basic settings" are set before ModPlug_Load
	Settings.mResamplingMode = MODPLUG_RESAMPLE_FIR;
	Settings.mChannels = 2;
	Settings.mBits = 16;
	Settings.mFrequency = 44100;
	Settings.mStereoSeparation = 128;
	Settings.mMaxMixChannels = 256;
	Settings.mLoopCount = -1;

	ModPlug_SetSettings( &Settings );

	g_ModPlugLoaded = true;
}

clModPlugDataProvider::clModPlugDataProvider( const std::shared_ptr<clBlob>& Data )
: m_Data( Data )
, m_Format()
, m_DecodingBuffer()
, m_BufferUsed( 0 )
, m_IsEndOfStream( false )
{
	LoadModPlug();

	m_Format.m_NumChannels      = 2;
	m_Format.m_SamplesPerSecond = 44100;
	m_Format.m_BitsPerSample    = 16;

	m_ModPlugFile = ModPlug_Load( Data->GetDataPtr(), static_cast<int>( Data->GetDataSize() ) );
}

clModPlugDataProvider::~clModPlugDataProvider()
{
	ModPlug_Unload( m_ModPlugFile );
}

const uint8_t* clModPlugDataProvider::GetWaveData() const
{
	return m_DecodingBuffer.data();
}

size_t clModPlugDataProvider::GetWaveDataSize() const
{
	return m_BufferUsed;
}

int clModPlugDataProvider::DecodeFromFile( size_t Size )
{
	if ( m_IsEndOfStream )
	{
		return 0;
	}

	return ModPlug_Read( m_ModPlugFile, m_DecodingBuffer.data(), Size );
}

size_t clModPlugDataProvider::StreamWaveData( size_t Size )
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

void clModPlugDataProvider::Seek( float Seconds )
{
	m_IsEndOfStream = false;

	ModPlug_Seek( m_ModPlugFile, static_cast<int>( Seconds * 1000.0f ) );	
}
