
#ifndef _LOAD_LIBRARY_MAKER_TYPE_15_01_2010_
#define _LOAD_LIBRARY_MAKER_TYPE_15_01_2010_


class baseobject;

#ifndef WINDOWS
typedef baseobject* (*maker_t)();
#else
#include <windows.h>
typedef baseobject* (WINAPI*maker_t)();
#endif

#endif /* _LOAD_LIBRARY_MAKER_TYPE_15_01_2010_ */

