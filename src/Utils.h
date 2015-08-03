#pragma once

#include <vector>
#include <stdint.h>

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
