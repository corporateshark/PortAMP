#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"
#include "Decoders/iWaveDataProvider.h"

int main()
{
	auto AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();
	AudioSubsystem->Stop();

	return 0;
};
