// $Id: GameWorldGame.cpp 5178 2009-07-03 11:55:24Z OLiver $
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
#include "GameWorld.h"

#include "GameClient.h"
#include "GameClientPlayer.h"
#include "Random.h"
#include "SoundManager.h"
#include "SerializedGameData.h"

#include "nofCarrier.h"
#include "noEnvObject.h"
#include "noStaticObject.h"
#include "noGranite.h"
#include "noTree.h"
#include "noFlag.h"
#include "nobHQ.h"
#include "noFire.h"
#include "nobUsual.h"
#include "noBuildingSite.h"
#include "Ware.h"
#include "MilitaryConsts.h"
#include "TerritoryRegion.h"
#include "nobMilitary.h"
#include "nofAttacker.h"
#include "nofPassiveSoldier.h"
#include "noAnimal.h"
#include "noFighting.h"
#include "CatapultStone.h"
#include "MapGeometry.h"
#include "nofScout_Free.h"

#include "WindowManager.h"
#include "GameInterface.h"


void GameWorldGame::RecalcBQAroundPoint(const MapCoord x, const MapCoord y)
{
	// Drumherum BQ neu berechnen, da diese sich ja jetzt hätten ändern können
	GetNode(x,y).bq = CalcBQ(x,y,GAMECLIENT.GetPlayerID());
	for(unsigned char i = 0;i<6;++i)
		GetNode(GetXA(x,y,i),GetYA(x,y,i)).bq = CalcBQ(GetXA(x,y,i), GetYA(x,y,i),GAMECLIENT.GetPlayerID());
}

void GameWorldGame::RecalcBQAroundPointBig(const MapCoord x, const MapCoord y)
{
	RecalcBQAroundPoint(x,y);

	// 2. Außenschale
	for(unsigned i = 0;i<12;++i)
		GetNode(GetXA2(x,y,i),GetYA2(x,y,i)).bq = CalcBQ(GetXA2(x,y,i),GetYA2(x,y,i),GAMECLIENT.GetPlayerID());
}

void GameWorldGame::SetFlag(const MapCoord x, const MapCoord y, const unsigned char player,const unsigned char dis_dir)
{
	// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	if(CalcBQ(x,y,player,true,false) != BQ_FLAG)
		return;
	//
	//// Abfragen, ob schon eine Flagge in der Nähe ist (keine Mini-1-Wege)
	//for(unsigned char i = 0;i<6;++i)
	//{
	//	if(GetNO(GetXA(x,y,i), GetYA(x,y,i))->GetType() == NOP_FLAG)
	//		return;
	//}

	//// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	//// Abfragen, ob evtl ein Baum gepflanzt wurde, damit der nicht überschrieben wird
	//if(GetNO(x,y)->GetType() == NOP_TREE)
	//	return;

	// Gucken, nicht, dass schon eine Flagge dasteht
	if(GetNO(x,y)->GetType() != NOP_FLAG)
	{
		noBase *no = GetSpecObj<noBase>(x,y);
		if(no)
		{
			no->Destroy();
			delete no;
		}

		SetNO(NULL, x, y);
		SetNO(new noFlag(x, y, player, dis_dir), x, y);

		RecalcBQAroundPointBig(x,y);
	}
}

void GameWorldGame::DestroyFlag(const MapCoord x, const MapCoord y)
{
	// Gucken, ob sich da auch eine Flagge befindet
	if(GetNO(x,y)->GetType() == NOP_FLAG)
	{
		// Dann abreißen mitsamt Gebäude
		noFlag * flag = GetSpecObj<noFlag>(x,y);
		SetNO(NULL, x, y);
		flag->DestroyAttachedBuilding();
		flag->Destroy();
		delete flag;

		RecalcBQAroundPointBig(x,y);
	}

	gi->GI_FlagDestroyed(x,y);
}



///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den echten Straßen-Wert an der Stelle X,Y (berichtigt).
 *
 * Bit 0-6 jeweils 2 Bit für jede Richtung jeweils der Typ, Bit 7
 *  @author OLiver
 */	
