#include <chrono>
#include <thread>
#include <cstring>
#include <stdio.h>

#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "Decoders/WAV/WAVDataProvider.h"
#include "Encoders/iWaveDataEncoder.h"
#include "Utils.h"
#include "Playlist.h"

#define ENABLE_TEST	0

const char* PORTAMP_VERSION = "1.1.2";

sConfig g_Config;
clPlaylist g_Playlist;

sConfig ReadConfigFromCommandLine( int argc, char* argv[] )
{
	sConfig Cfg;

	int i = 1;

	while ( i < argc )
	{
		if ( strstr( argv[i], "--loop" ) == argv[i] ) Cfg.m_Loop = true;
		else if ( strstr( argv[i], "--wav-modplug" ) == argv[i] ) Cfg.m_UseModPlugToDecodeWAV = true;
		else if ( strstr( argv[i], "--verbose" ) == argv[i] ) Cfg.m_Verbose = true;
		else if ( strstr (argv[i], "--output-file" ) == argv[i] )
		{
			if ( i+1 < argc )
			{
				Cfg.m_OutputFile = argv[++i];
			}
			else
			{
				printf( "Expected output file name for --output-file\n" );
				exit(0);
			}
		}
		else g_Playlist.EnqueueTrack( argv[i] );
		i++;
	}

	return Cfg;
}

void PrintBanner()
{
	printf( "PortAMP version %s (%s)\n", PORTAMP_VERSION, __DATE__ " " __TIME__ " via " __COMPILER_VER__ " for " BUILD_OS );
	printf( "Copyright (C) 2015-2016 Sergey Kosarevsky\n" );
	printf( "portamp@linderdaum.com\n" );
	printf( "https://github.com/corporateshark/PortAMP\n" );
	printf( "\n" );
	printf( "portamp <filename1> [<filename2> ...] [--loop] [--wav-modplug] [--verbose]\n" );
	printf( "\n" );
}

int main( int argc, char* argv[] )
{
#if !ENABLE_TEST
	if ( argc <= 1 )
	{
		PrintBanner();
		return 0;
	}
#endif

	g_Config = ReadConfigFromCommandLine( argc, argv );

#if ENABLE_TEST
	if ( g_Playlist.IsEmpty() ) g_Playlist.EnqueueTrack( "test.ogg" );
	g_Config.m_Verbose = true;
#endif

	auto AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();

	auto Source = AudioSubsystem->CreateAudioSource();
	// allow seamless looping if there is only one track
	if ( g_Playlist.GetNumTracks() == 1 ) Source->SetLooping( g_Config.m_Loop );

	bool RequestingExit = false;

	while ( !g_Playlist.IsEmpty() && !RequestingExit )
	{
		auto FileName = g_Playlist.GetAndPopNextTrack( g_Config.m_Loop );
		auto DataBlob = ReadFileAsBlob( FileName.c_str() );
		if (!DataBlob || !DataBlob->GetDataSize()) continue;

		auto Provider = CreateWaveDataProvider( FileName.c_str(), DataBlob );
		if (!Provider) continue;
		
		Source->BindDataProvider( Provider );
		Source->Play();

		while ( Source->IsPlaying() && !RequestingExit )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds(10) );

			if ( IsKeyPressed() ) RequestingExit = true;
		};

		Source->Stop();
	}

	Source = nullptr;

	AudioSubsystem->Stop();

	return 0;
};
