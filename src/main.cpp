#include <chrono>
#include <thread>

#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "Decoders/WAV/WAVDataProvider.h"
#include "Utils.h"

int main( int argc, char* argv[] )
{
	const char* FileName = ( argc > 1 ) ? argv[1] : "test.wav";

	auto AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();

	auto TestBlob = ReadFileAsBlob( FileName );
	auto Provider = CreateWaveDataProvider( FileName, TestBlob );
	auto Source = AudioSubsystem->CreateAudioSource();
	Source->BindDataProvider( Provider );
	Source->Play();

	while ( Source->IsPlaying() && !IsKeyPressed() );

	std::this_thread::sleep_for( std::chrono::milliseconds(300) );

	Source = nullptr;

	AudioSubsystem->Stop();

	return 0;
};
