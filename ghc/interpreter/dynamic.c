
/* --------------------------------------------------------------------------
 * Dynamic loading (of .dll or .so files) for Hugs
 *
 * The Hugs 98 system is Copyright (c) Mark P Jones, Alastair Reid, the
 * Yale Haskell Group, and the Oregon Graduate Institute of Science and
 * Technology, 1994-1999, All rights reserved.  It is distributed as
 * free software under the license in the file "License", which is
 * included in the distribution.
 *
 * $RCSfile: dynamic.c,v $
 * $Revision: 1.8 $
 * $Date: 1999/10/20 02:15:59 $
 * ------------------------------------------------------------------------*/

#include "prelude.h"
#include "storage.h"
#include "errors.h"
#include "dynamic.h"

#if HAVE_WINDOWS_H && !defined(__MSDOS__)

#include <windows.h>

ObjectFile loadLibrary(fn)
String fn; {
    return LoadLibrary(fn);
}

void* lookupSymbol(file,symbol)
ObjectFile file;
String symbol; {
    return GetProcAddress(file,symbol);
}

const char *dlerror(void)
{
   return "<unknown>";
}

void* getDLLSymbol(dll,symbol)  /* load dll and lookup symbol */
String dll;
String symbol; {
    ObjectFile instance = LoadLibrary(dll);
    if (NULL == instance) {
        /* GetLastError allegedly provides more detail - in practice,
	 * it tells you nothing more.
         */
        ERRMSG(0) "Error while importing DLL \"%s\"", dll
        EEND;
    }
    return GetProcAddress(instance,symbol);
}

#elif HAVE_DLFCN_H /* eg LINUX, SOLARIS, ULTRIX */

#include <stdio.h>
#include <dlfcn.h>

ObjectFile loadLibrary(fn)
String fn; {
    return dlopen(fn,RTLD_NOW | RTLD_GLOBAL);
}

void* lookupSymbol(file,symbol)
ObjectFile file;
String symbol; {
    return dlsym(file,symbol);
}

void* getDLLSymbol(dll,symbol)  /* load dll and lookup symbol */
String dll;
String symbol; {
#ifdef RTLD_NOW
    ObjectFile instance = dlopen(dll,RTLD_NOW);
#elif defined RTLD_LAZY /* eg SunOS4 doesn't have RTLD_NOW */
    ObjectFile instance = dlopen(dll,RTLD_LAZY);
#else /* eg FreeBSD doesn't have RTLD_LAZY */
    ObjectFile instance = dlopen(dll,1);
#endif
    void *sym;

    if (NULL == instance) {
	ERRMSG(0) "Error while importing DLL \"%s\":\n%s\n", dll, dlerror()
        EEND;
    }
    if (sym = dlsym(instance,symbol))
        return sym;

    ERRMSG(0) "Error loading sym:\n%s\n", dlerror()
    EEND;
}

#elif HAVE_DL_H /* eg HPUX */

#include <dl.h>

void* getDLLSymbol(dll,symbol)  /* load dll and lookup symbol */
String dll;
String symbol; {
    ObjectFile instance = shl_load(dll,BIND_IMMEDIATE,0L);
    void* r;
    if (NULL == instance) {
        ERRMSG(0) "Error while importing DLL \"%s\"", dll
        EEND;
    }
    return (0 == shl_findsym(&instance,symbol,TYPE_PROCEDURE,&r)) ? r : 0;
}

#else /* Dynamic loading not available */

void* getDLLSymbol(dll,symbol)  /* load dll and lookup symbol */
String dll;
String symbol; {
#if 1 /* very little to choose between these options */
    return 0;
#else
    ERRMSG(0) "This Hugs build does not support dynamic loading\n"
    EEND;
#endif
}

#endif /* Dynamic loading not available */