void GameWorldGame::SetRoad(const MapCoord x, const MapCoord y, unsigned char dir, unsigned char type)
{
	assert(dir < 6);

	// Virtuelle Straße setzen
	SetVirtualRoad(x, y, dir, type);

	unsigned pos = width * unsigned(y) + unsigned(x);


	// Flag nullen wenn nur noch das real-flag da ist oder es setzen
	if(!nodes[pos].roads[dir])
		nodes[pos].roads_real[dir] = false;
	else
		nodes[pos].roads_real[dir] = true;

	if(gi)
		gi->GI_UpdateMinimap(x,y);
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Straßen-Wert um den Punkt X,Y.
 *
 *  @author OLiver
 */
void GameWorldGame::SetPointRoad(const MapCoord x, const MapCoord y, unsigned char dir, unsigned char type)
{
	assert(dir < 6);

	if(dir >= 3)
		SetRoad(x,y, dir - 3, type);
	else
		SetRoad(GetXA(x, y, dir),GetYA(x, y, dir), dir, type);
}



void GameWorldGame::AddFigure(noBase * fig,const MapCoord x, const MapCoord y)
{
	if(!fig)
		return;

	assert(!GetNode(x,y).figures.search(fig).valid());
	GetNode(x,y).figures.push_back(fig);



	for(unsigned char i = 0;i<6;++i)
	{
		int xa = GetXA(x,y,i);
		int ya = GetYA(x,y,i);

		if(xa <0 || ya < 0 || xa >= width || ya >= height)
			continue;

		if(GetNode(xa,ya).figures.search(fig).valid())
			assert(false);
	}

	//if(fig->GetDir() == 1 || fig->GetDir() == 2)
	//	figures[y*width+x].push_front(fig);
	//else
	//	figures[y*width+x].push_back(fig);
}

void GameWorldGame::RemoveFigure(const noBase * fig,const MapCoord x, const MapCoord y)
{
	for(list<noBase*>::iterator it = GetNode(x,y).figures.begin(); it.valid(); ++it)
	{
		if(*it == fig)
		{
			GetNode(x,y).figures.erase(it);
			return;
		}
	}
}



void GameWorldGame::SetBuildingSite(const BuildingType type,const MapCoord x, const MapCoord y, const unsigned char player)
{
	//// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	//// Abfragen, ob schon eine Flagge in der Nähe ist (keine Mini-1-Wege)
	//unsigned short flag_x = x+(y&1),flag_y = y+1;
	//for(unsigned char i = 0;i<6;++i)
	//{
	//	if(GetNO(GetXA(flag_x,flag_y,i),GetYA(flag_x,flag_y,i))->GetType() == NOP_FLAG)
	//		return;
	//}

	//// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	//// Abfragen, ob evtl ein Baum gepflanzt wurde, damit der nicht überschrieben wird und auch nich am Platz davor für die
	//// Flagge!
	//if(GetNO(x,y)->GetType() == NOP_TREE || GetNO(x+(y&1),y+1)->GetType() == NOP_TREE)
	//	return;

	// Gucken, ob das Gebäude hier überhaupt noch gebaut wrden kann
	BuildingQuality bq = CalcBQ(x,y,player,false,false);

	switch(BUILDING_SIZE[type])
	{
	case BQ_HUT: if(!((bq >= BQ_HUT && bq <= BQ_CASTLE) || bq == BQ_HARBOR)) return; break;
	case BQ_HOUSE: if(!((bq >= BQ_HOUSE && bq <= BQ_CASTLE) || bq == BQ_HARBOR)) return; break;
	case BQ_CASTLE: if(!( bq == BQ_CASTLE || bq == BQ_HARBOR)) return; break;
	case BQ_HARBOR: if(bq != BQ_HARBOR) return; break;
	case BQ_MINE: if(bq != BQ_MINE) return; break;
	default: break;
	}

	// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	// Wenn das ein Militärgebäude ist und andere Militärgebäude bereits in der Nähe sind, darf dieses nicht gebaut werden
	if(type >= BLD_BARRACKS && type <= BLD_FORTRESS)
	{
		if(IsMilitaryBuildingNearNode(x,y))
			return;
	}

	// ggf. vorherige Objekte löschen
	noBase *no = GetSpecObj<noBase>(x,y);
	if(no)
	{
		no->Destroy();
		delete no;
	}

	// Baustelle setzen
	SetNO(new noBuildingSite(type, x, y, player), x, y);
	gi->GI_UpdateMinimap(x,y);

	// Bauplätze drumrum neu berechnen
	RecalcBQAroundPointBig(x,y);
}

void GameWorldGame::DestroyBuilding(const MapCoord x, const MapCoord y, const unsigned char player)
{
	// Steht da auch ein Gebäude oder eine Baustelle, nicht dass wir aus Verzögerung Feuer abreißen wollen, das geht schief
	if(GetNO(x,y)->GetType() == NOP_BUILDING || 
		GetNO(x,y)->GetType() == NOP_BUILDINGSITE)
	{
		
		noBaseBuilding * nbb  = GetSpecObj<noBaseBuilding>(x,y);

		// Ist das Gebäude auch von dem Spieler, der es abreißen will?
		if(nbb->GetPlayer() != player)
			return;

		// Militärgebäude?
		if(nbb->GetGOT() == GOT_NOB_MILITARY)
		{
			// Darf das Gebäude abgerissen werden?
			if(!static_cast<nobMilitary*>(nbb)->IsDemolitionAllowed())
				// Nein, darf nicht abgerissen werden
				return;
		}

		
		nbb->Destroy();
		delete nbb;
		// Bauplätze drumrum neu berechnen
		RecalcBQAroundPointBig(x,y);
	}
	else
		LOG.lprintf("GameWorldGame::DestroyBuilding: WARNING: BuildingType not found!\n");
}


void GameWorldGame::BuildRoad(const unsigned char playerid,const bool boat_road,
							  unsigned short start_x,unsigned short start_y, const std::vector<unsigned char>& route)
{
	// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	if(!GetSpecObj<noFlag>(start_x,start_y))
		return;

	unsigned short tmpx = start_x, tmpy = start_y;

	// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	// Gucken, ob der Weg überhaupt noch gebaut werden kann
	unsigned short testx = start_x, testy = start_y;
	assert(route.size() > 1);
	for(unsigned i = 0;i<route.size()-1;++i)
	{
		int tx = testx,ty = testy;
		testx = GetXA(tx,ty,route[i]);
		testy = GetYA(tx,ty,route[i]);

		// Feld bebaubar und auf unserem Gebiet
		if(!RoadAvailable(boat_road,testx,testy,i,false) || !IsPlayerTerritory(testx,testy))
		{
			// Nein? Dann Weg nicht bauen und ggf. das visuelle wieder zurückbauen
			RemoveVisualRoad(start_x,start_y,route);
			return;
		}
	}

	int tx = testx,ty = testy;
	testx = GetXA(tx,ty,route[route.size()-1]);
	testy = GetYA(tx,ty,route[route.size()-1]);

	// Prüfen, ob am Ende auch eine Flagge steht oder eine gebaut werden kann
	if(GetNO(testx,testy)->GetGOT() == GOT_FLAG)
	{
		// Falscher Spieler?
		if(GetSpecObj<noFlag>(testx,testy)->GetPlayer() != playerid)
		{
			// Dann Weg nicht bauen und ggf. das visuelle wieder zurückbauen
			RemoveVisualRoad(start_x,start_y,route);
			return;
		}
	}
	else
	{
		// Es ist keine Flagge dort, dann muss getestet werden, ob da wenigstens eine gebaut werden kann

		// TODO: Verzögerungsbugabfrage, kann später ggf. weg
		// Abfragen, ob schon eine Flagge in der Nähe ist (keine Mini-1-Wege)
		for(unsigned char i = 0;i<6;++i)
		{
			if(GetNO(GetXA(testx,testy,i), GetYA(testx,testy,i))->GetGOT() == GOT_FLAG)
			{
				// Dann Weg nicht bauen und ggf. das visuelle wieder zurückbauen
				RemoveVisualRoad(start_x,start_y,route);
				return;
			}
		}

		// TODO: Verzögerungsbugabfrage, kann später ggf. weg
		// Abfragen, ob evtl ein Baum gepflanzt wurde, damit der nicht überschrieben wird
		if(GetNO(testx,testy)->GetType() == NOP_TREE)
		{
			// Dann Weg nicht bauen und ggf. das visuelle wieder zurückbauen
			RemoveVisualRoad(start_x,start_y,route);
			return;
		}
	}

	// Evtl Zierobjekte abreißen (Anfangspunkt)
	if(IsObjectionableForRoad(start_x,start_y))
	{
		noBase * obj = GetSpecObj<noBase>(start_x,start_y);
		obj->Destroy();
		delete obj;
		SetNO(0,start_x,start_y);
	}

	for(unsigned i = 0;i<route.size();++i)
	{
		SetPointRoad(start_x,start_y, route[i], boat_road?(RoadSegment::RT_BOAT+1):(RoadSegment::RT_NORMAL+1));
		int tx = start_x,ty = start_y;
		start_x = GetXA(tx,ty,route[i]);
		start_y = GetYA(tx,ty,route[i]);
		CalcRoad(tx,ty,GAMECLIENT.GetPlayerID());

		// Evtl Zierobjekte abreißen
		if(IsObjectionableForRoad(start_x,start_y))
		{
			noBase * obj = GetSpecObj<noBase>(start_x,start_y);
			obj->Destroy();
			delete obj;
			SetNO(0,start_x,start_y);
		}
	}

	if(GetNO(start_x,start_y)->GetType() != NOP_FLAG)
		SetFlag(start_x,start_y,playerid,(route[route.size()-1]+3)%6);

	RoadSegment * rs = new RoadSegment(boat_road?RoadSegment::RT_BOAT:RoadSegment::RT_NORMAL,
		GetSpecObj<noFlag>(tmpx,tmpy),GetSpecObj<noFlag>(start_x,start_y),route);

	GetSpecObj<noFlag>(tmpx,tmpy)->routes[route.front()] = rs;
	GetSpecObj<noFlag>(start_x,start_y)->routes[(route.back()+3)%6] = rs;

	// Der Wirtschaft mitteilen, dass eine neue Straße gebaut wurde, damit sie alles Näcige macht
	GAMECLIENT.GetPlayer(playerid)->NewRoad(rs);

}



bool GameWorldGame::IsObjectionableForRoad(const MapCoord x, const MapCoord y)
{
	if(GetNO(x,y)->GetGOT() == GOT_ENVOBJECT)
	{
		noEnvObject *no = GetSpecObj<noEnvObject>(x,y);
		unsigned short type = no->GetItemID();
		switch(no->GetItemFile())
		{
		case 0xFFFF: // map_?_z.lst
			{
				if(type == 505 || type == 506 || type == 507 || type == 508 || type == 510 || (type >= 542 && type <= 546)
					|| type == 536 || type == 541) // abgeerntete Getreidefelder
				   return true;
			} break;
		case 0:
			{
				// todo:
			} break;
		case 1:
			{
				if(type <= 12)
					return true;
				// todo:
			} break;
		case 2:
			{
				// todo:
			} break;
		case 3:
			{
				// todo:
			} break;
		case 4:
			{
				// todo:
			} break;
		case 5:
			{
				// todo:
			} break;
		}
	}

	return false;
}

void GameWorldGame::DestroyRoad(const MapCoord x, const MapCoord y, const unsigned char dir)
{
	// TODO: Verzögerungsbugabfrage, kann später ggf. weg
	if(!GetSpecObj<noFlag>(x,y))
		return;

	GetSpecObj<noFlag>(x,y)->DestroyRoad(dir);
}


void GameWorldGame::RecalcTerritory(const nobBaseMilitary * const building,const unsigned short radius, const bool destroyed, const bool newBuilt)
{
	list<nobBaseMilitary*> buildings; // Liste von Militärgebäuden in der Nähe

	// alle Militärgebäude in der Nähe abgrasen
	LookForMilitaryBuildings(buildings,building->GetX(),building->GetY(),3);

	// Radius der noch draufaddiert wird auf den eigentlich ausreichenden Bereich, für das Eliminieren von
	// herausragenden Landesteilen und damit Grenzsteinen
	const unsigned ADD_RADIUS = 2;

	// Koordinaten erzeugen für TerritoryRegion (nicht bis ganz an den Rand gehen, da unten noch die Punkte herum geprüft werden!)
	MapCoord x1 = (building->GetX()>radius+ADD_RADIUS) ? (building->GetX()-(radius+ADD_RADIUS)) : 1;
	MapCoord y1 = (building->GetY()>radius+ADD_RADIUS) ? (building->GetY()-(radius+ADD_RADIUS)) : 1;
	MapCoord x2 = (building->GetX()+radius+ADD_RADIUS+1 < width) ? (building->GetX()+(radius+ADD_RADIUS)+1) : width-1;
	MapCoord y2 = (building->GetY()+radius+ADD_RADIUS+1 < height) ? (building->GetY()+(radius+ADD_RADIUS)+1) : height-1;

	TerritoryRegion tr(x1,y1,x2,y2);


	// Alle Gebäude ihr Terrain in der Nähe neu berechnen
	unsigned c=0;

	for(list<nobBaseMilitary*>::iterator it = buildings.end();it.valid();--it)
	{
		++c;
		// Ist es ein richtiges Militärgebäude?
		if((*it)->GetBuildingType() >= BLD_BARRACKS && (*it)->GetBuildingType() <= BLD_FORTRESS)
		{
			// Wenn es noch nicht besetzt war(also gerade neu gebaut), darf es nicht mit einberechnet werden!
			if(static_cast<nobMilitary*>(*it)->IsNewBuilt())
				continue;
		}

		// Wenn das Gebäude abgerissen wird oder wenn es noch nicht besetzt war, natürlich nicht mit einberechnen
		if(*it != building || !destroyed)
			tr.CalcTerritoryOfBuilding(*it);
	}

	// Merkwn, wo sich der Besitzer geändert hat
	bool * owner_changed = new bool[(x2-x1)*(y2-y1)];


	std::vector<int> sizeChanges(GAMECLIENT.GetPlayerCount());
	// Daten von der TR kopieren in die richtige Karte, dabei zus. Grenzen korrigieren und Objekte zerstören, falls
	// das Land davon jemanden anders nun gehört
 	for(MapCoord y = y1;y<y2;++y)
	{
		for(MapCoord x = x1;x<x2;++x)
		{
			unsigned char prev_player,player;
			// Wenn der Punkt den Besitz geändert hat
			if((prev_player=GetNode(x,y).owner) != (player=tr.GetOwner(x,y)))
			{
				// Dann entsprechend neuen Besitzer setzen
				GetNode(x,y).owner = player;
				owner_changed[(x2-x1)*(y-y1)+(x-x1)] = true;
				if (player != 0)
					sizeChanges[player-1]++;
				if (prev_player != 0)
					sizeChanges[prev_player-1]--;
			}
			else
				owner_changed[(x2-x1)*(y-y1)+(x-x1)] = false;
		}
	}

	for (unsigned i=0; i<GAMECLIENT.GetPlayerCount(); ++i)
	{
		GAMECLIENT.GetPlayer(i)->ChangeStatisticValue(STAT_COUNTRY, sizeChanges[i]);

		// Negatives Wachstum per Post dem/der jeweiligen Landesherren/dame melden, nur bei neugebauten Gebäuden
		if (newBuilt && sizeChanges[i] < 0)
		{
			if(GameClient::inst().GetPlayerID() == i)
				GameClient::inst().SendPostMessage(
					new ImagePostMsgWithLocation(_("Lost land by this building"), PMC_MILITARY, building->GetX(), building->GetY(), 
					building->GetBuildingType(), building->GetNation()));
		}
	}

	for(MapCoord y = y1;y<y2;++y)
	{
		for(MapCoord x = x1;x<x2;++x)
		{
			bool isplayerterritory_near = false;
			/// Grenzsteine, die alleine "rausragen" und nicht mit einem richtigen Territorium verbunden sind, raushauen
			for(unsigned d = 0;d<6;++d)
			{
				if(IsPlayerTerritory(GetXA(x,y,d),GetYA(x,y,d)))
				{
					isplayerterritory_near = true;
					break;
				}
			}

			// Wenn kein Land angrenzt, dann nicht nehmen
			if(!isplayerterritory_near)
				GetNode(x,y).owner = 0;

			// Drumherum (da ja Grenzen mit einberechnet werden ins Gebiet, da darf trotzdem nichts stehen) alles vom Spieler zerstören
			// nicht das Militärgebäude oder dessen Flagge nochmal abreißen
			if(owner_changed[(x2-x1)*(y-y1)+(x-x1)])
			{
				for(unsigned char i = 0;i<6;++i)
				{
					unsigned short tx = GetXA(x,y,i), ty = GetYA(x,y,i);

					DestroyPlayerRests(tx,ty,GetNode(x,y).owner,building);

					// BQ neu berechnen
					GetNode(tx,ty).bq = CalcBQ(tx,ty,GAMECLIENT.GetPlayerID());
					// ggf den noch darüber, falls es eine Flagge war (kann ja ein Gebäude entstehen)
					if(GetNodeAround(tx,ty,1).bq)
						SetBQ(GetXA(tx,ty,1),GetYA(tx,ty,1),GAMECLIENT.GetPlayerID());
				}

				if(gi)
					gi->GI_UpdateMinimap(x,y);
			}
		}
	}

	delete [] owner_changed;

	// Grenzsteine neu berechnen, noch 1 über das Areal hinausgehen, da dieses auch die Grenzsteine rundrum
	// mit beeinflusst

	for(int y = y1-3;y < y2+3;++y)
	{
		for(int x = x1-3;x < x2+3;++x)
		{
			// Korrigierte X-Koordinaten (nicht über den Rand gehen)
			MapCoord xc,yc;
			ConvertCoords(x,y,&xc,&yc);

			unsigned char owner = GetNode(xc,yc).owner;

			// Grenzstein direkt auf diesem Punkt?
			if(owner && IsBorderNode(xc,yc,owner))
			{
				GetNode(xc,yc).boundary_stones[0] = owner;

				// Grenzsteine prüfen auf den Zwischenstücken in die 3 Richtungen nach unten und nach rechts
				for(unsigned i = 0;i<3;++i)
				{
					MapCoord xa = GetXA(xc,yc,3+i), ya = GetYA(xc,yc,3+i);
					if(IsBorderNode(xa,ya,owner))
						GetNode(xc,yc).boundary_stones[i+1] = owner;
					else
						GetNode(xc,yc).boundary_stones[i+1] = 0;

				}
			}
			else
			{
				// Kein Grenzstein --> etwaige vorherige Grenzsteine löschen
				for(unsigned i = 0;i<4;++i)
					GetNode(xc,yc).boundary_stones[i] = 0;

				//for(unsigned i = 0;i<3;++i)
				//	GetNodeAround(x,y,3+i).boundary_stones[i+1] = 0;
			}
		}
	}

	// Sichtbarkeiten berechnen
	
	// Wurde es zerstört, müssen die Sichtbarkeiten entsprechend neu berechnet werden, ansonsten reicht es auch
	// sie einfach auf sichtbar zu setzen
	if(destroyed)
		RecalcVisibilitiesAroundPoint(building->GetX(),building->GetY(),building->GetMilitaryRadius()+VISUALRANGE_MILITARY,
			building->GetPlayer(), destroyed ? building : 0);
	else
		SetVisibilitiesAroundPoint(building->GetX(),building->GetY(),building->GetMilitaryRadius()+VISUALRANGE_MILITARY,
			building->GetPlayer());
}

void GameWorldGame::DestroyPlayerRests(const MapCoord x, const MapCoord y, const unsigned char new_player,const nobBaseMilitary * exception)
{
	noBase * no = GetNO(x,y);


	// Flaggen, Gebäude und Baustellen zerstören, aber keine übernommenen und nicht die Ausahme oder dessen Flagge!
	if((no->GetType() == NOP_FLAG || no->GetType() == NOP_BUILDING || no->GetType() == NOP_BUILDINGSITE) && exception != no)
	{
		// Wurde das Objekt auch nicht vom Gegner übernommen?
		if(static_cast<noRoadNode*>(no)->GetPlayer()+1 != new_player)
		{
			// vorher Bescheid sagen
			if(no->GetType() == NOP_FLAG && no != (exception?exception->GetFlag():0))
				static_cast<noFlag*>(no)->DestroyAttachedBuilding();

			no->Destroy();

			delete no;

			return;
		}
	}


	// ggf. Weg kappen
	unsigned char dir;
	noFlag * flag = GetRoadFlag(x,y,dir,0xFF);
	if(flag)
	{
		// Die Ministraße von dem Militärgebäude nich abreißen!
		if(flag->routes[dir]->route.size() == 1)
		{
			if(flag->routes[dir]->f2 == exception)
				return;
		}

		flag->DestroyRoad(dir);
	}
}


bool GameWorldGame::IsNodeForFigures(const MapCoord x, const MapCoord y)
{
	// Nicht über die Kante gehen!
	if(x>=width||y>=height)
		return false;


	// Irgendwelche Objekte im Weg?
	noBase * no = GetNO(x,y);

	if(/*	no->GetType() == NOP_FLAG ||*/
		no->GetType() == NOP_GRANITE ||
		/*no->GetType() == NOP_TREE ||*/
		no->GetType() == NOP_GRAINFIELD ||
		no->GetType() == NOP_BUILDING ||
		no->GetType() == NOP_BUILDINGSITE ||
		no->GetType() == NOP_EXTENSION ||
		no->GetType() == NOP_FIRE ||
		no->GetType() == NOP_OBJECT)
		return false;

	unsigned char t;

	// Terrain untersuchen
	unsigned char good_terrains = 0;
	for(unsigned char i = 0;i<6;++i)
	{
		t = GetTerrainAround(x,y,i);
		if(TERRAIN_BQ[t] == BQ_CASTLE || TERRAIN_BQ[t] == BQ_MINE || TERRAIN_BQ[t] == BQ_FLAG) ++good_terrains;
		else if(TERRAIN_BQ[t] == BQ_DANGER) return false; // in die Nähe von Lava usw. dürfen die Figuren gar nich kommen!
	}

	// Darf nicht im Wasser liegen,
	if(!good_terrains)
		return false;

	return true;
}

void GameWorldGame::RoadNodeAvailable(const MapCoord x, const MapCoord y)
{
	// Figuren direkt daneben
	for(unsigned char i = 0;i<6;++i)
	{
		// Nochmal prüfen, ob er nun wirklich verfügbar ist (evtl blocken noch mehr usw.)
		if(!IsRoadNodeForFigures(x,y,(i+3)%6))
			continue;

		// Koordinaten um den Punkt herum
		MapCoord xa = GetXA(x,y,i), ya = GetYA(x,y,i);

		
		// Figuren Bescheid sagen, es können auch auf den Weg gestoppte sein, die müssen auch berücksichtigt 
		// werden, daher die *From-Methode
		list<noBase*> objects;
		GetDynamicObjectsFrom(xa,ya,objects);

		// Auch Figuren da, die rumlaufen können?
		if(objects.size())
		{
	
			for(list<noBase*>::iterator it = objects.begin();it.valid();++it)
			{
				if((*it)->GetType() == NOP_FIGURE)
					static_cast<noFigure*>(*it)->NodeFreed(x,y);
			}


			//// Achtung: Hier können Iteratoren gelöscht werden in NodeFreed, daher Sicherheitsschleife!
			//list<noBase*>::iterator next_it;
			//for(list<noBase*>::iterator it = GetFigures(xa,ya).begin();
			//	it.valid();it = next_it)
			//{
			//	next_it = it.GetNext();
			//	if((*it)->GetType() == NOP_FIGURE)
			//		static_cast<noFigure*>(*it)->NodeFreed(x,y);
			//}
		}
	}
}



/// Kleine Klasse für Angriffsfunktion für einen potentielle angreifenden Soldaten
struct PotentialAttacker
{
	nofPassiveSoldier * soldier;
	/// Weglänge zum Angriffsziel
	unsigned distance;
};

void GameWorldGame::Attack(const unsigned char player_attacker, const MapCoord x, const MapCoord y, const unsigned short soldiers_count, const bool strong_soldiers)
{
	// Verzögerungsbug-Abfrage:
	// Existiert das angegriffenen Gebäude überhaupt noch?
	if(GetNO(x,y)->GetGOT() != GOT_NOB_MILITARY && GetNO(x,y)->GetGOT() != GOT_NOB_HQ)
		return;

	// Auch noch ein Gebäude von einem Feind (nicht inzwischen eingenommen)?
	if(!GameClient::inst().GetPlayer(player_attacker)->IsPlayerAttackable(GetSpecObj<noBuilding>(x,y)->GetPlayer()))
		return;

	// Prüfen, ob der angreifende Spieler das Gebäude überhaupt sieht (Cheatvorsorge)
	if(CalcWithAllyVisiblity(x,y,player_attacker) != VIS_VISIBLE)
		return;

	// Ist das angegriffenne ein normales Gebäude?
	nobBaseMilitary * attacked_building = GetSpecObj<nobBaseMilitary>(x,y);
	if(attacked_building->GetBuildingType() >= BLD_BARRACKS && attacked_building->GetBuildingType() <= BLD_FORTRESS)
	{
		// Wird es gerade eingenommen?
		if(static_cast<nobMilitary*>(attacked_building)->IsCaptured())
			// Dann darf es nicht angegriffen werden
			return;
	}

	// Militärgebäude in der Nähe finden
	list<nobBaseMilitary*> buildings;
	LookForMilitaryBuildings(buildings,x,y,3);

	// Liste von verfügbaren Soldaten, geordnet einfügen, damit man dann starke oder schwache Soldaten nehmen kann
	list<PotentialAttacker> soldiers;


	for(list<nobBaseMilitary*>::iterator it = buildings.begin();it.valid();++it)
	{
		// Muss ein Gebäude von uns sein und darf nur ein "normales Militärgebäude" sein (kein HQ etc.)
		if((*it)->GetPlayer() == player_attacker && (*it)->GetBuildingType() >= BLD_BARRACKS && (*it)->GetBuildingType() <= BLD_FORTRESS)
		{

			// Soldaten ausrechnen, wie viel man davon nehmen könnte, je nachdem wie viele in den
			// Militäreinstellungen zum Angriff eingestellt wurden
			unsigned short soldiers_count =
				(static_cast<nobMilitary*>(*it)->GetTroopsCount()>1)?
				((static_cast<nobMilitary*>(*it)->GetTroopsCount()-1)*GAMECLIENT.GetPlayer(player_attacker)->military_settings[3]/5):0;

      unsigned int distance = CalcDistance(x,y,(*it)->GetX(),(*it)->GetY());

      // Falls Entfernung größer als Basisreichweite, Soldaten subtrahieren
      if (distance > BASE_ATTACKING_DISTANCE)
      {
        // je einen soldaten zum entfernen vormerken für jeden EXTENDED_ATTACKING_DISTANCE großen Schritt
        unsigned short soldiers_to_remove = ((distance - BASE_ATTACKING_DISTANCE + EXTENDED_ATTACKING_DISTANCE - 1) / EXTENDED_ATTACKING_DISTANCE);
        if (soldiers_to_remove < soldiers_count)
          soldiers_count -= soldiers_to_remove;
        else
          continue;
      }

			if(soldiers_count)
			{
				// und auch der Weg zu Fuß darf dann nicht so weit sein, wenn das alles bestanden ist, können wir ihn nehmen..
				// Bei dem freien Pfad noch ein bisschen Toleranz mit einberechnen
				if(FindHumanPath(x,y,(*it)->GetX(),(*it)->GetY(),MAX_ATTACKING_RUN_DISTANCE) != 0xFF) // TODO check: hier wird ne random-route berechnet? soll das so?
				{
					// Soldaten davon nehmen
					unsigned i = 0;
					for(list<nofPassiveSoldier*>::iterator it2 = strong_soldiers?
						static_cast<nobMilitary*>(*it)->troops.end():static_cast<nobMilitary*>(*it)->troops.begin()
						;it2.valid()&&i<soldiers_count;++i)
					{
						// Sortiert einfügen (aufsteigend nach Rang)

						unsigned old_size = soldiers.size();

						for(list<PotentialAttacker>::iterator it3 = soldiers.end(); it3.valid(); --it3)
						{
							// Ist das einzufügende Item größer als das aktuelle?
							// an erster Stelle nach Rang, an zweiter dann nach Entfernung gehen
							if( (it3->soldier->GetRank() < (*it2)->GetRank() && !strong_soldiers) ||
								(it3->soldier->GetRank() > (*it2)->GetRank() && strong_soldiers) ||
								(it3->soldier->GetRank() == (*it2)->GetRank() && it3->distance > distance))
							{
								// ja dann hier einfügen
								PotentialAttacker pa = { *it2,distance };
								soldiers.insert(it3, pa);
								break;
							}
						}

						// Ansonsten ganz nach vorn einfügen, wenns noch nich eingefügt wurde
						if(old_size == soldiers.size())
						{
							PotentialAttacker pa = { *it2,distance };
							soldiers.push_front(pa);
						}

						if(strong_soldiers)
							--it2;
						else
							++it2;
					}
				}
			}
		}
	}

	// Alle Soldaten zum Angriff schicken (jeweils wieder von hinten oder vorne durchgehen um schwache oder starke
	// Soldaten zu nehmen)
	unsigned short i = 0;

	for(list<PotentialAttacker>::iterator it = soldiers.begin();
		it.valid()&&i<soldiers_count;++i,++it)
	{
		// neuen Angreifer-Soldaten erzeugen
		new nofAttacker(it->soldier,attacked_building);
		// passiven Soldaten entsorgen
		it->soldier->Destroy();
		delete it->soldier;
	}

	/*if(!soldiers.size())
		LOG.lprintf("GameWorldGame::Attack: WARNING: Attack failed. No Soldiers available!\n");*/
}


bool GameWorldGame::IsRoadNodeForFigures(const MapCoord x, const MapCoord y,const unsigned char dir)
{
	/// Objekte sammeln
	list<noBase*> objects;
	GetDynamicObjectsFrom(x,y,objects);

	// Figuren durchgehen, bei Kämpfen und wartenden Angreifern sowie anderen wartenden Figuren stoppen!
	for(list<noBase*>::iterator it = objects.begin();it.valid();++it)
	{
		// andere wartende Figuren
		if((*it)->GetType() == NOP_FIGURE)
		{
			noFigure * fig = static_cast<noFigure*>(*it);
			// Figuren dürfen sich nicht gegenüber stehen, sonst warten sie ja ewig aufeinander
			// Außerdem muss auch die Position stimmen, sonst spinnt der ggf. rum, da 
			if(fig->IsWaitingForFreeNode() && (fig->GetDir()+3)%6 != dir)
				return false;
		}

		// Kampf
		if((*it)->GetGOT() == GOT_FIGHTING)
		{
			if(static_cast<noFighting*>(*it)->IsActive())
				return false;
		}

		//// wartende Angreifer
		if((*it)->GetGOT() == GOT_NOF_ATTACKER)
		{
			if(static_cast<nofAttacker*>(*it)->IsBlockingRoads())
				return false;
		}
	}

	// alles ok
	return true;
}

/// Lässt alle Figuren, die auf diesen Punkt  auf Wegen zulaufen, anhalten auf dem Weg (wegen einem Kampf)
void GameWorldGame::StopOnRoads(const MapCoord x, const MapCoord y, const unsigned char dir)
{
	// Figuren drumherum sammeln (auch von dem Punkt hier aus)
	list<noBase*> figures;

	// Auch vom Ausgangspunkt aus, da sie im GameWorldGame wegem Zeichnen auch hier hängen können!
	for(list<noBase*>::iterator it = GetFigures(x,y).begin();it.valid();++it)
		if((*it)->GetType() == NOP_FIGURE)
			figures.push_back(*it);

	// Und natürlich in unmittelbarer Umgebung suchen
	for(unsigned d = 0;d<6;++d)
	{
		for(list<noBase*>::iterator it = GetFigures(GetXA(x,y,d),GetYA(x,y,d)).begin()
			;it.valid();++it)
			if((*it)->GetType() == NOP_FIGURE)
				figures.push_back(*it);
	}

	for(list<noBase*>::iterator it = figures.begin();it.valid();++it)
	{
		if(dir <6)
		{
			if((dir+3)%6 == static_cast<noFigure*>(*it)->GetDir())
			{
				if(GetXA(x,y,dir) == static_cast<noFigure*>(*it)->GetX() &&
					GetYA(x,y,dir) == static_cast<noFigure*>(*it)->GetY())
					continue;
			}
		}

		// Derjenige muss ggf. stoppen, wenn alles auf ihn zutrifft
		static_cast<noFigure*>(*it)->StopIfNecessary(x,y);
	}
}

void GameWorldGame::Armageddon()
{
	for(unsigned i = 0;i < map_size; ++i)
	{
		if(nodes[i].obj)
		{
			if(nodes[i].obj->GetGOT() == GOT_FLAG)
			{
				noFlag * flag = static_cast<noFlag*>(nodes[i].obj);
				nodes[i].obj = 0;
				flag->DestroyAttachedBuilding();
				flag->Destroy();
				delete flag;
			}
		}
	}
}

void GameWorldGame::Armageddon(const unsigned char player)
{
	for(unsigned i = 0;i < map_size; ++i)
	{
		if(nodes[i].obj)
		{
			if(nodes[i].obj->GetGOT() == GOT_FLAG)
			{
				noFlag * flag = static_cast<noFlag*>(nodes[i].obj);
        if (flag->GetPlayer() == player)
        {
				  nodes[i].obj = 0;
				  flag->DestroyAttachedBuilding();
				  flag->Destroy();
				  delete flag;
        }
			}
		}
	}
}



bool GameWorldGame::ValidWaitingAroundBuildingPoint(const MapCoord x, const MapCoord y, nofAttacker * attacker)
{
	// Gültiger Punkt für Figuren?
	if(!IsNodeForFigures(x,y))
		return false;

	// Objekte, die sich hier befinden durchgehen
	for(list<noBase*>::iterator it = GetFigures(x,y).begin();it.valid();++it)
	{
		// Ist hier ein anderer Soldat, der hier ebenfalls wartet?
		if((*it)->GetGOT() == GOT_NOF_ATTACKER || (*it)->GetGOT() == GOT_NOF_AGGRESSIVEDEFENDER || 
			(*it)->GetGOT() == GOT_NOF_DEFENDER)
		{
			if(static_cast<nofActiveSoldier*>(*it)->GetState() == nofActiveSoldier::STATE_WAITINGFORFIGHT ||
				static_cast<nofActiveSoldier*>(*it)->GetState() == nofActiveSoldier::STATE_ATTACKING_WAITINGAROUNDBUILDING )
				return false;
		}

		// Oder ein Kampf, der hier tobt?
		if((*it)->GetGOT() == GOT_FIGHTING)
			return false;
	}

	return true;
}

bool GameWorldGame::ValidPointForFighting(const MapCoord x, const MapCoord y)
{
	// Objekte, die sich hier befinden durchgehen
	for(list<noBase*>::iterator it = GetFigures(x,y).begin();it.valid();++it)
	{
		// Ist hier ein anderer Soldat, der hier ebenfalls wartet?
		if((*it)->GetGOT() == GOT_NOF_ATTACKER || (*it)->GetGOT() == GOT_NOF_AGGRESSIVEDEFENDER || 
			(*it)->GetGOT() == GOT_NOF_DEFENDER)
		{
			switch(static_cast<nofActiveSoldier*>(*it)->GetState())
			{
			default: break;
			case nofActiveSoldier::STATE_WAITINGFORFIGHT:
			case nofActiveSoldier::STATE_ATTACKING_WAITINGAROUNDBUILDING:
			case nofActiveSoldier::STATE_ATTACKING_WAITINGFORDEFENDER:
			case nofActiveSoldier::STATE_DEFENDING_WAITING:
				return false;
			}
		}

		// Oder ein Kampf, der hier tobt?
		if((*it)->GetGOT() == GOT_FIGHTING)
		{
			if(static_cast<noFighting*>(*it)->IsActive())
			return false;
		}
	}

	return true;
}

bool GameWorldGame::IsPointCompletelyVisible(const MapCoord x, const MapCoord y, const unsigned char player, const noBuilding * const exception) const
{ 
	list<nobBaseMilitary*> buildings;
	LookForMilitaryBuildings(buildings,x,y,3);

	// Sichtbereich von Militärgebäuden
	for(list<nobBaseMilitary*>::iterator it = buildings.begin();it.valid();++it)
	{
		if((*it)->GetPlayer() == player && *it != exception)
		{
			// Prüfen, obs auch unbesetzt ist
			if((*it)->GetGOT() == GOT_NOB_MILITARY)
			{
				if(static_cast<nobMilitary*>(*it)->IsNewBuilt())
					continue;
			}

			if(CalcDistance(x,y,(*it)->GetX(),(*it)->GetY()) 
				<= unsigned((*it)->GetMilitaryRadius()+VISUALRANGE_MILITARY))
				return true;
		}
	}

	// Sichtbereich von Spähtürmen

	for(std::list<nobUsual*>::const_iterator it = GameClient::inst().GetPlayer(player)->GetBuildings(BLD_LOOKOUTTOWER).begin();
		it!=GameClient::inst().GetPlayer(player)->GetBuildings(BLD_LOOKOUTTOWER).end(); ++it)
	{
		// Ist Späturm überhaupt besetzt?
		if(!(*it)->HasWorker())
			continue;

		// Nicht die Ausnahme wählen
		if(*it == exception)
			continue;

		// Liegt Spähturm innerhalb des Sichtradius?
		if(CalcDistance(x,y,(*it)->GetX(),(*it)->GetY()) <= VISUALRANGE_LOOKOUTTOWER)
				return true;
	}

	// Erkunder prüfen

	// Zunächst auf dem Punkt selbst
	if(IsScoutingFigureOnNode(x,y,player,0))
		return true;

	// Und drumherum
	for(MapCoord tx=GetXA(x,y,0), r = 1;r<=VISUALRANGE_SCOUT;tx=GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;GetPointA(tx2,ty2,i%6),++r2)
			{
				if(IsScoutingFigureOnNode(tx2,ty2,player,r))
					return true;
			}
		}
	}



	return false;

	///// Auf eigenem Terrain --> sichtbar
	//if(GetNode(x,y).owner == player+1)
	//	visible = true;
}

