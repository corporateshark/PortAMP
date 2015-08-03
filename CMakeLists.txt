cmake_minimum_required(VERSION 2.8)
project("PortAMP")

include_directories(
	.
	src
)

set(SRC_FILES
	src/main.cpp
)

set(HEADER_FILES
)

add_executable(PortAMP
	${SRC_FILES}
	${HEADER_FILES}
)
