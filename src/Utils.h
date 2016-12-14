#pragma once

#include <memory>
#include <vector>
#include <stdint.h>

// GCC
#ifdef __GNUC__
#  define PLATFORM_GCC
#  ifdef __clang__
#     define __COMPILER_VER__ "Clang " __VERSION__
#  else
#     define __COMPILER_VER__ "GCC " __VERSION__
#  endif
#endif

// Microsoft Visual C++
#ifdef _MSC_VER
#  define PLATFORM_MSVC
#define __COMPILER_VER__ "Microsoft Visual C++"
#endif

#if defined(_WIN64)
#  define BUILD_OS "Win64"
#elif defined(_WIN32)
#  define BUILD_OS "Win32"
#elif defined(__APPLE__)
#  if __x86_64__ || __ppc64__
#     define BUILD_OS "OS X 64"
#  else
#     define BUILD_OS "OS X 32"
#  endif
#elif defined(__FreeBSD__)
#  if __x86_64__ || __ppc64__
#     define BUILD_OS "FreeBSD 64"
#  else
#     define BUILD_OS "FreeBSD 32"
#  endif
#else
#  if __x86_64__ || __ppc64__
#     define BUILD_OS "Linux64"
#  else
#     define BUILD_OS "Linux32"
#  endif
#endif

/// A data blob
class clBlob
{
public:
	explicit clBlob( const std::vector<uint8_t>& Data )
	: m_Data( Data )
	{}

	const std::vector<uint8_t>& GetData() const { return m_Data; }
	size_t GetDataSize() const { return m_Data.size(); }
	const uint8_t* GetDataPtr() const { return m_Data.data(); }

private:
	std::vector<uint8_t> m_Data;
};

struct sConfig
{
	sConfig()
	: m_Loop( false )
	, m_UseModPlugToDecodeWAV( false )
	, m_Verbose( false )
	, m_OutputFile()
	{}

	bool m_Loop;
	bool m_UseModPlugToDecodeWAV;
	bool m_Verbose;
	std::string m_OutputFile;
};

std::shared_ptr<clBlob> ReadFileAsBlob( const char* FileName );

int IsKeyPressed();
bool IsVerbose();

void Log_Error( const char* Format... );

extern sConfig g_Config;
