#include <fstream>

#include "Utils.h"

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
