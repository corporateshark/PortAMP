#include <chrono>
#include <thread>
#include <cstring>
#include <fstream>
#include <stdio.h>

#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "Decoders/WAV/WAVDataProvider.h"
#include "Encoders/iWaveDataEncoder.h"
#include "Encoders/WAV/WAVDataEncoder.h"
#include "Utils.h"
#include "Playlist.h"

#define ENABLE_TEST	0

const char* PORTAMP_VERSION = "1.2.3";

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
		else if (strstr(argv[i], "--gain") == argv[i])
		{
			if (i + 1 < argc)
			{
				Cfg.m_Gain = strtof(argv[++i], nullptr);
			}
			else
			{
				printf("Expected gain level [float] for --volume\n");
				exit(0);
			}
		}
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
	printf( "Copyright (C) 2015-2025 Sergey Kosarevsky\n" );
	printf( "portamp@linderdaum.com\n" );
	printf( "https://github.com/corporateshark/PortAMP\n" );
	printf( "\n" );
	printf( "portamp <filename1> [<filename2> ...] [--loop] [--wav-modplug] [--verbose] [--gain <float>] [--output-file <filename.wav>]\n" );
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

	std::shared_ptr<iAudioSubsystem> AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();

	if (g_Config.m_Gain > 0) {
		AudioSubsystem->SetListenerGain(g_Config.m_Gain);
	}

	std::shared_ptr<iAudioSource> Source = AudioSubsystem->CreateAudioSource();

	// allow seamless looping if there is only one track
	if ( g_Playlist.GetNumTracks() == 1 ) Source->SetLooping( g_Config.m_Loop );

	bool RequestingExit = false;

	std::shared_ptr<iWaveDataEncoder> Encoder = g_Config.m_OutputFile.empty() ? nullptr : std::make_shared<clWAVDataEncoder>();

	while ( !g_Playlist.IsEmpty() && !RequestingExit )
	{
		auto FileName = g_Playlist.GetAndPopNextTrack( g_Config.m_Loop );
		auto DataBlob = ReadFileAsBlob( FileName.c_str() );
		if (!DataBlob || !DataBlob->GetDataSize())
		{
			printf("Cannot read file %s\n", FileName.c_str());
			continue;
		}

		auto Provider = CreateWaveDataProvider( FileName.c_str(), DataBlob );
		if (!Provider)
		{
			printf("Cannot parse file %s\n", FileName.c_str());
			continue;
		}

		if (Encoder)
		{
			const auto Format = Provider->GetWaveDataFormat();
			Encoder->ResetEncoder(Format.m_NumChannels, Format.m_SamplesPerSecond, Format.m_BitsPerSample, 1.0f);
			printf("Converting %s to %s...\n", FileName.c_str(), g_Config.m_OutputFile.c_str());
			if (Provider->IsStreaming())
			{
				while (Provider->StreamWaveData(65535))
				{
					Encoder->EncodePCMData(Provider->GetWaveData(), Provider->GetWaveDataSize());
				}
			}
			else
			{
				Encoder->EncodePCMData(Provider->GetWaveData(), Provider->GetWaveDataSize());
			}
			const auto Result = Encoder->FinalizeAndGetResult();
			std::ofstream OutFile(g_Config.m_OutputFile, std::ios::out | std::ofstream::binary);
			std::copy(Result.begin(), Result.end(), std::ostreambuf_iterator<char>(OutFile));
			break;
		}
		
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
