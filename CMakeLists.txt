cmake_minimum_required(VERSION 3.19)
project("PortAMP")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(
	-DMODPLUG_STATIC
	-DFLAC__NO_DLL
	-DNOMINMAX
)

include_directories(
	.
	src
	src/OpenAL
	libs
	libs/libogg/include
	libs/libvorbis-1.3.5/include
	libs/libmodplug-0.8.8.5/src
	libs/id3v2lib/include
	libs/flac-1.3.3/include
	libs/monkey-audio
	libs/libopus/include
	libs/libopusfile/include
)

set(SRC_FILES
	src/main.cpp
	src/AudioSubsystem_OpenAL.cpp
	src/Decoders/iWaveDataProvider.cpp
	src/Decoders/WAV/WAVDataProvider.cpp
	src/Decoders/MP3/MP3DataProvider.cpp
	src/Decoders/OGG/OGGDataProvider.cpp
	src/Decoders/ModPlug/ModPlugDataProvider.cpp
	src/Decoders/FLAC/FLACDataProvider.cpp
	src/Decoders/APE/APEDataProvider.cpp
	src/Decoders/Opus/OpusDataProvider.cpp
	src/Encoders/WAV/WAVDataEncoder.cpp
	src/OpenAL/LAL.cpp
	src/Utils.cpp
)

set(HEADER_FILES
	src/AudioSubsystem.h
	src/AudioSubsystem_OpenAL.h
	src/Decoders/iWaveDataProvider.h
	src/Decoders/WAV/WAVDataProvider.h
	src/Decoders/MP3/MP3DataProvider.h
	src/Decoders/OGG/OGGDataProvider.h
	src/Decoders/ModPlug/ModPlugDataProvider.h
	src/Decoders/FLAC/FLACDataProvider.h
	src/Decoders/APE/APEDataProvider.h
	src/Decoders/Opus/OpusDataProvider.h
	src/Encoders/iWaveDataEncoder.h
	src/Encoders/WAV/WAVDataEncoder.h
	src/OpenAL/LAL.h
	src/Utils.h
)

add_executable(portamp
	${SRC_FILES}
	${HEADER_FILES}
)

set_property(TARGET portamp PROPERTY CXX_STANDARD 14)
set_property(TARGET portamp PROPERTY CXX_STANDARD_REQUIRED ON)

if(MSVC)
	add_subdirectory(libs/libogg EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libvorbis-1.3.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libmodplug-0.8.8.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/id3v2lib EXCLUDE_FROM_ALL)
	add_subdirectory(libs/flac-1.3.3 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/monkey-audio EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopus EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopusfile EXCLUDE_FROM_ALL)
	target_link_libraries(portamp
		libogg
		libvorbis
		libmodplug
		id3v2
		libflac
		monkeyaudio
		libopus
		libopusfile
	)
	set_property(TARGET libogg PROPERTY FOLDER ThirdParty)
	set_property(TARGET libvorbis PROPERTY FOLDER ThirdParty)
	set_property(TARGET libmodplug PROPERTY FOLDER ThirdParty)
	set_property(TARGET libflac PROPERTY FOLDER ThirdParty)
	set_property(TARGET id3v2 PROPERTY FOLDER ThirdParty)
	set_property(TARGET monkeyaudio PROPERTY FOLDER ThirdParty)
	set_property(TARGET libopus PROPERTY FOLDER ThirdParty)
	set_property(TARGET libopusfile PROPERTY FOLDER ThirdParty)
elseif(APPLE)
	add_subdirectory(libs/libogg EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libvorbis-1.3.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libmodplug-0.8.8.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/id3v2lib EXCLUDE_FROM_ALL)
	add_subdirectory(libs/flac-1.3.3 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/monkey-audio EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopus EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopusfile EXCLUDE_FROM_ALL)
	target_link_libraries(portamp
		libogg
		libvorbis
		libmodplug
		libflac
		id3v2
		monkeyaudio
		libopus
		libopusfile
		dl
		pthread
	)
	include_directories(/usr/local/include /opt/local/include)
	find_package(OpenAL REQUIRED)
	target_link_libraries(portamp "${OPENAL_LIBRARY}")
else()
	add_subdirectory(libs/monkey-audio EXCLUDE_FROM_ALL)
	target_link_libraries(portamp
		ogg
		vorbis
		vorbisfile
		modplug
		FLAC
		monkeyaudio
		opus
		opusfile
		pthread
		dl
		openal
	)
	include_directories(/usr/local/include)
endif()