bool GameWorldGame::IsScoutingFigureOnNode(const MapCoord x, const MapCoord y, const unsigned player, const unsigned distance) const
{
	list<noBase*> objects;
	GetDynamicObjectsFrom(x,y,objects);

	// Späher/Soldaten in der Nähe prüfen und direkt auf dem Punkt
	for(list<noBase*>::iterator it = objects.begin();it.valid();++it)
	{
		// Späher?
		if((*it)->GetGOT() == GOT_NOF_SCOUT_FREE)
		{
			// Prüfen, ob er auch am Erkunden ist und an der Position genau und ob es vom richtigen Spieler ist
			nofScout_Free* scout = dynamic_cast<nofScout_Free*>(*it);
			if(scout->GetX() == x && scout->GetY() == y && scout->GetPlayer() == player)
				return true;
		}

		// Soldaten?
		if(distance <= VISUALRANGE_SOLDIER)
		{
			// Soldaten?
			if((*it)->GetGOT() == GOT_NOF_ATTACKER || (*it)->GetGOT() == GOT_NOF_AGGRESSIVEDEFENDER)
			{
				nofActiveSoldier * soldier = dynamic_cast<nofActiveSoldier*>(*it);
				if(soldier->GetX() == x && soldier->GetY() == y && soldier->GetPlayer() == player)
					return true;
			}
			// Kämpfe (wo auch Soldaten drin sind)
			else if((*it)->GetGOT() == GOT_FIGHTING)
			{
				// Prüfen, ob da ein Soldat vom angegebenen Spieler dabei ist
				if(dynamic_cast<noFighting*>(*it)->IsSoldierOfPlayer(player))
					return true;
			}
		}
	}

	return false;
}

