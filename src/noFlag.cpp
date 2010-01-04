// $Id: noFlag.cpp 5853 2010-01-04 16:14:16Z FloSoft $
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
#include "noFlag.h"

#include "GameWorld.h"
#include "Loader.h"
#include "macros.h"
#include "nofCarrier.h"
#include "GameClient.h"
#include "GameClientPlayer.h"
#include "Ware.h"
#include "noBuilding.h"
#include "noBuildingSite.h"
#include "nobMilitary.h"
#include "SerializedGameData.h"
#include "FOWObjects.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

noFlag::noFlag(const unsigned short x, 
			   const unsigned short y,
			   const unsigned char player,
			   const unsigned char dis_dir)
	: noRoadNode(NOP_FLAG,x,y,player),
	ani_offset(rand() % 20000), flagtype(FT_NORMAL)
{
	for(unsigned i = 0;i<8;++i)
		wares[i] = 0;

	// BWUs nullen
	for(unsigned i = 0;i<MAX_BWU;++i)
	{
		bwus[i].id = 0xFFFFFFFF;
		bwus[i].last_gf = 0;
	}

	// Gucken, ob die Flagge auf einen bereits bestehenden Weg gesetzt wurde
	unsigned char dir;
	noFlag * flag = gwg->GetRoadFlag(x,y,dir,dis_dir);

	if(flag)
		flag->routes[dir]->SplitRoad(this);

	// auf Wasseranteile prüfen
	for(unsigned char i = 0; i < 6; ++i)
	{
		if(gwg->GetTerrainAround(x, y, i) == 14)
			flagtype = FT_WATER;
	}
}

noFlag::~noFlag()
{
	// Waren vernichten
	for(unsigned i = 0;i<8;++i)
		delete wares[i];
}

void noFlag::Destroy_noFlag()
{
	/// Da ist dann nichts
	gwg->SetNO(0,x,y);

	//// Straßen abreißen
	//for(unsigned char i = 0;i<6;++i)
	//	DestroyRoad(i);

	// Waren vernichten
	for(unsigned i = 0;i<8;++i)
	{
		if(wares[i])
		{
			// Inventur entsprechend verringern
			wares[i]->WareLost(player);
			delete wares[i];
			wares[i] = NULL;
		}
	}

	
	// Den Flag-Workern Bescheid sagen, die hier ggf. arbeiten
	gwg->GetPlayer(player)->FlagDestroyed(this);

	Destroy_noRoadNode();
}

void noFlag::Serialize_noFlag(SerializedGameData * sgd) const
{
	Serialize_noRoadNode(sgd);

	sgd->PushUnsignedChar(static_cast<unsigned char>(flagtype));
	for(unsigned i = 0;i<8;++i)
		sgd->PushObject(wares[i],true);

	// BWUs speichern
	for(unsigned i = 0;i<MAX_BWU;++i)
	{
		sgd->PushUnsignedInt(bwus[i].id);
		sgd->PushUnsignedInt(bwus[i].last_gf);
	}
}

noFlag::noFlag(SerializedGameData * sgd, const unsigned obj_id) : noRoadNode(sgd,obj_id),
ani_offset(rand() % 20000), 
flagtype(FlagType(sgd->PopUnsignedChar()))
{
	for(unsigned i = 0;i<8;++i)
		wares[i] = sgd->PopObject<Ware>(GOT_WARE);

	// BWUs laden
	for(unsigned i = 0;i<MAX_BWU;++i)
	{
		bwus[i].id = sgd->PopUnsignedInt();
		bwus[i].last_gf = sgd->PopUnsignedInt();
	}
}

void noFlag::Capture(const unsigned char new_owner)
{
	// Alle Straßen um mich herum zerstören bis auf die zum Gebäude (also Nr. 1)
	for(unsigned char i = 0;i<6;++i)
	{
		if(i != 1)
			DestroyRoad(i);
	}

	// Waren vernichten
	for(unsigned i = 0;i<8;++i)
	{
		if(wares[i])
		{
			wares[i]->WareLost(player);
			delete wares[i];
			wares[i] = NULL;
		}
	}

	this->player = new_owner;


}


