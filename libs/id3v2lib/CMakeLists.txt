CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(id3v2lib)

if(MSVC)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
else()
	ADD_DEFINITIONS(-O2)
	ADD_DEFINITIONS(-Wno-deprecated-writable-strings)
	ADD_DEFINITIONS(-Wno-write-strings)
	ADD_DEFINITIONS(-Wno-unused-result)
endif()

SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)

ADD_SUBDIRECTORY(src)
