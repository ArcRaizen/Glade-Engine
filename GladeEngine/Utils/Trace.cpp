#include "Trace.h"
#include <stdio.h>

namespace Glade
{
	bool traceEnabled = false;

	int traceLevel = 0;

	std::vector<std::string> traceStrings;

	bool traceAllStrings = false;

	void TracePrintf(const char* format, ...)
	{
		va_list ap;
		static bool init = false;
		static FILE* logFile = nullptr;
		static errno_t err;

		if(init == false)
		{
			init = true;
			err = fopen_s(&logFile, "program.log", "w");

			if(logFile == nullptr || err != 0)
			{
				fprintf(stderr, "Unable to open program.log\n");
#ifdef WIN32
				fprintf(stderr, "Trying C:\\program.log\n");
				char logFileName[] = "C:\\program.log";
#else
				fprintf(stderr, "Trying /tmp/program.log\n");
				char logFileName[] = "/tmp/program.log";
#endif
				err = fopen_s(&logFile, logFileName, "w");
				if(logFile == nullptr || err != 0)
					fprintf(stderr, "Unable to open backup %s\n", logFileName);
				else
					printf("Opened log file: %s\n", logFileName);
			}
			else
				printf("Opened log file: program.log\n");
		}

		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);

		if(logFile && err == 0)
		{
			va_start(ap, format);
			vfprintf(logFile, format, ap);
			fflush(logFile);
			va_end(ap);
		}
	}
}