#include <fstream>

#include "Utils.h"

#if defined(__APPLE__)
#	include <sys/select.h>
#	include <sys/time.h>
#	include <sys/types.h>
#	include <sys/select.h>
#	include <unistd.h>
#	include <termios.h>
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

int IsKeyPressed()
{
#if defined(_WIN32)
	bool Res = _kbhit();
	while (_kbhit()) getch();
	return Res;
#elif defined(__APPLE__)
	struct termios ttystate;
 	tcgetattr( STDIN_FILENO, &ttystate );
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_cc[VMIN] = 1;
	tcsetattr( STDIN_FILENO, TCSANOW, &ttystate );

	fd_set fds;
	FD_ZERO( &fds );
	FD_SET( STDIN_FILENO, &fds );
	struct timeval tv = { 0L, 0L };
	(void)select( STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv );
	bool HasKey = FD_ISSET( STDIN_FILENO, &fds );
	return HasKey ? fgetc( stdin ) : 0;
#else
	bool Res = kbhit();
	while (kbhit()) getch();
	return Res;
#endif
}