void GameWorldGame::RecalcVisibility(const MapCoord x, const MapCoord y, const unsigned char player, const noBuilding * const exception)
{
	///// Bei völlig ausgeschalteten Nebel muss nur das erste Mal alles auf sichtbar gesetzt werden
	//if(GameClient::inst().GetGGS().exploration == GlobalGameSettings::EXP_DISABLED && !update_terrain)
	//	GetNode(x,y).fow[player].visibility = VIS_VISIBLE;
	//else if(GameClient::inst().GetGGS().exploration == GlobalGameSettings::EXP_DISABLED && update_terrain)
	//	return;

	/// Zustand davor merken
	Visibility visibility_before = GetNode(x,y).fow[player].visibility;

	/// Herausfinden, ob vollständig sichtbar
	bool visible = IsPointCompletelyVisible(x,y,player,exception);

	// Vollständig sichtbar --> vollständig sichtbar logischerweise
	if(visible)
	{
		GetNode(x,y).fow[player].visibility = VIS_VISIBLE;

		// Etwaige FOW-Objekte zerstören
		delete GetNode(x,y).fow[player].object;
		GetNode(x,y).fow[player].object = NULL;
	}
	else
	{
		// nicht mehr sichtbar
		// Je nach vorherigen Zustand und Einstellung entscheiden
		switch(GameClient::inst().GetGGS().exploration)
		{
		default: assert(false);
		case GlobalGameSettings::EXP_DISABLED:
		case GlobalGameSettings::EXP_CLASSIC:
			{
				// einmal sichtbare Bereiche bleiben erhalten
				// nichts zu tun
			} break;
		case GlobalGameSettings::EXP_FOGOFWAR:
		case GlobalGameSettings::EXP_FOGOFWARE_EXPLORED:
			{
				// wenn es mal sichtbar war, nun im Nebel des Krieges
				if(visibility_before == VIS_VISIBLE)
				{
					GetNode(x,y).fow[player].visibility = VIS_FOW;

					SaveFOWNode(x,y,player);
				}
			} break;
		}

	}

	// Minimap Bescheid sagen
	if(gi && visibility_before != GetNode(x,y).fow[player].visibility)
		gi->GI_UpdateMinimap(x,y);

	// Lokaler Spieler oder Verbündeter (wenn Team-Sicht an ist)? --> Terrain updaten
	if(player == GameClient::inst().GetPlayerID() || 
		(GameClient::inst().GetGGS().team_view && GameClient::inst().GetLocalPlayer()->IsAlly(player)))
		VisibilityChanged(x,y);
}

