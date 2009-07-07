// $Id: main.h 5211 2009-07-07 13:14:17Z FloSoft $
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
#ifdef _WIN32
#	define _CRTDBG_MAP_ALLOC
#	define WINVER 0x0501
#	define _WIN32_WINNT 0x0501
#	include <windows.h>
#	include <io.h>
#else
#	include <unistd.h>
#	include <limits.h>
#endif // !_WIN32

#if defined _WIN32 && defined _DEBUG
#	include <crtdbg.h>
#endif // _WIN32 && _DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>

#ifdef __APPLE__
	#include <SDL.h>
#else
	#include <SDL/SDL.h>
#endif // !__APPLE__

#include "oem.h"

#endif // !MAIN_H_INCLUDED
