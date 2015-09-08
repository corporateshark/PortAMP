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
	virtual size_t GetNumTracks() const { return m_FileNames.size(); }
	virtual std::string GetAndPopNextTrack( bool Loop )
	{
		std::string Result = m_FileNames.front();

		m_FileNames.pop_front();

		// readd to the end of the list
		if ( Loop ) m_FileNames.push_back( Result );

		return Result;
	}

private:
	std::deque<std::string> m_FileNames;
};