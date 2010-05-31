// $Id: glArchivItem_Bob.cpp 6458 2010-05-31 11:38:51Z FloSoft $
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
#include "glArchivItem_Bob.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeichnet einen Animationsstep.
 *
 *  @author FloSoft
 */
void glArchivItem_Bob::Draw(unsigned int item, unsigned int direction, bool fat, unsigned int animationstep, short x, short y, unsigned int color)
{
	unsigned int good = item*96 + animationstep*12 + ( (direction + 3) % 6 ) + fat*6;
	unsigned int body = fat*48 + ( (direction + 3) % 6 )*8 + animationstep;
	if(links[good] == 92)
	{
		good -= fat*6;
		body -= fat*48;
	}

	glArchivItem_Bitmap_Player *koerper = dynamic_cast<glArchivItem_Bitmap_Player*>(get(body));
	if(koerper)
		koerper->Draw(x, y, 0, 0, 0, 0, 0, 0, COLOR_WHITE, color);
	glArchivItem_Bitmap_Player *ware = dynamic_cast<glArchivItem_Bitmap_Player*>(get(96+links[good]));
	if(ware)
		ware->Draw(x, y, 0, 0, 0, 0, 0, 0, COLOR_WHITE, color);
}
