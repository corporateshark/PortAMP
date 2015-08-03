#include <fstream>

#include "Utils.h"

#if defined(__APPLE__)
#	include <sys/select.h>
#else
#	include <conio.h>
#endif

std::vector<uint8_t> ReadFileAsVector( const char* FileName )
{
	std::ifstream File( FileName, std::ifstream::binary );

	File.seekg( 0, std::ios::end );
	std::streampos End = File.tellg();
	File.seekg( 0, std::ios::beg );
	std::streampos Start = File.tellg();
	size_t Size = static_cast<size_t>( End - Start );

	std::vector<uint8_t> Result( Size );

	File.read( reinterpret_cast<char*>( Result.data() ), Size );

	return Result;
}

std::shared_ptr<clBlob> ReadFileAsBlob( const char* FileName )
{
	return std::make_shared<clBlob>( ReadFileAsVector( FileName ) );
}

bool IsKeyPressed()
{
#if defined(_WIN32)
	return _kbhit();
#elif defined(__APPLE__)
	struct timeval tv = { 0L, 0L };
	fd_set fds;
	FD_ZERO( &fds );
	FD_SET( 0, &fds );
	return select( 1, &fds, nullptr, nullptr, &tv );
#else
	return kbhit();
#endif
}
