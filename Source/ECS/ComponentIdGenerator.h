#pragma once
#include "Types.h"
#include "Macros.h"

class ComponentIdGenerator final
{
public:
	template<typename TComponent> static ComponentId GetComponentId();

private:
	static ComponentId m_nextComponentId;
};

template<typename TComponent>
static ComponentId ComponentIdGenerator::GetComponentId()
{
	const static ComponentId componentId = m_nextComponentId++;
	return componentId;
}

