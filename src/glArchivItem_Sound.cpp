// $Id: glArchivItem_Sound.cpp 5853 2010-01-04 16:14:16Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "glArchivItem_Sound.h"

#include "VideoDriverWrapper.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Konstruktor von @p glArchivItem_Sound.
 *
 *  @author FloSoft
 */
glArchivItem_Sound::glArchivItem_Sound(void)
	: baseArchivItem_Sound(), sound(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Kopiekonstruktor von @p glArchivItem_Sound.
 *
 *  @author FloSoft
 */
glArchivItem_Sound::glArchivItem_Sound(const glArchivItem_Sound *item)
	: baseArchivItem_Sound(item), sound(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Destruktor von @p glArchivItem_Sound.
 *
 *  @author FloSoft
 */
glArchivItem_Sound::~glArchivItem_Sound(void)
{
}
