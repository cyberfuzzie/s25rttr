// $Id: noGranite.cpp 6458 2010-05-31 11:38:51Z FloSoft $
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
#include "noGranite.h"

#include "Loader.h"
#include "EventManager.h"
#include "SerializedGameData.h"
#include "FOWObjects.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

noGranite::noGranite(const GraniteType type, const unsigned char state) : noBase(NOP_GRANITE), type(type), state(state)
{
}

void noGranite::Serialize_noGranite(SerializedGameData * sgd) const
{
	Serialize_noBase(sgd);

	sgd->PushUnsignedChar(static_cast<unsigned char>(type));
	sgd->PushUnsignedChar(state);
}

noGranite::noGranite(SerializedGameData * sgd, const unsigned obj_id) : noBase(sgd,obj_id),
type(GraniteType(sgd->PopUnsignedChar())),
state(sgd->PopUnsignedChar())
{
}

void noGranite::Draw(int x, int y)
{
	LOADER.GetMapImageN(516 + type*6 + state)->Draw(x,y,0,0,0,0,0,0);
	LOADER.GetMapImageN(616 + type*6 + state)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
}

FOWObject * noGranite::CreateFOWObject() const
{
	return new fowGranite(type,state);
}

void noGranite::Hew()
{
	if(state)
		--state;
}
