#include "iWaveDataProvider.h"

#include "WAV/WAVDataProvider.h"
#include "MP3/MP3DataProvider.h"

const char* GetFileExt( const char* FileName )
{
	const char* Dot = strrchr( FileName, '.' );
	if ( !Dot || Dot == FileName ) return "";
	return Dot + 1;
}

#if defined(_MSC_VER)
#	define strcmpi _strcmpi
#else
#	define strcmpi strcasecmp
#endif

std::shared_ptr<iWaveDataProvider> CreateWaveDataProvider( const char* FileName, const std::shared_ptr<clBlob>& Data )
{
	const char* Ext = GetFileExt( FileName );

	if ( strcmpi( Ext, "mp3" ) == 0 ) return std::make_shared<clMP3DataProvider>( Data );

	auto MP3Blob = TryMP3InsideWAV( Data );

	if ( MP3Blob )
	{
		return std::make_shared<clMP3DataProvider>( MP3Blob );
	}
	
	// default
	return std::make_shared<clWAVDataProvider>( Data );
}
