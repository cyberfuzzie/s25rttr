// $Id: nofGeologist.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "nofGeologist.h"

#include "GameConsts.h"
#include "noFlag.h"
#include "Loader.h"
#include "macros.h"
#include "GameWorld.h"
#include "Random.h"
#include "EventManager.h"
#include "GameClient.h"
#include "noSign.h"
#include "nobBaseWarehouse.h"
#include "SoundManager.h"
#include "SerializedGameData.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofGeologist::nofGeologist(const unsigned short x, const unsigned short y,const unsigned char player,noRoadNode * goal)
: nofFlagWorker(JOB_GEOLOGIST,x,y,player,goal),  signs(0)
{
	node_goal.x = 0;
	node_goal.y = 0;
}

void nofGeologist::Serialize_nofGeologist(SerializedGameData * sgd) const
{
	Serialize_nofFlagWorker(sgd);

	sgd->PushUnsignedShort(signs);

	sgd->PushUnsignedInt(available_nodes.size());
	for(list<Point>::const_iterator it = available_nodes.begin();it.valid();++it)
	{
		sgd->PushUnsignedShort(it->x);
		sgd->PushUnsignedShort(it->y);
	}

	sgd->PushUnsignedShort(node_goal.x);
	sgd->PushUnsignedShort(node_goal.y);
}

nofGeologist::nofGeologist(SerializedGameData * sgd, const unsigned obj_id) : nofFlagWorker(sgd,obj_id),
signs(sgd->PopUnsignedShort())
{
	unsigned available_nodes_count = sgd->PopUnsignedInt();
	for(unsigned i = 0;i<available_nodes_count;++i)
	{
		Point p;
		p.x = sgd->PopUnsignedShort();
		p.y = sgd->PopUnsignedShort();
		available_nodes.push_back(p);
	}

	node_goal.x = sgd->PopUnsignedShort();
	node_goal.y = sgd->PopUnsignedShort();
}

void nofGeologist::Draw(int x, int y)
{
	switch(state)
	{
	default: break;
	case STATE_FIGUREWORK:
	case STATE_GEOLOGIST_GOTONEXTNODE:
	case STATE_GOTOFLAG:
		{
			// normales Laufen zeichnen
			DrawWalking(x,y,GetBobFile(jobs_bob),26,false);
		} break;
	case STATE_GEOLOGIST_DIG:
		{
			// 1x grab, 1x "spring-grab", 2x grab, 1x "spring-grab", 2x grab, 1x "spring-grab", 4x grab
			unsigned short i = GAMECLIENT.Interpolate(84,current_ev);
			unsigned sound = 0, sound_id;

			if(i<6)
			{
				GetRomBob(324+i)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				if(i == 4) { sound = 1; sound_id = 0; }
			}
			else if(i<16)
			{
				GetRomBob(314+i-6)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				if(i == 14) { sound = 2; sound_id = 1; }
			}
			else if(i<28)
			{
				GetRomBob(324+(i-16)%6)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				if(i == 20) { sound = 1; sound_id = 2; } else if(i == 26) { sound = 1; sound_id = 3; }
			}
			else if(i<38)
			{
				GetRomBob(314+i-28)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				if(i == 36) { sound = 2; sound_id = 4; }
			}
			else if(i<50)
			{
				GetRomBob(324+(i-38)%6)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				if(i == 42) { sound = 1; sound_id = 5; } else if(i == 48) { sound = 1; sound_id = 6; }
			}
			else if(i<60)
			{
				GetRomBob(314+i-50)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				if(i == 58) { sound = 2; sound_id = 7; }
			}
			else
			{
				GetRomBob(324+(i-60)%6)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				if(i == 64) { sound = 1; sound_id = 8; } else if(i == 70) { sound = 1; sound_id = 9; }
				else if(i == 76) { sound = 1; sound_id = 10; } else if(i == 82) { sound = 1; sound_id = 11; }
			}

			if(sound)
				SoundManager::inst().PlayNOSound((sound==1)?81:56,this,sound_id);

		} break;
	case STATE_GEOLOGIST_CHEER:
		{
			unsigned short i = GAMECLIENT.Interpolate(16,current_ev);

			if(i<7)
			{
				GetRomBob(357+
					i)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
			}
			else
			{
				unsigned char ids[9] = { 1, 0, 1, 2, 1, 0, 1, 2, 1};
				GetRomBob(361+ids[i-7])->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
			}

			if(i == 4)
				SoundManager::inst().PlayNOSound(107,this,12);

		} break;
	}

	/*char number[256];
	sprintf(number,"%u",obj_id);
	NormalFont->Draw(x,y,number,0,0xFFFF0000);*/
}


