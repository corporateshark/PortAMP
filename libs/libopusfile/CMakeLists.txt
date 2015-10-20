CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libopusfile)

if(MSVC)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
else()
	ADD_DEFINITIONS(-O2)
	ADD_DEFINITIONS(-Wno-deprecated-writable-strings)
	ADD_DEFINITIONS(-Wno-write-strings)
	ADD_DEFINITIONS(-Wno-unused-result)
endif()

ADD_DEFINITIONS(-DUSE_ALLOCA=1 -DHAVE_STDINT_H=1 -DOPUS_BUILD -DCUSTOM_MODES=1 -DHAVE_LRINT=1 -DHAVE_LRINTF=1)

if(APPLE)
	ADD_DEFINITIONS(-DHAVE_LROUND)
	ADD_DEFINITIONS(-DHAVE_SYS_PARAM_H)
endif()

SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)

include_directories(
	.
	src
	include
	../libopus/include
)

set(SRC_FILES
	src/http.c
	src/info.c
	src/internal.c
	src/opusfile.c
	src/stream.c
	src/wincerts.c
)

if(MSVC)
	set(SRC_FILES
		${SRC_FILES}
	)
endif()

set(HEADER_FILES
	include/opusfile.h
)

add_library(libopusfile ${SRC_FILES} ${HEADER_FILES}
	)
