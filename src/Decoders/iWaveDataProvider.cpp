#include <cstring>

#include "iWaveDataProvider.h"

#include "WAV/WAVDataProvider.h"
#include "MP3/MP3DataProvider.h"
#include "OGG/OGGDataProvider.h"
#include "ModPlug/ModPlugDataProvider.h"
#include "FLAC/FLACDataProvider.h"
#include "APE/APEDataProvider.h"
#include "Opus/OpusDataProvider.h"
#include "Utils.h"

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

bool IsModule( const char* Ext )
{
	return (
		( strcmpi( Ext, "mod" ) == 0 ) ||
		( strcmpi( Ext, "s3m" ) == 0 ) ||
		( strcmpi( Ext, "xm" ) == 0 ) ||
		( strcmpi( Ext, "it" ) == 0 ) ||
		( strcmpi( Ext, "669" ) == 0 ) ||
		( strcmpi( Ext, "amf" ) == 0 ) ||
		( strcmpi( Ext, "ams" ) == 0 ) ||
		( strcmpi( Ext, "dbm" ) == 0 ) ||
		( strcmpi( Ext, "dmf" ) == 0 ) ||
		( strcmpi( Ext, "dsm" ) == 0 ) ||
		( strcmpi( Ext, "far" ) == 0 ) ||
		( strcmpi( Ext, "mdl" ) == 0 ) ||
		( strcmpi( Ext, "med" ) == 0 ) ||
		( strcmpi( Ext, "mtm" ) == 0 ) ||
		( strcmpi( Ext, "okt" ) == 0 ) ||
		( strcmpi( Ext, "ptm" ) == 0 ) ||
		( strcmpi( Ext, "stm" ) == 0 ) ||
		( strcmpi( Ext, "ult" ) == 0 ) ||
		( strcmpi( Ext, "umx" ) == 0 ) ||
		( strcmpi( Ext, "mt2" ) == 0 ) ||
		( strcmpi( Ext, "psm" ) == 0 ) ||
		// MIDI
		( strcmpi( Ext, "mid" ) == 0 )
		// compressed modules
/*		||
		( strcmpi( Ext, "mdz" ) == 0 ) ||
		( strcmpi( Ext, "s3z" ) == 0 ) ||
		( strcmpi( Ext, "xmz" ) == 0 ) ||
		( strcmpi( Ext, "itz" ) == 0 ) ||
		( strcmpi( Ext, "mdr" ) == 0 ) ||
		( strcmpi( Ext, "s3r" ) == 0 ) ||
		( strcmpi( Ext, "xmr" ) == 0 ) ||
		( strcmpi( Ext, "itr" ) == 0 ) ||
		( strcmpi( Ext, "mdgz" ) == 0 ) ||
		( strcmpi( Ext, "s3gz" ) == 0 ) ||
		( strcmpi( Ext, "xmgz" ) == 0 ) ||
		( strcmpi( Ext, "itgz" ) == 0 )
*/
		);
}

extern sConfig g_Config;

std::shared_ptr<iWaveDataProvider> CreateWaveDataProvider( const char* FileName, const std::shared_ptr<clBlob>& Data )
{
	const char* Ext = GetFileExt( FileName );

	if ( strcmpi( Ext, "mp3" ) == 0 ) return std::make_shared<clMP3DataProvider>( Data );
	if ( strcmpi( Ext, "ogg" ) == 0 ) return std::make_shared<clOGGDataProvider>( Data );
	if ( strcmpi( Ext, "flac" ) == 0 ) return std::make_shared<clFLACDataProvider>( Data );
	if ( strcmpi( Ext, "ape" ) == 0 ) return std::make_shared<clAPEDataProvider>( Data );
	if ( strcmpi( Ext, "opus" ) == 0 ) return std::make_shared<clOpusDataProvider>( Data );
	if ( IsModule( Ext ) ) return std::make_shared<clModPlugDataProvider>( Data );

	auto MP3Blob = TryMP3InsideWAV( Data );

	if ( MP3Blob )
	{
		return std::make_shared<clMP3DataProvider>( MP3Blob );
	}


	if ( g_Config.m_UseModPlugToDecodeWAV )
	{
		if ( strcmpi( Ext, "wav" ) == 0 ) return std::make_shared<clModPlugDataProvider>( Data );
	}
	
	// default
	return std::make_shared<clWAVDataProvider>( Data );
}
