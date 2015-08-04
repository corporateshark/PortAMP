cmake_minimum_required(VERSION 2.8)
project("PortAMP")

add_subdirectory(minimp3 EXCLUDE_FROM_ALL)

include_directories(
	.
	src
	src/OpenAL
)

set(SRC_FILES
	src/main.cpp
	src/AudioSubsystem_OpenAL.cpp
	src/Decoders/WAV/WAVDataProvider.cpp
	src/Decoders/MP3/MP3DataProvider.cpp
	src/OpenAL/LAL.cpp
	src/Utils.cpp
)

set(HEADER_FILES
	src/AudioSubsystem.h
	src/AudioSubsystem_OpenAL.h
	src/Decoders/iWaveDataProvider.h
	src/Decoders/WAV/WAVDataProvider.h
	src/Decoders/MP3/MP3DataProvider.h
	src/OpenAL/LAL.h
	src/Utils.h
)

add_executable(portamp
	${SRC_FILES}
	${HEADER_FILES}
)

if(MSVC)
	target_link_libraries(portamp
		minimp3
	)
elseif(APPLE)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	include(CMakeLists_Apple.txt)
	target_link_libraries(portamp
		minimp3
		dl
		pthread
	)
	include_directories(/usr/local/include /opt/local/include)
	ADD_FRAMEWORK(OpenAL portamp)
else()
	target_link_libraries(portamp
		minimp3
		pthread
		dl
		openal
	)
	include_directories(/usr/local/include)
endif()
