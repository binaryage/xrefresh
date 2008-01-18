//+---------------------------------------------------------------------------
//
//  Copyright ( C ) Microsoft, 1994 - 2002.
//
//  File:       syntax2.cpp
//
//  Contents:   data definitions for the syntax modules
//
//  Classes:
//
//  Functions:
//
//  Coupling:
//
//  Notes:
//
//  Author:     Eric Niebler ( ericne@microsoft.com )
//
//  History:    3-29-00   ericne   Created
//
//----------------------------------------------------------------------------

#include "syntax2.h"

namespace regex
{

REGEX_SELECTANY TOKEN const perl_syntax_base::s_rgreg[ UCHAR_MAX + 1 ] =
{
/*  0*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/*  8*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 16*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 24*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 32*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    END_LINE,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 40*/ BEGIN_GROUP, END_GROUP, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,  MATCH_ANY, NO_TOKEN,
/* 48*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 56*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 64*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 72*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 80*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/* 88*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN, BEGIN_CHARSET,      ESCAPE,  NO_TOKEN, BEGIN_LINE, NO_TOKEN,
/* 96*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/*104*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/*112*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN,    NO_TOKEN,  NO_TOKEN,   NO_TOKEN, NO_TOKEN,
/*120*/    NO_TOKEN,  NO_TOKEN, NO_TOKEN,      NO_TOKEN, ALTERNATION,  NO_TOKEN,   NO_TOKEN, NO_TOKEN
// and the rest are 0...
};

REGEX_SELECTANY TOKEN const perl_syntax_base::s_rgescape[ UCHAR_MAX + 1 ] =
{
/*  0*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/*  8*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 16*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 24*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 32*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 40*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 48*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 56*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 64*/    NO_TOKEN,   ESC_BEGIN_STRING, ESC_NOT_WORD_BOUNDARY,       NO_TOKEN,
      ESC_NOT_DIGIT, ESC_QUOTE_META_OFF,              NO_TOKEN,       NO_TOKEN,
/* 72*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 80*/    NO_TOKEN,  ESC_QUOTE_META_ON,              NO_TOKEN,  ESC_NOT_SPACE,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,   ESC_NOT_WORD,
/* 88*/    NO_TOKEN,           NO_TOKEN,        ESC_END_STRING,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/* 96*/    NO_TOKEN,           NO_TOKEN,     ESC_WORD_BOUNDARY,       NO_TOKEN,
          ESC_DIGIT,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/*104*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN,
/*112*/    NO_TOKEN,           NO_TOKEN,              NO_TOKEN,      ESC_SPACE,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       ESC_WORD,
/*120*/    NO_TOKEN,           NO_TOKEN,      ESC_END_STRING_z,       NO_TOKEN,
           NO_TOKEN,           NO_TOKEN,              NO_TOKEN,       NO_TOKEN
// and the rest are 0...
};

namespace detail
{

REGEX_SELECTANY extern posix_charset_type const g_rgposix_charsets[] =
{
    { "[:alnum:]",    9 },
    { "[:^alnum:]",  10 },
    { "[:alpha:]",    9 },
    { "[:^alpha:]",  10 },
    { "[:blank:]",    9 },
    { "[:^blank:]",  10 },
    { "[:cntrl:]",    9 },
    { "[:^cntrl:]",  10 },
    { "[:digit:]",    9 },
    { "[:^digit:]",  10 },
    { "[:graph:]",    9 },
    { "[:^graph:]",  10 },
    { "[:lower:]",    9 },
    { "[:^lower:]",  10 },
    { "[:print:]",    9 },
    { "[:^print:]",  10 },
    { "[:punct:]",    9 },
    { "[:^punct:]",  10 },
    { "[:space:]",    9 },
    { "[:^space:]",  10 },
    { "[:upper:]",    9 },
    { "[:^upper:]",  10 },
    { "[:xdigit:]",  10 },
    { "[:^xdigit:]", 11 }
};

REGEX_SELECTANY extern size_t const g_cposix_charsets = ARRAYSIZE( g_rgposix_charsets );

} // namespace detail

} // namespace regex






/***
*resetstk - Recover from Stack overflow.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the _resetstkoflw() function.
*
*******************************************************************************/

#if defined(_MSC_VER) & _MSC_VER < 1300
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>

#define MIN_STACK_REQ_WIN9X 0x11000
#define MIN_STACK_REQ_WINNT 0x2000

#ifdef _WIN64
typedef unsigned __int64 REGEX_DWORD_PTR;
#else
typedef unsigned __int32 REGEX_DWORD_PTR;
#endif

struct osplatform_getter
{
    int m_osplatform;

    osplatform_getter() : m_osplatform( 0 )
    {
        OSVERSIONINFOA osvi;
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        if( GetVersionExA( & osvi ) )
            m_osplatform = osvi.dwPlatformId;
    }
};

inline int get_osplatform()
{
    static osplatform_getter const s_osplatform_getter;
    return s_osplatform_getter.m_osplatform;
};

/***
* void _resetstkoflw(void) - Recovers from Stack Overflow
*
* Purpose:
*       Sets the guard page to its position before the stack overflow.
*
* Exit:
*       Returns nonzero on success, zero on failure
*
*******************************************************************************/

extern "C" int __cdecl _resetstkoflw(void)
{
    LPBYTE pStack, pGuard, pStackBase, pMinGuard;
    MEMORY_BASIC_INFORMATION mbi;
    SYSTEM_INFO si;
    DWORD PageSize;
    DWORD flNewProtect;
    DWORD flOldProtect;

    // Use _alloca() to get the current stack pointer

    pStack = static_cast<LPBYTE>( _alloca(1) );

    // Find the base of the stack.

    if (VirtualQuery(pStack, &mbi, sizeof mbi) == 0)
        return 0;
    pStackBase = static_cast<LPBYTE>( mbi.AllocationBase );

    // Find the page just below where the stack pointer currently points.
    // This is the new guard page.

    GetSystemInfo(&si);
    PageSize = si.dwPageSize;

    pGuard = (LPBYTE) (((REGEX_DWORD_PTR)pStack & ~(REGEX_DWORD_PTR)(PageSize - 1))
                       - PageSize);

    // If the potential guard page is too close to the start of the stack
    // region, abandon the reset effort for lack of space.  Win9x has a
    // larger reserved stack requirement.

    pMinGuard = pStackBase + ((get_osplatform() == VER_PLATFORM_WIN32_WINDOWS)
                              ? MIN_STACK_REQ_WIN9X
                              : MIN_STACK_REQ_WINNT);

    if (pGuard < pMinGuard)
        return 0;

    // On a non-Win9x system, release the stack region below the new guard
    // page.  This can't be done for Win9x because of OS limitations.

    if (get_osplatform() != VER_PLATFORM_WIN32_WINDOWS) {
        if (pGuard > pStackBase)
            VirtualFree(pStackBase, pGuard - pStackBase, MEM_DECOMMIT);

        VirtualAlloc(pGuard, PageSize, MEM_COMMIT, PAGE_READWRITE);
    }

    // Enable the new guard page.

    flNewProtect = get_osplatform() == VER_PLATFORM_WIN32_WINDOWS
                   ? PAGE_NOACCESS
                   : PAGE_READWRITE | PAGE_GUARD;

    return VirtualProtect(pGuard, PageSize, flNewProtect, &flOldProtect);
}

#endif
