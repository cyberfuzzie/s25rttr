// $Id: noCharburnerPile.cpp 6711 2010-09-06 14:42:33Z FloSoft $
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
#include "noEnvObject.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/// Length of the smoldering
const unsigned SMOLDERING_LENGTH = 200;

/// Work steps for one graphical step during the construction
const unsigned short CONSTRUCTION_WORKING_STEPS = 1;
/// Work steps for one graphical step during the remove of the cover
const unsigned short REMOVECOVER_WORK_STEPS = 1;
/// Work steps for one graphical step during the "harvest"
const unsigned short HARVEST_WORK_STEPS = 1;

noCharburnerPile::noCharburnerPile(const unsigned short x, const unsigned short y) : noCoordBase(NOP_CHARBURNERPILE,x,y),
state(STATE_WOOD), step(0), sub_step(0), event(NULL)
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
	switch(state)
	{
	case STATE_WOOD:
		{
			unsigned draw_id;
			if(step < 5)
				draw_id = 51-step;
			else 
				draw_id = 26;

			LOADER.GetImageN("charburner_bobs",draw_id)->Draw(x,y);
		} return;
	case STATE_SMOLDERING:
		{
			LOADER.GetImageN("charburner_bobs",27+GameClient::inst().
				GetGlobalAnimation(2,10,1,obj_id+this->x*10+this->y*10))->Draw(x,y);
		} return;
	case STATE_REMOVECOVER:
		{
			LOADER.GetImageN("charburner_bobs",28+step)->Draw(x,y);
		} return;
	case STATE_HARVEST:
		{
			LOADER.GetImageN("charburner_bobs",27+GameClient::inst().
				GetGlobalAnimation(2,1,10,obj_id+this->x*10+this->y*10))->Draw(x,y);

		} return;
	default: return;
	}
}

void noCharburnerPile::HandleEvent(const unsigned int id)
{
	// Smoldering is over 
	// Pile is ready for the remove of the cover
	state = STATE_REMOVECOVER;
	event = NULL;
}


/// Charburner has worked on it --> Goto next step
void noCharburnerPile::NextStep()
{
	switch(state)
	{
	case STATE_WOOD:
		{
			++sub_step;
			if(sub_step == CONSTRUCTION_WORKING_STEPS)
			{
				++step;
				sub_step = 0;

				// Reached new state?
				if(step == 6)
				{
					step = 0;
					state = STATE_SMOLDERING;
					event = em->AddEvent(this,SMOLDERING_LENGTH,0);
				}
			}

		} return;
	case STATE_REMOVECOVER:
		{
			++sub_step;
			if(sub_step == REMOVECOVER_WORK_STEPS)
			{
				++step;
				sub_step = 0;

				// Reached new state?
				if(step == 6)
				{
					state = STATE_HARVEST;
					step = 0;
				}
			}

		} return;
	case STATE_HARVEST:
		{
			++sub_step;
			if(sub_step == HARVEST_WORK_STEPS)
			{
				++step;
				sub_step = 0;

				// Reached new state?
				if(step == 6)
				{
					// Add an empty pile as environmental object
					gwg->SetNO(new noEnvObject(x,y,40,6),x,y);
					em->AddToKillList(this);

					// BQ drumrum neu berechnen
					gwg->RecalcBQAroundPoint(x,y);
				}
			}
		} return;
	default: return;
	}
}


