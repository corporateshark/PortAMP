CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libopus)

if(MSVC)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
else()
	ADD_DEFINITIONS(-O2)
	ADD_DEFINITIONS(-Wno-deprecated-writable-strings)
	ADD_DEFINITIONS(-Wno-write-strings)
	ADD_DEFINITIONS(-Wno-unused-result)
endif()

ADD_DEFINITIONS(-DUSE_ALLOCA -DHAVE_STDINT_H -DOPUS_BUILD)
#ADD_DEFINITIONS(-DVERSION="")

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
	celt
	silk
)

set(SRC_FILES
	src/analysis.c
	src/mlp.c
	src/mlp_data.c
	src/opus.c
	src/opus_compare.c
	src/opus_decoder.c
	src/opus_demo.c
	src/opus_encoder.c
	src/opus_multistream.c
	src/opus_multistream_decoder.c
	src/opus_multistream_encoder.c
	src/repacketizer.c
	src/repacketizer_demo.c
)

if(MSVC)
	set(SRC_FILES
		${SRC_FILES}
#		src/share/win_utf8_io/win_utf8_io.c
	)
endif()

set(HEADER_FILES
	include/opus.h
	include/opus_custom.h
	include/opus_defines.h
	include/opus_multistream.h
	include/opus_types.h
)

add_library(libopus ${SRC_FILES} ${HEADER_FILES}
	)
