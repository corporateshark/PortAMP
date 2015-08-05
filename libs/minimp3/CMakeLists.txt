cmake_minimum_required(VERSION 2.8)

project("minimp3")

if(MSVC)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
endif()

set(SRC_FILES
	minimp3.c
)

set(HEADER_FILES
	libc.h
	minimp3.h
)

add_library(
	minimp3 ${SRC_FILES} ${HEADER_FILES}
)
