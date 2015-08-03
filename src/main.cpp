#include "AudioSubsystem.h"
#include "AudioSubsystem_OpenAL.h"

int main()
{
	auto AudioSubsystem = CreateAudioSubsystem_OpenAL();

	AudioSubsystem->Start();
	AudioSubsystem->Stop();

	return 0;
};