void nofGeologist::GoalReached()
{
	// An der Flagge angekommen

	// Geologe muss nun 15 Schildchen aufstellen

	signs = 15;

	// Umgebung absuchen
	LookForNewNodes();

	// ersten Punkt suchen
	/*dir = GetNextNode();*/
	GoToNextNode();
}

void nofGeologist::Walked()
{
	if(state == STATE_GEOLOGIST_GOTONEXTNODE)
	{
		// Ist mein Zielpunkt überhaupt noch geeignet zum Graben (kann ja mittlerweile auch was drauf gebaut worden sein)
		if(!IsNodeGood(node_goal.x,node_goal.y))
		{
			// alten Punkt wieder freigeben
			gwg->GetNode(node_goal.x,node_goal.y).reserved = false;;
			// wenn nicht, dann zu einem neuen Punkt gehen
			GoToNextNode();
			return;
		}

		// Bin ich am Zielpunkt?
		if(x == node_goal.x && y == node_goal.y)
		{
			// anfangen zu graben
			current_ev = em->AddEvent(this,100,1);
			state = STATE_GEOLOGIST_DIG;
		}
		else
		{
			// Weg zum nächsten Punkt suchen
			dir = gwg->FindFreePath(x,y,node_goal.x,node_goal.y,20);

			// Wenns keinen gibt 
			if(dir == 0xFF)
			{
				// alten Punkt wieder freigeben
				gwg->GetNode(node_goal.x,node_goal.y).reserved = false;;
				// dann neuen Punkt suchen
				dir = GetNextNode();
				// falls es keinen gibt, dann zurück zur Flagge gehen und es übernimmt der andere "Walked"-Zweig
				if(dir == 0xFF)
				{
					state = STATE_GOTOFLAG;
					Walked();
				}
				else
					StartWalking(dir);
			}
			else
				StartWalking(dir);
		}
	}
	else if(state == STATE_GOTOFLAG)
	{
		GoToFlag();
	}
}

void nofGeologist::HandleDerivedEvent(const unsigned int id)
{
	switch(state)
	{
	default:
		break;
	case STATE_GEOLOGIST_DIG:
		{
			// Ressourcen an diesem Punkt untersuchen
			unsigned char resources = gwg->GetNode(x,y).resources;


			if((resources >= 0x41 && resources <= 0x47)||(resources >= 0x49 && resources <= 0x4F)||
				(resources >= 0x51 && resources <= 0x57)||(resources >= 0x59 && resources <= 0x5F)||
				(resources == 0x20 || resources == 0x21))
			{
				// Es wurde was gefunden, erstmal Jubeln
				state = STATE_GEOLOGIST_CHEER;
				current_ev = em->AddEvent(this,15,1);
			}
			else
			{
				// leeres Schild hinstecken und ohne Jubel weiterziehen
				SetSign(resources);
				GoToNextNode();
				/// Punkt wieder freigeben
				gwg->GetNode(x,y).reserved = false;;
			}
				
		} break;
	case STATE_GEOLOGIST_CHEER:
		{
			// Schild reinstecken
			SetSign(gwg->GetNode(x,y).resources);
			// Und weiterlaufen
			GoToNextNode();
			/// Punkt wieder freigeben
			gwg->GetNode(x,y).reserved = false;;
			/// Sounds evtl löschen
			SoundManager::inst().WorkingFinished(this);
		} break;
	}
}


bool nofGeologist::IsNodeGood(const unsigned short x, const unsigned short y)
{
	// Es dürfen auch keine bestimmten Objekte darauf stehen und auch keine Schilder !!
	if(!gwg->IsNodeForFigures(x,y) || gwg->GetNO(x,y)->GetGOT() == GOT_SIGN
		|| gwg->GetNO(x,y)->GetType() == NOP_FLAG || gwg->GetNO(x,y)->GetType() == NOP_TREE)
		return false;

	// Und es darf auch kein Weg an diesem Punkt liegen
	for(unsigned char i = 0;i<6;++i)
	{
		if(gwg->GetPointRoad(x,y,i))
			return false;
	}

	return true;
}

void nofGeologist::LookForNewNodes()
{
	unsigned short max_radius = 15;
	bool found = false;

	for(unsigned short r = 1;r<max_radius;++r)
	{
		int x = flag->GetX(), y = flag->GetY();

		// r Punkte rüber
		x-=r;

		// r schräg runter bzw hoch
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y+i); // unten
			if(i) TestNode(x,y-i); // oben

			x+=(y&1);
		}

		// Die obere bzw untere Reihe
		for(unsigned short i = 0;i<r+1;++i,++x)
		{
			TestNode(x,y+r); // unten
			TestNode(x,y-r); // oben
		}

		x = flag->GetX()+r;

		// auf der anderen Seite wieder schräg hoch/runter
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y+i); // unten
			if(i) TestNode(x,y-i); // oben

			x-=!(y&1);
		}

		// Wenn es in diesem Umkreis welche gibt, dann nur noch 2 Kreise zusätzlich weitergehen
		if(!found && available_nodes.size())
		{
			max_radius = min(10,r+3);
			found = true;
		}

	}
}

