// $Id: noCharburnerPile.cpp 6582 2010-07-16 11:23:35Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "noCharburnerPile.h"

#include "Loader.h"
#include "GameClient.h"
#include "Random.h"
#include "GameWorld.h"
#include "SerializedGameData.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/// Länge des Wachs-Wartens
const unsigned GROWING_WAITING_LENGTH = 1100;
/// Länge des Wachsens
const unsigned GROWING_LENGTH = 16;

noCharburnerPile::noCharburnerPile(const unsigned short x, const unsigned short y) : noCoordBase(NOP_CHARBURNERPILE,x,y),
state(STATE_GROWING_WAITING), event(em->AddEvent(this,GROWING_WAITING_LENGTH))
{
}

noCharburnerPile::~noCharburnerPile()
{
	
}

void noCharburnerPile::Destroy_noGrainfield()
{
	em->RemoveEvent(event);

	// Bauplätze drumrum neu berechnen
	gwg->RecalcBQAroundPoint(x,y);

	Destroy_noCoordBase();
}

void noCharburnerPile::Serialize_noCharburnerPile(SerializedGameData * sgd) const
{
	Serialize_noCoordBase(sgd);

	sgd->PushUnsignedChar(static_cast<unsigned char>(state));
	sgd->PushObject(event,true);
}

noCharburnerPile::noCharburnerPile(SerializedGameData * sgd, const unsigned obj_id) : noCoordBase(sgd,obj_id),
state(State(sgd->PopUnsignedChar())),
event(sgd->PopObject<EventManager::Event>(GOT_EVENT))
{
}

void noCharburnerPile::Draw( int x,	int y)
{


}

void noCharburnerPile::HandleEvent(const unsigned int id)
{

}



