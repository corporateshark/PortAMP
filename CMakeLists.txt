cmake_minimum_required(VERSION 3.0)
project("PortAMP")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(libs/minimp3 EXCLUDE_FROM_ALL)

add_definitions(
	-DMODPLUG_STATIC
	-DFLAC__NO_DLL
)

include_directories(
	.
	src
	src/OpenAL
	libs
	libs/libogg-1.3.2/include
	libs/libvorbis-1.3.5/include
	libs/libmodplug-0.8.8.5/src
	libs/id3v2lib/include
	libs/flac-1.3.1/include
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

if(MSVC)
	add_subdirectory(libs/libogg-1.3.2 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libvorbis-1.3.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libmodplug-0.8.8.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/id3v2lib EXCLUDE_FROM_ALL)
	add_subdirectory(libs/flac-1.3.1 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/monkey-audio EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopus EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopusfile EXCLUDE_FROM_ALL)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	add_definitions(-D_ALLOW_RTCc_IN_STL)
	target_link_libraries(portamp
		minimp3
		libogg
		libvorbis
		libmodplug
		id3v2
		libflac
		monkeyaudio
		libopus
		libopusfile
	)
elseif(APPLE)
	add_subdirectory(libs/libogg-1.3.2 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libvorbis-1.3.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libmodplug-0.8.8.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/id3v2lib EXCLUDE_FROM_ALL)
	add_subdirectory(libs/flac-1.3.1 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/monkey-audio EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopus EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopusfile EXCLUDE_FROM_ALL)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	include(CMakeLists_Apple.txt)
	target_link_libraries(portamp
		minimp3
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
	ADD_FRAMEWORK(OpenAL portamp)
else()
	add_subdirectory(libs/id3v2lib EXCLUDE_FROM_ALL)
	add_subdirectory(libs/monkey-audio EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopus EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopusfile EXCLUDE_FROM_ALL)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	target_link_libraries(portamp
		minimp3
		ogg
		vorbis
		vorbisfile
		modplug
		FLAC
		id3v2
		monkeyaudio
		opus
		opusfile
		pthread
		dl
		openal
	)
	include_directories(/usr/local/include)
endif()

set_property(TARGET minimp3 PROPERTY FOLDER ThirdParty)
set_property(TARGET libogg PROPERTY FOLDER ThirdParty)
set_property(TARGET libvorbis PROPERTY FOLDER ThirdParty)
set_property(TARGET libmodplug PROPERTY FOLDER ThirdParty)
set_property(TARGET libflac PROPERTY FOLDER ThirdParty)
set_property(TARGET id3v2 PROPERTY FOLDER ThirdParty)
set_property(TARGET monkeyaudio PROPERTY FOLDER ThirdParty)
set_property(TARGET libopus PROPERTY FOLDER ThirdParty)
set_property(TARGET libopusfile PROPERTY FOLDER ThirdParty)
