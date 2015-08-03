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
	src/OpenAL/LAL.cpp
)

set(HEADER_FILES
	src/AudioSubsystem.h
	src/AudioSubsystem_OpenAL.h
	src/Decoders/iWaveDataProvider.h
	src/OpenAL/LAL.h
)

add_executable(PortAMP
	${SRC_FILES}
	${HEADER_FILES}
)

if(MSVC)
elseif(APPLE)
	include(CMakeLists_Apple.txt)
	target_link_libraries(PortAMP
		dl
		pthread
	)
	include_directories(/usr/local/include /opt/local/include)
	ADD_FRAMEWORK(OpenAL PortAMP)
else()
	target_link_libraries(PortAMP
		pthread
		dl
		openal
	)
	include_directories(/usr/local/include)
endif()
