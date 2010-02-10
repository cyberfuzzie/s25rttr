// $Id: win32_nanosleep.h 5991 2010-02-10 15:44:37Z FloSoft $
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
#ifndef WIN32_NANOSLEEP_H_INCLUDED
#define WIN32_NANOSLEEP_H_INCLUDED

#pragma once

#ifdef _WIN32

typedef unsigned int useconds_t;

typedef struct timespec
{
    unsigned int tv_sec;	// Seconds.
    long int tv_nsec;		// Nanoseconds.
} timespec_t;

/// Sleep at least some number of microseconds.
int usleep (useconds_t microseconds);

/// nanosleep replacement for windows.
int nanosleep(const timespec_t *requested_delay, timespec_t *remaining_delay);

#endif // _WIN32

#endif // !WIN32_NANOSLEEP_H_INCLUDED