void GameWorldGame::SetVisibility(const MapCoord x, const MapCoord y,  const unsigned char player)
{
	Visibility visibility_before = GetNode(x,y).fow[player].visibility;
	GetNode(x,y).fow[player].visibility = VIS_VISIBLE;

	// Etwaige FOW-Objekte zerstören
	delete GetNode(x,y).fow[player].object;
	GetNode(x,y).fow[player].object = NULL;

	// Minimap Bescheid sagen
	if(gi && visibility_before != GetNode(x,y).fow[player].visibility)
		gi->GI_UpdateMinimap(x,y);

	// Lokaler Spieler oder Verbündeter (wenn Team-Sicht an ist)? --> Terrain updaten
	if(player == GameClient::inst().GetPlayerID() || 
		(GameClient::inst().GetGGS().team_view && GameClient::inst().GetLocalPlayer()->IsAlly(player)))
		VisibilityChanged(x,y);
}





void GameWorldGame::RecalcVisibilitiesAroundPoint(const MapCoord x, const MapCoord y, const MapCoord radius, const unsigned char player, const noBuilding * const exception)
{
	RecalcVisibility(x,y,player,exception);

	for(MapCoord tx=GetXA(x,y,0), r=1;r<=radius;tx=GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;GetPointA(tx2,ty2,i%6),++r2)
				RecalcVisibility(tx2,ty2,player,exception);
		}
	}
}

