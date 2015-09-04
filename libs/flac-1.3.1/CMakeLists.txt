CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libflac)

if(MSVC)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
else()
	ADD_DEFINITIONS(-O2)
	ADD_DEFINITIONS(-Wno-deprecated-writable-strings)
	ADD_DEFINITIONS(-Wno-write-strings)
	ADD_DEFINITIONS(-Wno-unused-result)
endif()

ADD_DEFINITIONS(-DFLAC__NO_DLL -DFLAC__HAS_OGG -DHAVE_STDINT_H)
ADD_DEFINITIONS(-DVERSION="")

if(APPLE)
	ADD_DEFINITIONS(-DHAVE_LROUND)
	ADD_DEFINITIONS(-DHAVE_SYS_PARAM_H)
endif()

SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)

include_directories(
	.
	src
	src/libFLAC
	src/libFLAC/include
	include
)

set(SRC_FILES
	src/libFLAC/bitmath.c
	src/libFLAC/bitreader.c
	src/libFLAC/bitwriter.c
	src/libFLAC/cpu.c
	src/libFLAC/crc.c
	src/libFLAC/fixed.c
	src/libFLAC/fixed_intrin_sse2.c
	src/libFLAC/fixed_intrin_ssse3.c
	src/libFLAC/float.c
	src/libFLAC/format.c
	src/libFLAC/lpc.c
	src/libFLAC/lpc_intrin_avx2.c
	src/libFLAC/lpc_intrin_sse.c
	src/libFLAC/lpc_intrin_sse2.c
	src/libFLAC/lpc_intrin_sse41.c
	src/libFLAC/md5.c
	src/libFLAC/memory.c
	src/libFLAC/metadata_iterators.c
	src/libFLAC/metadata_object.c
	src/libFLAC/ogg_decoder_aspect.c
	src/libFLAC/ogg_encoder_aspect.c
	src/libFLAC/ogg_helper.c
	src/libFLAC/ogg_mapping.c
	src/libFLAC/stream_decoder.c
	src/libFLAC/stream_encoder.c
	src/libFLAC/stream_encoder_framing.c
	src/libFLAC/stream_encoder_intrin_avx2.c
	src/libFLAC/stream_encoder_intrin_sse2.c
	src/libFLAC/stream_encoder_intrin_ssse3.c
	src/libFLAC/window.c
)

if(MSVC)
	set(SRC_FILES
		${SRC_FILES}
		src/share/win_utf8_io/win_utf8_io.c
	)
endif()

set(HEADER_FILES
	include/FLAC/all.h
	include/FLAC/assert.h
	include/FLAC/callback.h
	include/FLAC/export.h
	include/FLAC/format.h
	include/FLAC/metadata.h
	include/FLAC/ordinals.h
	include/FLAC/stream_decoder.h
	include/FLAC/stream_encoder.h
)

add_library(libflac ${SRC_FILES} ${HEADER_FILES}
	)
