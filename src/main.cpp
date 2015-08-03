#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"
#include "Decoders/WAV/WAVDataProvider.h"
#include "Utils.h"

int main()
{
	auto AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();

	auto TestBlob = ReadFileAsBlob( "test.wav" );
	auto Provider = std::make_shared<clWAVDataProvider>( TestBlob );
	auto Source = AudioSubsystem->CreateAudioSource();
	Source->BindDataProvider( Provider );
	Source->Play();

	while ( Source->IsPlaying() );

	AudioSubsystem->Stop();

	return 0;
};
