// $Id: main.h 6395 2010-05-03 20:37:41Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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

#define _CRTDBG_MAP_ALLOC

#ifdef _WIN32
	#include <windows.h>
#ifdef _MSC_VER
	#include <crtdbg.h>
#else
    #include <assert.h>
#endif
	
	int mkdir(const char *p, int unused) { return (CreateDirectoryA(p, NULL) == TRUE ? 0 : 1); }

	#undef PlaySound
#else
	#include <sys/stat.h>
	#include <unistd.h>
#endif // !_WIN32

#if defined _WIN32 && defined _DEBUG
	#include <crtdbg.h>
#endif // _WIN32 && _DEBUG

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>

#include <bzlib.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#endif // MAIN_H_INCLUDED
