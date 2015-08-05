#include <chrono>
#include <thread>

#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "Decoders/WAV/WAVDataProvider.h"
#include "Utils.h"

struct sConfig
{
	sConfig()
	: m_Loop( false )
	{}

	bool m_Loop;
};

sConfig ReadConfigFromCommandLine( int argc, char* argv[] )
{
	sConfig Cfg;

	for ( int i = 1; i < argc; i++ )
	{
		if ( strstr( argv[i], "--loop" ) == argv[i] ) Cfg.m_Loop = true;
	}

	return Cfg;
}

int main( int argc, char* argv[] )
{
	sConfig Config = ReadConfigFromCommandLine( argc, argv );

	const char* FileName = ( argc > 1 ) ? argv[1] : "test.ogg";

	auto AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();

	auto TestBlob = ReadFileAsBlob( FileName );
	auto Provider = CreateWaveDataProvider( FileName, TestBlob );
	auto Source = AudioSubsystem->CreateAudioSource();
	Source->BindDataProvider( Provider );
	Source->SetLooping( Config.m_Loop );
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