void noFlag::Draw(int x, int y)
{

	//for(unsigned i = 0;i<6;++i)
	//	if(routes[i])
	//		assert(routes[i]->GetObjId() != 0);

	// Positionen der Waren an der Flagge relativ zur Flagge
	static const signed char WARES_POS[8][2] =
	{
		{0,0},
		{-4,0},
		{3,-1},
		{-7,-1},
		{6,-2},
		{-10,-2},
		{9,-5},
		{-13,-5}
	};

	// Flaggentyp berücksichtigen
	int nr = 0;
	switch(flagtype)
	{
	case FT_NORMAL:	nr = 100; break;
	case FT_LARGE:	nr = 120; break;
	case FT_WATER:	nr = 140; break;
	}

	// Flagge
	LOADER.GetNationImageN(gwg->GetPlayer(player)->nation, nr+GAMECLIENT.GetGlobalAnimation(8,80,40,ani_offset))->Draw(x,y,0,0,0,0,0,0, COLOR_WHITE, COLORS[gwg->GetPlayer(player)->color]);
	// Schatten
	LOADER.GetNationImageN(gwg->GetPlayer(player)->nation, nr+10+GAMECLIENT.GetGlobalAnimation(8,80,40,ani_offset))->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);

	// Waren (von hinten anfangen zu zeichnen)
	for(unsigned i = 8;i;--i)
	{
		if(wares[i-1])
			LOADER.GetMapImageN(2200+wares[i-1]->type)->Draw(x+WARES_POS[i-1][0],y+WARES_POS[i-1][1],0,0,0,0,0,0);
	}

	//char addresse[256];
	//sprintf(addresse,"%d",this->obj_id);
	//NormalFont->Draw(x,y,addresse,0,0xFFFFFF00);

}


void noFlag::GotWorker(Job job, noFigure * worker)
{
}

FOWObject * noFlag::CreateFOWObject() const
{
	return new fowFlag(player,flagtype);
}

void noFlag::AddWare(Ware * ware)
{
	for(unsigned i = 0;i<8;++i)
	{
		if(!wares[i])
		{
			wares[i] = ware;
			// Träger Bescheid sagen
			if(ware->GetNextDir() != 0xFF)
				routes[ware->GetNextDir()]->AddWareJob(this);
			return;
		}
	}
}

unsigned noFlag::GetWareCount() const
{
	unsigned count = 0;
	for(unsigned i = 0;i<8;++i)
		if(wares[i])
			++count;

	return count;
}

Ware * noFlag::SelectWare(const unsigned char dir,const bool swap_wares,const noFigure * const carrier)
{
	Ware * best_ware = 0;
	// Index merken, damit wir die enstprechende Ware dann entfernen können
	unsigned best_ware_index = 0xFF;

	// Die mit der niedrigsten, d.h. höchsten Priorität wird als erstes transportiert
	for(unsigned i = 0;i<8;++i)
	{
		if(wares[i])
		{
			if(wares[i]->GetNextDir() == dir)
			{
				if(best_ware)
				{
					if(gwg->GetPlayer(player)->transport[wares[i]->type] < gwg->GetPlayer(player)->transport[best_ware->type])
					{
						best_ware = wares[i];
						best_ware_index = i;
					}

				}
				else
				{
					best_ware = wares[i];
					best_ware_index = i;
				}
			}
		}
	}

	// Ware von der Flagge entfernen
	if(best_ware)
		wares[best_ware_index] = 0;

	// ggf. anderen Trägern Bescheid sagen, aber nicht dem, der die Ware aufgehoben hat!
	routes[dir]->WareJobRemoved(carrier);


	if(!swap_wares)
	{
		// Wenn nun wieder ein Platz frei ist, allen Wegen rundrum sowie evtl Warenhäusern
		// Bescheid sagen, die evtl waren, dass sie wieder was ablegen können
		for(unsigned char i = 0;i<6;++i)
		{
			if(routes[i])
			{
				if(routes[i]->GetLength() == 1)
				{
					// Gebäude?
					if(gwg->GetSpecObj<noBase>(x-!(y&1),y-1)->GetType() == NOP_BUILDING)
					{
						if(gwg->GetSpecObj<noBuilding>(x-!(y&1),y-1)->FreePlaceAtFlag())
							break;
					}
				}
				else
				{
					// Richtiger Weg --> Träger Bescheid sagen
					for(unsigned c = 0;c<2;++c)
					{
						if(routes[i]->carrier[c])
						{
							if(routes[i]->carrier[c]->SpaceAtFlag(this == routes[i]->f2))
								break;
						}
					}
				}
			}
 		}
	}

	assert(best_ware);
	if(!best_ware)
		LOG.lprintf("Achtung: Bug im Spiel: noFlag::SelectWare: best_ware = 0!\n");
	//assert(best_ware != NULL);

	return best_ware;
}

