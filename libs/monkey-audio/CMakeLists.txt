CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(monkeyaudio)

if(MSVC)
	set(CMAKE_C_FLAGS "/EHsc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
else()
	ADD_DEFINITIONS(-O2)
endif()

include_directories(
	.
	Assembly
	Shared
)

set(SRC_FILES
	APECompress.cpp
	APECompressCore.cpp
	APECompressCreate.cpp
	APEDecompress.cpp
	APEHeader.cpp
	APEInfo.cpp
	APELink.cpp
	APETag.cpp
	BitArray.cpp
	MACLib.cpp
	MACProgressHelper.cpp
	MD5.cpp
	NewPredictor.cpp
	NNFilter.cpp
	Prepare.cpp
	UnBitArray.cpp
	UnBitArrayBase.cpp
	WAVInputSource.cpp
	Shared/CharacterHelper.cpp
	Shared/CircleBuffer.cpp
	Shared/GlobalFunctions.cpp
	Shared/MACUtils.cpp
	Shared/StdLibFileIO.cpp
	Shared/WinFileIO.cpp
	Assembly/common.cpp
)

set(HEADER_FILES
	APECompress.h
	APECompressCore.h
	APECompressCreate.h
	APEDecompress.h
	APEHeader.h
	APEInfo.h
	APELink.h
	APETag.h
	BitArray.h
	MACLib.h
	MACProgressHelper.h
	MD5.h
	MultichannelNNFilter.h
	NewPredictor.h
	NNFilter.h
	Predictor.h
	Prepare.h
	ScaledFirstOrderFilter.h
	StartFilter.h
	UnBitArray.h
	UnBitArrayBase.h
	WAVInputSource.h
)

add_library(monkeyaudio ${SRC_FILES} ${HEADER_FILES}
)
