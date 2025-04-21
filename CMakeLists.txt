cmake_minimum_required(VERSION 3.19)
project("PortAMP")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(
	-DMODPLUG_STATIC
	-DFLAC__NO_DLL
	-DNOMINMAX
)

include_directories(
	.
	src
	src/OpenAL
	libs
	libs/libogg/include
	libs/libmodplug/src
	libs/flac/include
	libs/monkey-audio
   libs/libopus/include
   libs/libopusfile/include
)

set(SRC_FILES
	src/main.cpp
	src/AudioSubsystem_OpenAL.cpp
	src/Decoders/iWaveDataProvider.cpp
	src/Decoders/WAV/WAVDataProvider.cpp
	src/Decoders/MP3/MP3DataProvider.cpp
	src/Decoders/OGG/OGGDataProvider.cpp
	src/Decoders/ModPlug/ModPlugDataProvider.cpp
	src/Decoders/FLAC/FLACDataProvider.cpp
	src/Decoders/APE/APEDataProvider.cpp
	src/Decoders/Opus/OpusDataProvider.cpp
	src/Encoders/WAV/WAVDataEncoder.cpp
	src/OpenAL/LAL.cpp
	src/Utils.cpp
)

set(HEADER_FILES
	src/AudioSubsystem.h
	src/AudioSubsystem_OpenAL.h
	src/Decoders/iWaveDataProvider.h
	src/Decoders/WAV/WAVDataProvider.h
	src/Decoders/MP3/MP3DataProvider.h
	src/Decoders/OGG/OGGDataProvider.h
	src/Decoders/ModPlug/ModPlugDataProvider.h
	src/Decoders/FLAC/FLACDataProvider.h
	src/Decoders/APE/APEDataProvider.h
	src/Decoders/Opus/OpusDataProvider.h
	src/Encoders/iWaveDataEncoder.h
	src/Encoders/WAV/WAVDataEncoder.h
	src/OpenAL/LAL.h
	src/Utils.h
)

add_executable(portamp
	${SRC_FILES}
	${HEADER_FILES}
)

set_property(TARGET portamp PROPERTY CXX_STANDARD 14)
set_property(TARGET portamp PROPERTY CXX_STANDARD_REQUIRED ON)

# patch OpusFile
set(__opus_version INCLUDED)
set(__opusfile_version INCLUDED)
function(get_package_version PACKAGE_VERSION PROJECT_VERSION)
  set(PACKAGE_VERSION ${OPUSFILE_PACKAGE_VERSION} PARENT_SCOPE)
  set(PROJECT_VERSION ${OPUSFILE_PROJECT_VERSION} PARENT_SCOPE)
endfunction()
set(OPUS_PACKAGE_VERSION "1.5.2" CACHE STRING "")
set(OPUSFILE_PACKAGE_VERSION "0.12" CACHE STRING "")
set(OPUSFILE_PROJECT_VERSION "0.12" CACHE STRING "")
set(OP_DISABLE_HTTP     ON CACHE BOOL "")
set(OP_DISABLE_EXAMPLES ON CACHE BOOL "")
set(OP_DISABLE_DOCS     ON CACHE BOOL "")
set(OPUS_INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "")

# Flac
set(BUILD_CXXLIBS             OFF CACHE BOOL "")
set(BUILD_PROGRAMS            OFF CACHE BOOL "")
set(BUILD_EXAMPLES            OFF CACHE BOOL "")
set(BUILD_TESTING             OFF CACHE BOOL "")
set(BUILD_DOCS                OFF CACHE BOOL "")
set(INSTALL_MANPAGES          OFF CACHE BOOL "")
set(INSTALL_PKGCONFIG_MODULES OFF CACHE BOOL "")
set(WITH_OGG                  ON CACHE BOOL "")

add_subdirectory(libs/monkey-audio EXCLUDE_FROM_ALL)

if(MSVC OR APPLE)
	add_subdirectory(libs/libogg EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libvorbis EXCLUDE_FROM_ALL)
	add_subdirectory(libs/flac EXCLUDE_FROM_ALL) # Flac adds ligogg
	add_subdirectory(libs/libmodplug EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopus EXCLUDE_FROM_ALL)
	add_subdirectory(libs/libopusfile EXCLUDE_FROM_ALL)
	target_compile_definitions(FLAC PRIVATE "PACKAGE_VERSION=\"1.5.0\"")
	target_compile_definitions(FLAC PRIVATE "FLAC__HAS_OGG=1")
	target_link_libraries(portamp
		Ogg::ogg
		vorbis
		vorbisfile
		modplug
		FLAC
		monkeyaudio
		Opus::opus
		OpusFile::opusfile
	)
   if(APPLE)
      target_compile_definitions(FLAC PRIVATE "HAVE_LROUND=1")
      target_compile_definitions(FLAC PRIVATE "HAVE_FSEEKO=1")
      target_compile_options(FLAC PRIVATE -Wno-undef)
      target_compile_options(modplug PRIVATE -Wno-deprecated-declarations)
      target_compile_options(monkeyaudio PRIVATE -Wno-deprecated-declarations)
      target_compile_options(opus PRIVATE -Wno-unused-but-set-variable)
      include_directories(/usr/local/include)
      find_package(OpenAL REQUIRED)
      target_link_libraries(portamp
         dl
         pthread
         "${OPENAL_LIBRARY}"
      )
   endif()
else()
	target_link_libraries(portamp
		ogg
		vorbis
		vorbisfile
		modplug
		FLAC
		monkeyaudio
		opus
		opusfile
		pthread
		dl
		openal
	)
	include_directories(/usr/local/include)
endif()

if(MSVC)
	set_property(TARGET ogg PROPERTY FOLDER ThirdParty)
	set_property(TARGET vorbis PROPERTY FOLDER ThirdParty)
	set_property(TARGET vorbisfile PROPERTY FOLDER ThirdParty)
	set_property(TARGET modplug PROPERTY FOLDER ThirdParty)
	set_property(TARGET FLAC PROPERTY FOLDER ThirdParty)
	set_property(TARGET monkeyaudio PROPERTY FOLDER ThirdParty)
	set_property(TARGET opus PROPERTY FOLDER ThirdParty)
	set_property(TARGET opusfile PROPERTY FOLDER ThirdParty)
endif()
