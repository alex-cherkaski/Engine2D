#pragma once

using EventId = size_t;

class EventIdGenerator final
{
public:
	template<typename TEvent> static EventId GetEventId();

private:
	static EventId m_nextEventId;
};

template<typename TEvent>
EventId EventIdGenerator::GetEventId()
{
	static EventId eventId = m_nextEventId++;
	return eventId;
}

