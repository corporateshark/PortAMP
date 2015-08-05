cmake_minimum_required(VERSION 2.8)

project("libogg")

if(MSVC)
	add_definitions(
		-D_CRT_SECURE_NO_WARNINGS
		-DHAVE_CONFIG_H
	)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
endif(MSVC)

include_directories(
	.
	src
)

set(SRC_FILES
	src/bitwise.c
	src/framing.c
)

add_library(libogg ${SRC_FILES}
	)