unsigned noFlag::GetWaresCountForRoad(const unsigned char dir) const
{
	unsigned count = 0;

	for(unsigned i = 0;i<8;++i)
	{
		if(wares[i])
		{
			if(wares[i]->GetNextDir() == dir)
				++count;
		}
	}

	return count;
}

unsigned short noFlag::GetPunishmentPoints(const unsigned char dir) const
{
	unsigned short points = 0;
	// Waren zählen, die in diese Richtung transportiert werden müssen
	for(unsigned i = 0;i<8;++i)
	{
		if(wares[i])
		{
			if(wares[i]->GetNextDir() == dir)
				points+=routes[dir]->GetLength()*2;
		}
	}

	// Wenn kein Träger auf der Straße ist, gibts nochmal extra satte Strafpunkte
	if(!routes[dir]->carrier)
		points+=200;

	return points;
}

//bool noFlag::ReserveWare()
//{
//	// Noch genug Platz für eine Ware (Reservierungen und Waren, die schon daliegen, dürfen nicht >= 8 sein)
//	if(GetWareCount()+reserved_wares > 7)
//		return false;
//	else
//	{
//		// Reservierung aufnehmen
//		++reserved_wares;
//		return true;
//	}
//}


void noFlag::DestroyAttachedBuilding()
{
	// ggf. damit verbundene Gebäude, Baustellen abreißen

	// Achtung es wird ein Feuer durch Destroy gesetzt, daher Objekt merken!
	noBase * no = gwg->GetNO(x-!(y&1),y-1);
	if(no->GetType() == NOP_BUILDINGSITE || no->GetType() == NOP_BUILDING)
	{
		no->Destroy();
		delete no;
	}
}

void noFlag::Upgrade()
{
	if(flagtype == FT_NORMAL)
		flagtype = FT_LARGE;
}

/// Zeitintervall, in der die Zugänglichkeit der Flaggen von einer bestimmten BWU überprüft wird
const unsigned MAX_BWU_INTERVAL = 2000;

/// Ist diese Flagge für eine bestimmte Lagerhausflüchtlingsgruppe (BWU) nicht zugänglich?
bool noFlag::IsImpossibleForBWU(const unsigned bwu_id) const
{
	// BWU-ID erstmal suchen
	for(unsigned i = 0;i<MAX_BWU;++i)
	{
		if(bwus[i].id == bwu_id)
		{
			// Wenn letzter TÜV noch nicht zu lange zurückliegt, können wir sie als unzugänglich zurückgeben
			if(GameClient::inst().GetGFNumber() - bwus[i].last_gf <= MAX_BWU_INTERVAL)
				return true;
			else
				// ansonsten nicht, evtl ist sie ja jetzt mal wieder zugänglich, sollte also mal neu geprüft werden
				return false;
		}
	}

	return false;
}

/// Hinzufügen, dass diese Flagge für eine bestimmte Lagerhausgruppe nicht zugänglich ist
void noFlag::ImpossibleForBWU(const unsigned bwu_id)
{
	// Evtl gibts BWU schon --> Dann einfach GF-Zahl aktualisieren
	for(unsigned i = 0;i<MAX_BWU;++i)
	{
		if(bwus[i].id == bwu_id)
		{
			bwus[i].last_gf = GameClient::inst().GetGFNumber();
			return;
		}
	}

	// Gibts noch nicht, dann den ältesten BWU-Account raussuchen und den überschreiben
	unsigned oldest_gf = 0xFFFFFFFF;
	unsigned oldest_account_id = 0;

	for(unsigned i = 0;i<MAX_BWU;++i)
	{
		if(bwus[i].last_gf < oldest_gf)
		{
			// Neuer ältester
			oldest_gf = bwus[i].last_gf;
			oldest_account_id = i;
		}
	}

	// Den ältesten dann schließlich überschreiben
	bwus[oldest_account_id].id = bwu_id;
	bwus[oldest_account_id].last_gf = oldest_gf;
}
