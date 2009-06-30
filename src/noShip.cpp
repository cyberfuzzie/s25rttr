// $Id: noShip.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "noShip.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "Random.h"
#include "EventManager.h"
#include "GameWorld.h"
#include "SerializedGameData.h"

const unsigned SHIP_SPEED = 20;


/// Konstruktor
noShip::noShip(const unsigned short x, const unsigned short y, const unsigned char playe) 
: noMovable(NOP_SHIP,x,y), player(player), state(STATE_IDLE)
{
}

void noShip::Serialize(SerializedGameData * sgd) const
{
	Serialize_noMovable(sgd);

	sgd->PushUnsignedChar(player);
	sgd->PushUnsignedChar(static_cast<unsigned char>(state));
}

noShip::noShip(SerializedGameData * sgd, const unsigned obj_id) : noMovable(sgd,obj_id),
player(player),
state(State(sgd->PopUnsignedChar()))
{
}

void noShip::Destroy()
{
}


void noShip::Draw(int x, int y)
{
 	switch(state)
	{
	default:
		break;
	case STATE_IDLE:
		{
			GetImage(boot_lst, 1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
			GetImage(boot_lst, 0)->Draw(x,y);
		} break;
	}

}


void noShip::HandleEvent(const unsigned int id)
{
	current_ev = 0;

	switch(id)
	{
	// Laufevent
	case 0:
		{
			// neue Position einnehmen
			Walk();

			// entscheiden, was als nächstes zu tun ist
			Driven();
		} break;

	}

}

void noShip::StartDriving(const unsigned char dir)
{
	StartMoving(dir,SHIP_SPEED);
}

void noShip::Driven()
{
	/*switch(state)
	{
	default:
		break;
	}*/
}
