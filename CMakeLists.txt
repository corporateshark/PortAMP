cmake_minimum_required(VERSION 2.8)
project("PortAMP")

include_directories(
	.
	src
)

set(SRC_FILES
	src/main.cpp
	src/AudioSubsystem_OpenAL.cpp
)

set(HEADER_FILES
	src/AudioSubsystem.h
	src/AudioSubsystem_OpenAL.h
)

add_executable(PortAMP
	${SRC_FILES}
	${HEADER_FILES}
)