/// Setzt die Sichtbarkeiten um einen Punkt auf sichtbar (aus Performancegründen Alternative zu oberem)
void GameWorldGame::SetVisibilitiesAroundPoint(const MapCoord x, const MapCoord y, const MapCoord radius, const unsigned char player)
{
	SetVisibility(x,y,player);

	for(MapCoord tx=GetXA(x,y,0), r=1;r<=radius;tx=GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;GetPointA(tx2,ty2,i%6),++r2)
				SetVisibility(tx2,ty2,player);
		}
	}
}

void GameWorldGame::SaveFOWNode(const MapCoord x, const MapCoord y, const unsigned player)
{
	// FOW-Objekt erzeugen, falls hier ein richtiges Objekt existiert
	noBase * obj = GetNode(x,y).obj;
	if(obj)
	{
		delete GetNode(x,y).fow[player].object;
		GetNode(x,y).fow[player].object = obj->CreateFOWObject();
	}

	// Wege speichern, aber nur richtige, keine, die gerade gebaut werden
	for(unsigned i = 0;i<3;++i)
	{
		if(GetNode(x,y).roads_real[i])
			GetNode(x,y).fow[player].roads[i] = GetNode(x,y).roads[i];
		else
			GetNode(x,y).fow[player].roads[i] = 0;
	}

	// Besitzverhältnisse speichern, damit auch die Grenzsteine im FoW gezeichnet werden können
	GetNode(x,y).fow[player].owner = GetNode(x,y).owner;
	// Grenzsteine merken
	for(unsigned i =0;i<4;++i)
		GetNode(x,y).fow[player].boundary_stones[i] = GetNode(x,y).boundary_stones[i];
}

