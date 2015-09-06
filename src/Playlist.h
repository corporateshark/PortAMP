#pragma once

#include <deque>
#include <string>

class clPlaylist
{
public:
	virtual ~clPlaylist() {};

	virtual void EnqueueTrack( const char* FileName )
	{
		m_FileNames.emplace_back( FileName );
	}

	virtual bool IsEmpty() const { return m_FileNames.empty(); }
	virtual std::string GetAndPopNextTrack()
	{
		std::string Result = m_FileNames.front();

		m_FileNames.pop_front();

		return Result;
	}

private:
	std::deque<std::string> m_FileNames;
};