cmake_minimum_required(VERSION 2.8)
project("PortAMP")

include_directories(
	.
	src
	src/OpenAL
)

set(SRC_FILES
	src/main.cpp
	src/AudioSubsystem_OpenAL.cpp
	src/Decoders/WAV/WAVDataProvider.cpp
	src/OpenAL/LAL.cpp
	src/Utils.cpp
)

set(HEADER_FILES
	src/AudioSubsystem.h
	src/AudioSubsystem_OpenAL.h
	src/Decoders/iWaveDataProvider.h
	src/Decoders/WAV/WAVDataProvider.h
	src/OpenAL/LAL.h
	src/Utils.h
)

add_executable(portamp
	${SRC_FILES}
	${HEADER_FILES}
)

if(MSVC)
elseif(APPLE)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	include(CMakeLists_Apple.txt)
	target_link_libraries(portamp
		dl
		pthread
	)
	include_directories(/usr/local/include /opt/local/include)
	ADD_FRAMEWORK(OpenAL portamp)
else()
	target_link_libraries(portamp
		pthread
		dl
		openal
	)
	include_directories(/usr/local/include)
endif()
