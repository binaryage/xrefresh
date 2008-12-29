///////////////////////////////////////////////////////////////
//
// tstring.h
//
// Created: 25/05/2003
// Copyright (c) 2003 Ralph Hare (ralph.hare@ysgyfarnog.co.uk)
// All rights reserved.
//
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
//
///////////////////////////////////////////////////////////////

#ifndef __TSTRING_H_A564C193_B602_4665_A5C7_957CB66B9C5E_
#define __TSTRING_H_A564C193_B602_4665_A5C7_957CB66B9C5E_

#include <tchar.h>
#ifndef _WINDOWS_
#include <windows.h>
#endif

#include <string>
#include <sstream>
#include <fstream>

typedef std::basic_string< TCHAR >          tstring;
typedef std::basic_ostringstream< TCHAR >   tostringstream;
typedef std::basic_istringstream< TCHAR >   tistringstream;
typedef std::basic_stringstream< TCHAR >    tstringstream;
typedef std::basic_istream< TCHAR >         tistream;
typedef std::basic_ostream< TCHAR >         tostream;
typedef std::basic_ofstream< TCHAR >        tofstream;
typedef std::basic_ifstream< TCHAR >        tifstream;
typedef std::basic_fstream< TCHAR >         tfstream;

#endif // __TSTRING_H_A564C193_B602_4665_A5C7_957CB66B9C5E_
