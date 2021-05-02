#pragma once

#define NO_COPY(OBJECT)	\
	OBJECT(const OBJECT&) = delete;	\
	const OBJECT& operator=(const OBJECT&) = delete

#define NO_MOVE(OBJECT)	\
	OBJECT(OBJECT&&) = delete; \
	const OBJECT& operator=(OBJECT&&) = delete

#define SINGLETON(OBJECT) \
private: \
	static OBJECT& GetInstance() \
	{ \
		static OBJECT instance; \
		return instance; \
	} \
 \
public: \
	static const OBJECT& GetInstanceRead() \
	{ \
		return GetInstance(); \
	} \
 \
	static OBJECT& GetInstanceWrite() \
	{ \
		return GetInstance(); \
	} \
 \
	NO_COPY(OBJECT); \
	NO_MOVE(OBJECT);

#define TO_DEGREES(ANGLE) ANGLE / static_cast<float>(M_PI) * 180.0f
#define TO_RADIANS(ANGLE) ANGLE / 180.0f * static_cast<float>(M_PI)

