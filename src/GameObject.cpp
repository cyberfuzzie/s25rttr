// $Id: GameObject.cpp 5348 2009-07-30 16:04:54Z jh $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "GameObject.h"
#include "SerializedGameData.h"

#include "GamePlayerList.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Objekt-ID-Counter.
 *
 *  @author OLiver
 */
unsigned int GameObject::obj_id_counter = 1;
unsigned int GameObject::obj_counter = 0;

GameWorldGame *GameObject::gwg = NULL;
EventManager *GameObject::em = NULL;
GameClientPlayerList *GameObject::players = NULL;

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p GameObject.
 *
 *  @param[in] nop ObjektTyp
 *
 *  @author OLiver
 */
GameObject::GameObject(void) : obj_id(obj_id_counter++)
{
	// ein Objekt mehr
	++obj_counter;
}

GameObject::GameObject(SerializedGameData * sgd, const unsigned obj_id) : obj_id(obj_id)
{
	// ein Objekt mehr
	++obj_counter;
	sgd->AddObject(this);
}

GameObject::GameObject(const GameObject& go) : obj_id(go.obj_id)
{
	// ein Objekt mehr
	++obj_counter;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p GameObject.
 *
 *  @author OLiver
 */
GameObject::~GameObject()
{
	// ein Objekt weniger
	--obj_counter;
}
