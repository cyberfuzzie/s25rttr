// $Id: noStaticObject.cpp 6458 2010-05-31 11:38:51Z FloSoft $
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
#include "noEnvObject.h"

#include "noExtension.h"

#include "Loader.h"
#include "GameWorld.h"
#include "SerializedGameData.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p noStaticObject.
 *
 *  @param[in] id   Nr der Grafik
 *  @param[in] file Nr der Datei (0xFFFF map_?_z.lst, 0-5 mis?bobs.lst)
 *  @param[in] size Größe des Objekts
 *  @param[in] type Typ des Objekts
 *
 *  @author FloSoft
 */
noStaticObject::noStaticObject(unsigned short x, unsigned short y, unsigned short id, unsigned short file, unsigned char size, NodalObjectType type)
	: noCoordBase(type,x,y), id(id), file(file), size(size)
{
	// sind wir ein "Schloss" Objekt?
	if(GetSize() == 2)
	{
		for(unsigned i = 0;i<3;++i)
		{
			MapCoord xa = gwg->GetXA(x,y,i);
			MapCoord ya = gwg->GetYA(x,y,i);
			
			noBase *no = gwg->GetSpecObj<noBase>(xa,ya);
			if(no)
			{
				no->Destroy();
				delete no;
			}
			gwg->SetNO(new noExtension(this),xa,ya);
		}
	}
}

void noStaticObject::Serialize_noStaticObject(SerializedGameData * sgd) const
{
	Serialize_noCoordBase(sgd);

	sgd->PushUnsignedShort(id);
	sgd->PushUnsignedShort(file);
	sgd->PushUnsignedChar(size);
}

noStaticObject::noStaticObject(SerializedGameData * sgd, const unsigned obj_id) : noCoordBase(sgd,obj_id),
id(sgd->PopUnsignedShort()),
file(sgd->PopUnsignedShort()),
size(sgd->PopUnsignedChar())
{
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  zeichnet das Objekt.
 *
 *  @param[in] id Nr der Grafik
 *  @param[in] file Nr der Datei (0xFFFF map_?_z.lst, 0-5 mis?bobs.lst)
 *
 *  @author FloSoft
 */
void noStaticObject::Draw(int x, int y)
{
	glArchivItem_Bitmap *bitmap = NULL, *shadow = NULL;

	if(file == 0xFFFF)
	{
		bitmap = LOADER.GetMapImageN(id);
		shadow = LOADER.GetMapImageN(id+100);
	}
	else if(file < 6)
	{
		static const std::string misxbobs[6] = {
			"mis0bobs", "mis1bobs", "mis2bobs", "mis3bobs", "mis4bobs", "mis5bobs"
		};
		bitmap = LOADER.GetImageN(misxbobs[file], id);
		shadow = LOADER.GetImageN(misxbobs[file], id+1);
	}

	assert(bitmap);

	// Bild zeichnen
	bitmap->Draw(x, y, 0, 0, 0, 0, 0, 0);

	// Schatten zeichnen
	if(shadow)
		shadow->Draw(x, y, 0, 0, 0, 0, 0, 0, COLOR_SHADOW);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  zerstört das Objekt.
 *
 *  @author FloSoft
 */
void noStaticObject::Destroy_noStaticObject(void)
{
	// waren wir ein "Schloss" Objekt?
	if(GetSize() == 2)
	{
	
		for(unsigned i = 0;i<3;++i)
		{
			MapCoord xa = gwg->GetXA(x,y,i);
			MapCoord ya = gwg->GetYA(x,y,i);
			
			noBase *no = gwg->GetSpecObj<noBase>(xa, ya);
			if(no)
			{
				no->Destroy();
				delete no;
				gwg->SetNO(NULL, xa, ya);
			}
		}
	}

	Destroy_noBase();
}
