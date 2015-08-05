cmake_minimum_required(VERSION 2.8)

project("libvorbis")

if(MSVC)
	add_definitions(
		-D_CRT_SECURE_NO_WARNINGS
	)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
endif(MSVC)

include_directories(
	.
	src
)

set(SRC_FILES
	lib/analysis.c
	lib/bitrate.c
	lib/block.c
	lib/codebook.c
	lib/envelope.c
	lib/floor0.c
	lib/floor1.c
	lib/info.c
	lib/lookup.c
	lib/lpc.c
	lib/lsp.c
	lib/mapping0.c
	lib/mdct.c
	lib/psy.c
	lib/registry.c
	lib/res0.c
	lib/sharedbook.c
	lib/smallft.c
	lib/synthesis.c
	lib/vorbisfile.c
	lib/window.c
)

add_library(libvorbis ${SRC_FILES}
	)
