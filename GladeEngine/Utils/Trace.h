#ifndef GLADE_TRACE_H
#define GLADE_TRACE_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdarg.h>

namespace Glade {

	// Enable/Disable Trace
	inline void EnableTrace(bool enable);

	// Set Trace level
	inline void SetTraceLevel(int level);

	// Trace all strings?
	inline void EnableTraceAllStrings(bool enable);

	// Add a string to the list of Traced strings
	inline void AddTraceStrings(std::vector<std::string> traceStrings);
	inline void AddTraceString(const std::string& traceString);
	inline void AddTraceSTring(const char* traceString);

	enum class TraceLevels
	{
		ONCE_1 = 1,		// significant once-off Trace
		ONCE_2 = 2,
		ONCE_3 = 3,		// low significance
		FRAME_1 = 4,	// significant once-per-frame trace
		FRAME_2 = 5,	
		FRAME_3 = 6,	// low significance
		MULTI_FRAME_1 = 7,	// significant multiple-times-per-frame trace
		MULTI_FRAME_2 = 8,
		MULTI_FRAME_3 = 9	// low significance
	};

	#ifdef TRACE
	#undef TRACE
	#endif

	#define TRACE Glade::TracePrintf


#ifdef USE_FUNCTION
#define TRACE_FUNCTION()	\
	TRACE("%s():%D: ", __FUNCTION__, __LINE__)
#else
#define TRACE_FUNCTION()	\
	TRACE("%s:%d: ", __FILE__, __LINE__)
#endif

#ifdef USE_FUNCTION
#define TRACE_METHOD()		\
	TRACE("%s:%s():%d [%p]: ", __FILE__, __FUNCTION, __LINE__, this)
#else
#define TRACE_METHOD()		\
	TRACE("%s:%d: [%p]: ", __FILE__, __LINE__, this)
#endif

#ifdef USE_FUNCTION
#define TRACE_METHOD_STATIC()	\
	TRACE("%s:%s():%d [static]: ", __FILE__, __FUNCTION__, __LINE__)
#else
#define TRACE_METHOD_STATIC()	\
	TRACE("%s:%d: [static]: ",	__FILE__, _LINE__)
#endif


#ifdef COMPILE_WITHOUT_TRACE
	#define TRACE_IF(level, traceString) if(0)
	#define TRACE_FILE_IF(level) if(0)
	#define TRACE_FUNCTION_ONLY(level) {}
	#define TRACE_METHOD_ONLY(level) {}
	#define TRACE_METHOD_STATIC_ONLY(level){}
#else
	#define TRACE_IF(level, traceString)	\
		if((traceEnabled) &&				\
			((level) <= traceLevel) &&		\
			((traceAllStrings == true)	||	\
			(Glade::CheckTraceString(traceString))))

	#define TRACE_FILE_IF(level)			\
		TRACE_IF(level, __FILE__)

	#define TRACE_FUNCTION_ONLY(level)		\
		TRACE_FILE_IF(level) { TRACE_FUNCTION(); TRACE("\n"); }

	#define TRACE_METHOD_ONLY(level)		\
		TRACE_FILE_IF(level) { TRACE_METHOD(); TRACE("\n");	}

	#define TRACE_METHOD_STATIC_ONLY(level)	\
		TRACE_FILE_IF(level) { TRACE_METHOD_STATIC(); TRACE("\n"); }
#endif	// COMPILE_WITHOUT_TRACE

	// Is Trace enabled?
	extern bool	traceEnabled;


	// Trace level - only Trace with a level <= this comes out
	extern int traceLevel;

	// Strings for which Trace is enabled.
	// Normally these will be file names, though they don't have to be
	extern std::vector<std::string> traceStrings;

	// If this flag is set, all Trace strings are enabled
	extern bool traceAllStrings;

	// Do Trace output
	void TracePrintf(const char* format, ...);

	// Enable/Disable Trace
	inline void EnableTrace(bool enable) { traceEnabled = enable; }

	// Enable/Disable all strings Trace
	inline void EnableTraceAllStrings(bool enable) { traceAllStrings = enable; }

	inline void SetTraceLevel(int level) { traceLevel = level; }

	inline void AddTraceString(const std::string& traceString)
	{
		traceStrings.push_back(traceString);
		std::sort(traceStrings.begin(), traceStrings.end());
	}

	inline void AddTraceString(const char* traceString)
	{
		AddTraceString(std::string(traceString));
	}

	inline void AddTraceStrings(std::vector<std::string> newTraceStrings)
	{
		for(unsigned int i = 0; i < newTraceStrings.size(); ++i)
			traceStrings.push_back(newTraceStrings[i]);
		std::sort(traceStrings.begin(), traceStrings.end());
	}

	inline bool CheckTraceString(const char* traceString)
	{
		return std::binary_search(traceStrings.begin(), traceStrings.end(), std::string(traceString));
	}

}	// namespace
#endif // GLADE_TRACE_H