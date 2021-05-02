#pragma once
#include "Types.h"

class TagIdGenerator final
{
public:
	template<typename TTag> static TagId GetTagId();

private:
	static TagId m_nextTagId;
};

template<typename TTag>
static TagId TagIdGenerator::GetTagId()
{
	const static TagId tagIg = m_nextTagId++;
	return tagIg;
}

