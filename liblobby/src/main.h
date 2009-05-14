// $Id: main.h 4865 2009-05-14 10:05:04Z FloSoft $
//
// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// System-Header

#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
	#include <windows.h>
	
	#ifndef __CYGWIN__
		#include <conio.h>
	#endif
	
	#include <ws2tcpip.h>

#ifdef _MSC_VER
	#include <crtdbg.h>
#else
    #include <assert.h>
#endif

#endif // !_WIN32

#include <errno.h>

#include <map>
#include <cstdarg>
#include <cstring>
#include <string>
#include <sstream>
#include <cstdlib>

#if defined _WIN32 && defined _DEBUG
	#include <crtdbg.h>
#endif // _WIN32 && _DEBUG

///////////////////////////////////////////////////////////////////////////////
// Eigene Header
#include "local.h"
#include "libutil.h"
#include "mygettext.h"
#include "liblobby.h"

#ifdef _WIN32

#ifdef _MSC_VER
	#define getch _getch
#ifndef snprintf
	#define snprintf _snprintf
#endif
	#define assert _ASSERT
#endif

#endif // _WIN32

const char *GetWindowTitle();
const char *GetWindowVersion();
const char *GetWindowRevision();

#endif // MAIN_H_INCLUDED
