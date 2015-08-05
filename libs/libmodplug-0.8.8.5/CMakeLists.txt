cmake_minimum_required(VERSION 2.8)

project("libmodplug")

if(MSVC)
	add_definitions(
		-D_CRT_SECURE_NO_WARNINGS
		-DMODPLUG_BUILD
		-DMODPLUG_STATIC
		-DMODPLUG_NO_FILESAVE
	)
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
endif(MSVC)

include_directories(
	.
	src
	src/libmodplug
)

if(APPLE)
	add_definitions(
		-DMODPLUG_BUILD
		-DMODPLUG_STATIC
		-DHAVE_SETENV
		-DHAVE_SINF
	)
endif(APPLE)

set(SRC_FILES
	src/fastmix.cpp
	src/load_abc.cpp
	src/load_669.cpp
	src/load_amf.cpp
	src/load_ams.cpp
	src/load_dbm.cpp
	src/load_dmf.cpp
	src/load_dsm.cpp
	src/load_far.cpp
	src/load_it.cpp
	src/load_j2b.cpp
	src/load_mdl.cpp
	src/load_med.cpp
	src/load_mid.cpp
	src/load_mod.cpp
	src/load_mt2.cpp
	src/load_mtm.cpp
	src/load_okt.cpp
	src/load_pat.cpp
	src/load_psm.cpp
	src/load_ptm.cpp
	src/load_s3m.cpp
	src/load_stm.cpp
	src/load_ult.cpp
	src/load_umx.cpp
	src/load_wav.cpp
	src/load_xm.cpp
	src/mmcmp.cpp
	src/modplug.cpp
	src/snd_dsp.cpp
	src/snd_flt.cpp
	src/snd_fx.cpp
	src/sndfile.cpp
	src/sndmix.cpp
)

set(HEADER_FILES
	src/load_pat.h
	src/modplug.h
	src/tables.h
)

add_library(libmodplug ${SRC_FILES} ${HEADER_FILES}
	)
