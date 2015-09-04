cmake_minimum_required(VERSION 2.8)
project("PortAMP")

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
)

set(SRC_FILES
	src/main.cpp
	src/AudioSubsystem_OpenAL.cpp
	src/Decoders/iWaveDataProvider.cpp
	src/Decoders/WAV/WAVDataProvider.cpp
	src/Decoders/MP3/MP3DataProvider.cpp
	src/Decoders/OGG/OGGDataProvider.cpp
	src/Decoders/ModPlug/ModPlugDataProvider.cpp
	src/Decoders/FLAC/FlacDataProvider.cpp
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
	src/Decoders/FLAC/FlacDataProvider.h
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
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	target_link_libraries(portamp
		minimp3
		libogg
		libvorbis
		libmodplug
		id3v2
		libflac
	)
elseif(APPLE)
	add_subdirectory(libs/libogg-1.3.2 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libvorbis-1.3.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libmodplug-0.8.8.5 EXCLUDE_FROM_ALL)
	add_subdirectory(libs/id3v2lib EXCLUDE_FROM_ALL)
	add_subdirectory(libs/flac-1.3.1 EXCLUDE_FROM_ALL)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	include(CMakeLists_Apple.txt)
	target_link_libraries(portamp
		minimp3
		libogg
		libvorbis
		libmodplug
		libflac
		id3v2
		dl
		pthread
	)
	include_directories(/usr/local/include /opt/local/include)
	ADD_FRAMEWORK(OpenAL portamp)
else()
	add_subdirectory(libs/id3v2lib EXCLUDE_FROM_ALL)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	target_link_libraries(portamp
		minimp3
		ogg
		vorbis
		vorbisfile
		modplug
		flac
		id3v2
		pthread
		dl
		openal
	)
	include_directories(/usr/local/include)
endif()