/// Stellt fest, ob auf diesem Punkt ein Grenzstein steht (ob das Grenzgebiet ist)
bool GameWorldGame::IsBorderNode(const MapCoord x, const MapCoord y, const unsigned char player) const
{
	// Wenn ich Besitzer des Punktes bin, dieser mir aber nicht gehört
	return (GetNode(x,y).owner == player && !IsPlayerTerritory(x,y));
}

/// Berechnet für alle Hafenpunkt jeweils die Richtung und Entfernung zu allen anderen Hafenpunkten
/// über die Kartenränder hinweg
void GameWorldGame::CalcHarborPosNeighbors()
{
	Point<int> diffs[9] =
	{
		Point<int>(0,0),
		Point<int>(-width,0),
		Point<int>(width,0),
		Point<int>(0,height),
		Point<int>(-width,height),
		Point<int>(width,height),
		Point<int>(0,-height),
		Point<int>(-width,-height),
		Point<int>(width,-height)
	};


	for(unsigned i = 0;i<harbor_pos.size();++i)
	{
		for(unsigned z = 0;z<harbor_pos.size();++z)
		{
			if(i == z)
				continue;

			for(unsigned o = 0;o<9;++o)
			{
				MapCoord ox = harbor_pos[i].x + diffs[o].x,
					oy = harbor_pos[i].y + diffs[o].y;
				// Richtung bestimmen, in der dieser Punkt relativ zum Ausgangspunkt steht
				unsigned char dir;
				
				unsigned diff = SafeDiff(oy,harbor_pos[z].y);
				if(!diff)
					diff = 1;
				// Oben?
				bool marginal_x = ((SafeDiff(ox,harbor_pos[z].x) * 1000 / diff) < 87);
				if(harbor_pos[z].y < oy)
				{
					if(marginal_x)
						dir = 0;
					else if(harbor_pos[z].x < ox)
						dir = 5;
					else if(harbor_pos[z].x > ox)
						dir = 1;
				}
				else
				{
					if(marginal_x)
						dir = 3;
					else if(harbor_pos[z].x < ox)
						dir = 4;
					else if(harbor_pos[z].x > ox)
						dir = 2;
				}

				GameWorldBase::HarborPos::Neighbor nb 
					= { z+1, CalcDistance(ox,oy,harbor_pos[z].x,harbor_pos[z].y) };
				harbor_pos[i].neighbors[dir].push_back(nb);
			}
		}

		// Nach Entfernung sortieren
		for(unsigned dir = 0;dir<6;++dir)
			std::sort(harbor_pos[i].neighbors[dir].begin(),
			harbor_pos[i].neighbors[dir].begin() + harbor_pos[i].neighbors[dir].size());


	}
}