void nofGeologist::TestNode(const unsigned short x, const unsigned short y)
{ 
	// Prüfen, ob er überhaupt auf der Karte liegt und nicht irgendwo im Nirvana
	if(x < gwg->GetWidth() && y < gwg->GetHeight())
	{
		if(IsNodeGood(x,y) && (gwg->FindFreePath(this->x,this->y,x,y,20)) != 0xFF && !gwg->GetNode(x,y).reserved)
		{
			nofGeologist::Point p = { x,y };
			available_nodes.push_back(p);
		}
	}
}

unsigned char nofGeologist::GetNextNode()
{
	// Überhaupt noch Schilder zum Aufstellen
	if(!signs)
		return 0xFF;
	do
	{
		// Sind überhaupt Punkte verfügbar?
		while(available_nodes.size())
		{
			// Dann einen Punkt zufällig auswählen
			list<nofGeologist::Point>::iterator it = available_nodes[RANDOM.Rand(__FILE__,__LINE__,obj_id,available_nodes.size())];
			// und aus der Liste entfernen
			node_goal = *it;
			available_nodes.erase(it);
			// Gucken, ob er gut ist und ob man hingehen kann und ob er noch nicht reserviert wurde!
			unsigned char ret_dir;
			if(IsNodeGood(node_goal.x,node_goal.y) && (ret_dir = gwg->FindFreePath(x,y,node_goal.x,node_goal.y,20)) != 0xFF && !gwg->GetNode(node_goal.x,node_goal.y).reserved)
			{
				// Reservieren
				gwg->GetNode(node_goal.x,node_goal.y).reserved = true;;
				return ret_dir;
			}
		}


		// Nach neuen Punkten sucehn 
		LookForNewNodes();



	} while(available_nodes.size());

	return 0xFF;
}

void nofGeologist::GoToNextNode()
{
	// Wenn es keine Flagge mehr gibt, dann gleich rumirren
	if(!flag)
	{
		StartWandering();
		Wander();
		state = STATE_FIGUREWORK;
		return;
	}

	// ersten Punkt suchen
	dir = GetNextNode();
	
	if(dir != 0xFF)
	{
		// Wenn es einen Punkt gibt, dann hingehen
		state = STATE_GEOLOGIST_GOTONEXTNODE;
		StartWalking(dir);
		--signs;
	}
	else
	{
		// ansonsten zur Flagge zurückgehen
		state = STATE_GOTOFLAG;
		Walked();
	}
}

void nofGeologist::SetSign(const unsigned char resources)
{
	// Bestimmte Objekte können gelöscht werden
	noBase * no = gwg->GetNO(x,y);

	if(no->GetType() != NOP_NOTHING && no->GetType() != NOP_ENVIRONMENT)
		return;

	// Zierobjekte löschen
	if(no->GetType() == NOP_ENVIRONMENT)
	{
		no->Destroy();
		delete no;
	}

	// Schildtyp und -häufigkeit herausfinden
	unsigned char type,quantity;


	if(resources >= 0x41 && resources <= 0x47)
	{
		// Kohle
		type = 2;
		quantity = (resources-0x40)/3;
	}
	else if(resources >= 0x49 && resources <= 0x4F)
	{
		// Eisen
		type = 0;
		quantity = (resources-0x48)/3;
	}
	else if(resources >= 0x51 && resources <= 0x57)
	{
		// Gold
		type = 1;
		quantity = (resources-0x50)/3;
	}
	else if(resources >= 0x59 && resources <= 0x5F)
	{
		// Granit
		type = 3;
		quantity = (resources-0x58)/3;
	}
	else if(resources == 0x20 || resources == 0x21)
	{
		// Wasser
		type = 4;
		quantity = 0;
	}
	else
	{
		// nichts
		type = 5;
		quantity = 0;
	}


	// Schild setzen
	gwg->SetNO(new noSign(x,y,type,quantity),x,y);


}

void nofGeologist::LostWork()
{
	flag = 0;

	switch(state)
	{
	default: break;
	// Wenn wir noch hingehen, dann zurückgehen
	case STATE_FIGUREWORK:
		{
			GoHome();
		} break;
	case STATE_GOTOFLAG:
		{
			// dann sofort rumirren, wenn wir zur Flagge gehen
			StartWandering();
			state = STATE_FIGUREWORK;
		} break;
	case STATE_GEOLOGIST_GOTONEXTNODE:
	case STATE_GEOLOGIST_DIG:
	case STATE_GEOLOGIST_CHEER:
		{
			gwg->GetNode(node_goal.x,node_goal.y).reserved = false;;
		} break;
	}
}
