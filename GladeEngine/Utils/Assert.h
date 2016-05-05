#ifndef GLADE_ASSERT_H
#define GLADE_ASSERT_H

#ifndef _DEBUG
namespace Glade
{
	#define Assert(exp)				((void)0)
	#define AssertMsg(exp, msg)		((void)0)
}	// namespace

#else	// _DEBUG

#include "../Utils/Trace.h"
#include <stdio.h>
#include <stdlib.h>

void AssertFn();

namespace Glade
{
#define GLADE_DEATH								\
	printf(										\
	"Glade you ignorant slut\n"				\
	"Press 'c <RET>' to continue or 'q <RET>' to quit.\n");	\
	for(;;)										\
	{											\
		int c = getc(stdin);					\
		if(c == 'c') break;						\
		else if(c = 'q') abort();				\
	}


#define Assert(exp)		\
	if(!(exp))			\
	{					\
		TRACE("Assert %s at %s:%d\n", #exp, __FILE__, __LINE__);	\
		AssertFn();		\
		GLADE_DEATH		\
	}

#define AssertMsg(exp, msg)		\
	if(!(exp))					\
	{							\
		TRACE("Assert %s at %s:%d\n%s\n", #exp, __FILE__, __LINE__, msg);	\
		AssertFn();				\
		GLADE_DEATH				\
	}

}	// namespace
#endif	// _DEBUG
#endif // GLADE_ASSERT_H