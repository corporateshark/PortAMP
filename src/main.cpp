#include <chrono>
#include <thread>
#include <cstring>
#include <stdio.h>

#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "Decoders/WAV/WAVDataProvider.h"
#include "Utils.h"

const char* PORTAMP_VERSION = "0.99.1";

sConfig g_Config;

sConfig ReadConfigFromCommandLine( int argc, char* argv[] )
{
	sConfig Cfg;

	for ( int i = 1; i < argc; i++ )
	{
		if ( strstr( argv[i], "--loop" ) == argv[i] ) Cfg.m_Loop = true;
		if ( strstr( argv[i], "--wav-modplug" ) == argv[i] ) Cfg.m_UseModPlugToDecodeWAV = true;
	}

	return Cfg;
}

void PrintBanner()
{
	printf( "PortAMP version %s (%s)\n", PORTAMP_VERSION, __DATE__ " " __TIME__ " via " __COMPILER_VER__ " for " BUILD_OS );
	printf( "Copyright (C) 2015 Sergey Kosarevsky\n" );
	printf( "https://github.com/corporateshark/PortAMP\n" );
	printf( "\n" );
	printf( "portamp <filename> [--loop] [--wav-modplug]\n" );
	printf( "\n" );
}

int main( int argc, char* argv[] )
{
	if ( argc <= 1 )
	{
		PrintBanner();
		return 0;
	}

	g_Config = ReadConfigFromCommandLine( argc, argv );

	const char* FileName = ( argc > 1 ) ? argv[1] : "test.ogg";

	auto AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();

	auto TestBlob = ReadFileAsBlob( FileName );
	auto Provider = CreateWaveDataProvider( FileName, TestBlob );
	auto Source = AudioSubsystem->CreateAudioSource();
	Source->BindDataProvider( Provider );
	Source->SetLooping( g_Config.m_Loop );
	Source->Play();

	while ( Source->IsPlaying() && !IsKeyPressed() )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds(10) );
	};

	Source->Stop();
	Source = nullptr;

	AudioSubsystem->Stop();

	return 0;
};
