// $Id: noMovable.cpp 5024 2009-06-09 20:02:17Z OLiver $
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
#include "noMovable.h"

#include "GlobalGameSettings.h"
#include "GameWorld.h"
#include "Random.h"
#include "GameClient.h"
#include "EventManager.h"
#include "SerializedGameData.h"


///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

noMovable::noMovable(const NodalObjectType nop,const unsigned short x, const unsigned short y)
	: noCoordBase(nop,x,y), dir(4), ascent(0), current_ev(0), pause_walked_gf(0), pause_event_length(0), moving(false)
{
}

void noMovable::Serialize_noMovable(SerializedGameData * sgd) const
{
	Serialize_noCoordBase(sgd);

	sgd->PushUnsignedChar(dir);
	sgd->PushUnsignedChar(ascent);
	sgd->PushObject(current_ev,true);
	sgd->PushUnsignedInt(pause_walked_gf);
	sgd->PushUnsignedInt(pause_event_length);
	sgd->PushBool(moving);
}

noMovable::noMovable(SerializedGameData * sgd, const unsigned obj_id) : noCoordBase(sgd,obj_id),
dir(sgd->PopUnsignedChar()),
ascent(sgd->PopUnsignedChar()),
current_ev(sgd->PopObject<EventManager::Event>(GOT_EVENT)),
pause_walked_gf(sgd->PopUnsignedInt()),
pause_event_length(sgd->PopUnsignedInt())
, moving(sgd->PopBool())
{
}

void noMovable::Walk()
{
	moving = false;

	int tx=x,ty=y;
	x = gwg->GetXA(tx,ty,dir);
	y = gwg->GetYA(tx,ty,dir);


	// Auf der jeweiligen Stelle mich suchen und dort entfernen...
	if(dir != 1 && dir != 2)
		gwg->RemoveFigure(this,tx,ty);

	// und an der anderen Stelle wieder hinzufgen
	if(dir != 1 && dir != 2)
		gwg->AddFigure(this,x,y);
}

void noMovable::StartMoving(const unsigned char dir, unsigned gf_length)
{
	assert(!moving);

	// Ist das Wesen stehengeblieben mitten aufm Weg?
	if(pause_walked_gf)
	{
		// Das Laufevent fortführen
	/*	assert(dir == this->dir);*/
		current_ev = em->AddEvent(this,pause_event_length,0,pause_walked_gf);
		pause_walked_gf = 0;
		moving = true;
		return;
	}

	// Steigung ermitteln, muss entsprechend langsamer (hoch) bzw. schneller (runter) laufen
	// runter natürlich nich so viel schneller werden wie langsamer hoch
	switch(int(gwg->GetNodeAround(x,y,dir).altitude)-int(gwg->GetNode(x,y).altitude))
	{
	default: ascent = 3; // gerade
	case 1: ascent = 4; gf_length+=(gf_length/2); break; // leicht hoch
	case 2: case 3: ascent = 5; gf_length*=2;  break; // mittelsteil hoch
	case 4: case 5: ascent = 6; gf_length*=3;  break; // steil hoch
	case -1: ascent = 2; break; // leicht runter
	case -2: case -3: ascent = 1;  break; // mittelsteil runter
	case -4: case -5: ascent = 0; break; // steil runter
	}

	current_ev = em->AddEvent(this,gf_length);
	this->dir = dir;
	moving = true;

	// Wenn wir nach oben gehen, muss vom oberen Punkt dann aus gezeichnet werden im GameWorld
	if(dir == 1 || dir == 2)
	{
		gwg->RemoveFigure(this,x,y);
		gwg->AddFigure(this,gwg->GetXA(x,y,dir),gwg->GetYA(x,y,dir));
	}
}

void noMovable::CalcRelative(int &x, int &y,int x1, int y1,int x2,int y2)
{
	if(current_ev)
	{
		assert(current_ev->gf_length > 0);
		if(current_ev->gf_length == 0)
		{
			LOG.lprintf("Achtung: Bug im Spiel: noMovable::CalcRelative: current_ev->gf_length = 0!\n");
			return;
		}
	}

	assert(current_ev || pause_walked_gf);

	// Wenn wir mittem aufm Weg stehen geblieben sind, die gemerkten Werte jeweils nehmen
	unsigned gf_diff = current_ev ? (GAMECLIENT.GetGFNumber()-current_ev->gf) : pause_walked_gf;
	unsigned gf_length = current_ev ? current_ev->gf_length : pause_event_length;
	unsigned frame_time = current_ev ? GAMECLIENT.GetFrameTime() : 0;

	if(dir != 1 && dir != 2)
	{
		x	-= (((x1-x2) * int((gf_diff) * GAMECLIENT.GetGFLength()+frame_time)) / int(gf_length * GAMECLIENT.GetGFLength()));
		y	-= (((y1-y2) * int((gf_diff) * GAMECLIENT.GetGFLength()+frame_time)) / int(gf_length * GAMECLIENT.GetGFLength()));
	}
	else
	{
		x	-= (((x1-x2) * (int(gf_length * GAMECLIENT.GetGFLength())-int((gf_diff) * GAMECLIENT.GetGFLength()+frame_time))) / int(gf_length * GAMECLIENT.GetGFLength()));
		y	-= (((y1-y2) * (int(gf_length * GAMECLIENT.GetGFLength())-int((gf_diff) * GAMECLIENT.GetGFLength()+frame_time))) / int(gf_length * GAMECLIENT.GetGFLength()));
	}
}


void noMovable::PauseWalking()
{
	// Frames festhalten, bis zu denen wir gekommen sind
	pause_walked_gf = GameClient::inst().GetGFNumber() - current_ev->gf;
	// Länge merken
	pause_event_length = current_ev->gf_length;
	// Event abmelden
	em->RemoveEvent(current_ev);
	current_ev = 0;
	moving = false;

	// Achtung, evtl wird er gleich in dem gf gestoppt, wo er auch losgelaufen war
	// in dem Fall ist es wie, als ob er normal an einem bestimmten Knotenpunkt stoppt, bloß dass er
	// schon losgelaufen war, als der andere ihn stoppt, daher muss Verschieben wegen dem Zeichnen im GameWorld
	// rückgängig gemacht werden!
	if(pause_walked_gf == 0)
	{
		// Wenn wir nach oben gehen, muss vom oberen Punkt dann aus gezeichnet werden im GameWorld
		// --> rückgängig!
		if(dir == 1 || dir == 2)
		{
			gwg->RemoveFigure(this,gwg->GetXA(x,y,dir),gwg->GetYA(x,y,dir));
			gwg->AddFigure(this,x,y);
			
		}
	}
}
