// $Id: GameWorldViewer.cpp 6305 2010-04-10 21:53:00Z OLiver $
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
#include "GameWorld.h"
#include "VideoDriverWrapper.h"
#include "glArchivItem_Map.h"
#include "noTree.h"
#include "nobUsual.h"
#include "nobMilitary.h"
#include "noBuildingSite.h"
#include "CatapultStone.h"
#include "GameClient.h"
#include "SoundManager.h"
#include "MapGeometry.h"
#include "MapConsts.h"
#include "dskGameInterface.h"
#include "FOWObjects.h"
#include "noShip.h"
#include "AddonManager.h"
#include "Settings.h"

#include "GameServer.h"
#include "AIPlayerJH.h"

GameWorldViewer::GameWorldViewer() : show_bq(false), show_names(false), show_productivity(false),
xoffset(0), yoffset(0), selx(0), sely(0), sx(0), sy(0), scroll(false), last_xoffset(0), last_yoffset(0), show_coordinates(false), d_what(0), d_player(0), d_active(false) 
{
	displayWidth  = VideoDriverWrapper::inst().GetScreenWidth();
	displayHeight = VideoDriverWrapper::inst().GetScreenHeight();
}

struct ObjectBetweenLines
{
	noBase * obj;
	Point<int> pos; // Zeichenposition
	
	ObjectBetweenLines(noBase * const obj, const Point<int> pos) : obj(obj), pos(pos) {}
};

void GameWorldViewer::Draw(const unsigned char player, unsigned * water, const bool draw_selected, const MapCoord selected_x, const MapCoord selected_y,const RoadsBuilding& rb)
{

	int shortest_len = 100000;
	//if(//GUIResources::inst().action.IsActive())
	//	shortest_len = 0;

	tr.Draw(this,water);

	// Draw-Counter der Bäume zurücksetzen vor jedem Zeichnen
	noTree::ResetDrawCounter();

	for(int y = fy;y<ly;++y)
	{
		// Figuren speichern, die in dieser Zeile gemalt werden m�ssen
		// und sich zwischen zwei Zeilen befinden, da sie dazwischen laufen
		std::vector<ObjectBetweenLines> between_lines;
		
		for(int x = fx;x<lx;++x)
		{
			unsigned short tx,ty;
			int xo,yo;
			tr.ConvertCoords(x,y,tx,ty,&xo,&yo);


			int xpos = int(tr.GetTerrainX(tx,ty))-xoffset+xo;
			int ypos = int(tr.GetTerrainY(tx,ty))-yoffset+yo;

			if(abs(VideoDriverWrapper::inst().GetMouseX() - static_cast<int>(xpos)) + abs(VideoDriverWrapper::inst().GetMouseY() - static_cast<int>(ypos)) < shortest_len)
			{
				selx = tx;
				sely = ty;
				selxo = xo;
				selyo = yo;
				shortest_len = abs(VideoDriverWrapper::inst().GetMouseX() - static_cast<int>(xpos)) + abs(VideoDriverWrapper::inst().GetMouseY() - static_cast<int>(ypos));
			}

			Visibility visibility = GetVisibility(tx,ty);

			DrawBoundaryStone(x,y,tx,ty,xpos,ypos, visibility);

			/// Nur bei Sichtbaren Stellen zeichnen
			if(visibility == VIS_VISIBLE)
			{
				////////////////////////////////////////////////

				///////////////////////////////////////
				//// Figuren und Menschen malen

				// Knotenobjekt zeichnen
				MapNode& mn = GetNode(tx,ty);
				if(mn.obj)
					mn.obj->Draw(static_cast<int>(xpos),static_cast<int>(ypos));


				// Menschen
				if(mn.figures.size())
				{
					for(list<noBase*>::iterator it = mn.figures.begin(); it.valid(); ++it)
					{
						// Bewegt er sich oder ist es ein Schiff?
						if((*it)->IsMoving() || (*it)->GetGOT() == GOT_SHIP)
							// Dann nach der gesamten Zeile zeichnen
							between_lines.push_back(ObjectBetweenLines(*it,Point<int>(static_cast<int>(xpos),static_cast<int>(ypos))));
						else
							// Ansonsten jetzt schon zeichnen
							(*it)->Draw(static_cast<int>(xpos),static_cast<int>(ypos));
					}
				}

				////////////////////////////////////////////////

				if(show_bq && GetNode(tx,ty).bq && GetNode(tx,ty).bq < 7)
				LOADER.GetMapImageN(49+GetNode(tx,ty).bq)->Draw(static_cast<int>(xpos),static_cast<int>(ypos), 0, 0, 0, 0, 0, 0);
			}
			// im Nebel die FOW-Objekte zeichnen
			else if(visibility == VIS_FOW)
			{
				FOWObject *fowobj = GetNode(tx,ty).fow[GameClient::inst().GetPlayerID()].object;
				if(fowobj)
					fowobj->Draw(static_cast<int>(xpos),static_cast<int>(ypos));
			}


			if(show_coordinates)
			{
				char high[32];
				//*sprintf(high,"%d",unsigned(GetNode(tx,ty).altitude));*/
				sprintf(high,"%d;%d",tx,ty);
				//*sprintf(high,"%X",unsigned(GetNode(tx,ty))).resources;*/
				//sprintf(high,"%u",GetNode(tx,ty)).reserved;
				NormalFont->Draw(static_cast<int>(xpos),static_cast<int>(ypos),high,0,0xFFFFFF00);

				if(GetNode(tx,ty).harbor_id)
				{
					sprintf(high,"%u",GetNode(tx,ty).harbor_id);
					NormalFont->Draw(static_cast<int>(xpos),static_cast<int>(ypos),high,0,0xFFFF0000);
				}

			}

			if (d_active)
			{
				std::stringstream ss;
				AIPlayerJH *ai = dynamic_cast<AIPlayerJH *>(GameServer::inst().GetAIPlayer(d_player));
				if (ai)
				{
					if (d_what == 1)
					{
						if(ai->GetAINode(tx, ty).bq && ai->GetAINode(tx, ty).bq  < 7)
							LOADER.GetMapImageN(49+ai->GetAINode(tx, ty).bq)->Draw(static_cast<int>(xpos),static_cast<int>(ypos), 0, 0, 0, 0, 0, 0);
					}
					else if (d_what == 2)
					{
						if (ai->GetAINode(tx, ty).reachable)
							LOADER.GetImageN("io", 32)->Draw(static_cast<int>(xpos),static_cast<int>(ypos), 0, 0, 0, 0, 0, 0);
						else
							LOADER.GetImageN("io", 40)->Draw(static_cast<int>(xpos),static_cast<int>(ypos), 0, 0, 0, 0, 0, 0);
					}
					else if (d_what == 3)
					{
						if (ai->GetAINode(tx, ty).farmed)
						LOADER.GetImageN("io", 32)->Draw(static_cast<int>(xpos),static_cast<int>(ypos), 0, 0, 0, 0, 0, 0);
					else
						LOADER.GetImageN("io", 40)->Draw(static_cast<int>(xpos),static_cast<int>(ypos), 0, 0, 0, 0, 0, 0);
					}
					else if (d_what > 3 && d_what < 13)
					{
						ss << ai->GetResMapValue(tx, ty, AIJH::Resource(d_what-4));
						NormalFont->Draw(static_cast<int>(xpos),static_cast<int>(ypos),ss.str().c_str(),0,0xFFFFFF00);
					}
				}
			}
		}
		
		// Figuren zwischen den Zeilen zeichnen
		for(unsigned i = 0;i<between_lines.size();++i)
			between_lines[i].obj->Draw(between_lines[i].pos.x,between_lines[i].pos.y);
	}

	if(show_names || show_productivity)
	{
		for(int x = fx; x < lx; ++x)
		{
			for(int y = fy; y < ly; ++y)
			{
				// Koordinaten transformieren
				unsigned short tx,ty;
				int xo,yo;
				tr.ConvertCoords(x,y,tx,ty,&xo,&yo);

				int xpos = (int)(tr.GetTerrainX(tx,ty) - xoffset +xo);
				int ypos = (int)(tr.GetTerrainY(tx,ty) - yoffset +yo);

				// Name bzw Produktivität anzeigen
				GO_Type got = GetNO(tx, ty)->GetGOT();
				if(IsBaseBuilding(got))
				{
					noBaseBuilding *no = GetSpecObj<noBaseBuilding>(tx, ty);

					// nur eigene Objekte anzeigen
					if(no->GetPlayer() != GAMECLIENT.GetPlayerID())
						continue;

					int py = ypos - 10;

					// Name von Objekt zeichnen
					if(show_names)
					{
						unsigned int color = (no->GetGOT() == GOT_BUILDINGSITE) ? COLOR_GREY : COLOR_YELLOW;
						SmallFont->Draw(xpos, py, _(BUILDING_NAMES[no->GetBuildingType()]), glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_VCENTER, color);
						py += SmallFont->getHeight();
					}

						

					if(show_productivity)
					{
						// Baustelle?
						if(got == GOT_BUILDINGSITE)
						{
							noBuildingSite *n = static_cast<noBuildingSite*>(no);
							if(n)
							{
								char text[256];
								unsigned int color = COLOR_GREY;

								unsigned short p = n->GetBuildProgress();
								snprintf(text, 256, "(%d %%)", p);
								SmallFont->Draw(xpos, py, text, glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_VCENTER, color);
							}
						}
						// Normales Gebäude?
						else if(got == GOT_NOB_USUAL || got == GOT_NOB_SHIPYARD)
						{
							nobUsual *n = dynamic_cast<nobUsual*>(no);
							if(n)
							{
								char text[256];
								unsigned int color = COLOR_RED;

								if(!n->HasWorker())
									snprintf(text, 256, "%s", _("(House unoccupied)"));
								else if(n->IsProductionDisabledVirtual())
									snprintf(text, 256, "%s", _("(stopped)"));
								else
								{
									// Bei Katapult und Spähturm keine Produktivität anzeigen!
									if(n->GetBuildingType() == BLD_CATAPULT || n->GetBuildingType() == BLD_LOOKOUTTOWER)
										text[0] = 0;
									else
									{
										unsigned short p = *n->GetProduktivityPointer();
										snprintf(text, 256, "(%d %%)", p);
										if(p >= 60)
											color = 0xFF00E000;
										else if(p >= 30)
											color = 0xFFFFFF00;
										else if(p >= 20)
											color = 0xFFFF8000;
									}
								}
								SmallFont->Draw(xpos, py, text, glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_VCENTER, color);
							}
						}
						// Militärgebäude?
						else if(got == GOT_NOB_MILITARY)
						{
							
				
							// Dann kommt noch die Soldatenanzahl drunter
							char str[64];
							if(static_cast<nobMilitary*>(no)->GetTroopsCount() == 1)
								strcpy(str,_("(1 soldier)"));
							else
								sprintf(str,_("(%d soldiers)"),static_cast<nobMilitary*>(no)->GetTroopsCount());


							SmallFont->Draw(xpos, py, str, glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_VCENTER, COLOR_YELLOW);
							py += SmallFont->getHeight();
						}
					}
				}
			}

					
		}
	}

	/// GUI-Symbole auf der Map zeichnen

	/// Falls im Straßenbaumodus: Punkte um den aktuellen Straßenbaupunkt herum ermitteln
	unsigned short road_points[6*2];

	if(rb.mode)
	{
		for(unsigned i = 0; i < 6; ++i)
		{
			road_points[i*2] = GetXA(rb.point_x, rb.point_y,i);
			road_points[i*2+1] = GetYA(rb.point_x, rb.point_y,i);
		}
	}

	for(int x = fx; x < lx; ++x)
	{
		for(int y = fy; y < ly; ++y)
		{
			// Koordinaten transformieren
			unsigned short tx,ty;
			int xo,yo;
			tr.ConvertCoords(x,y,tx,ty,&xo,&yo);

			int xpos = (int)(tr.GetTerrainX(tx,ty) - xoffset +xo);
			int ypos = (int)(tr.GetTerrainY(tx,ty) - yoffset +yo);

			/// Aktueller Punkt unter der Maus
			if(selx == tx && sely == ty)
			{
				// Mauszeiger am boten
				unsigned mid = 22;
				switch(GetNode(tx,ty).bq)
				{
				case BQ_FLAG: mid = 40; break;
				case BQ_MINE: mid = 41; break;
				case BQ_HUT: mid = 42; break;
				case BQ_HOUSE: mid = 43; break;
				case BQ_CASTLE: mid = 44; break;
				case BQ_HARBOR: mid = 45; break;
				default: break;
				}

				if(rb.mode)
					mid = 22;

				LOADER.GetMapImageN(mid)->Draw(xpos,ypos, 0, 0, 0, 0, 0, 0);
			}

			/// Aktuell selektierter Punkt
			if(draw_selected && selected_x == tx && selected_y == ty)
				LOADER.GetMapImageN(20)->Draw(xpos,ypos, 0, 0, 0, 0, 0, 0);

			// Wegbauzeug
			if(rb.mode)
			{
				if(rb.point_x == tx && rb.point_y == ty)
					LOADER.GetMapImageN(21)->Draw(xpos,ypos, 0, 0, 0, 0, 0, 0);



				int altitude = GetNode(rb.point_x,rb.point_y).altitude;

				const unsigned char waterway_lengthes[] = {3, 5, 9, 13, 21, 0}; // these are written into dskGameInterface.cpp, too
				const unsigned char index = ADDONMANAGER.getSelection(ADDON_MAX_WATERWAY_LENGTH);
				assert(index <= sizeof(waterway_lengthes) - 1);
				const unsigned char max_length = waterway_lengthes[index];

				for(unsigned i = 0;i<6;++i)
				{
					if(road_points[i*2] == tx  && road_points[i*2+1] == ty)
					{
						// test on maximal water way length
						if(rb.mode != RM_BOAT || rb.route.size() < max_length || max_length == 0 )
						{
							if( ( (RoadAvailable(rb.mode == RM_BOAT,tx,ty,i) 
							       && GetNode(tx,ty).owner-1 == (signed)GAMECLIENT.GetPlayerID())
							     || (GetNode(tx,ty).bq == BQ_FLAG) )
							   && IsPlayerTerritory(tx,ty) )
							{
								unsigned short id = 60;
								switch(int(GetNode(tx,ty).altitude)-altitude)
								{
								case 1: id = 61; break;
								case 2: case 3: id = 62; break;
								case 4: case 5: id = 63; break;
								case -1: id = 64; break;
								case -2: case -3: id = 65; break;
								case -4: case -5: id = 66; break;
								}

								LOADER.GetMapImageN(id)->Draw(xpos,ypos, 0, 0, 0, 0, 0, 0);
							}

							if(GetNO(tx,ty))
							{
								// Flaggenanschluss? --> extra zeichnen
								if(GetNO(tx,ty)->GetType() == NOP_FLAG && !(tx == rb.start_x && ty == rb.start_y))
									LOADER.GetMapImageN(20)->Draw(xpos,ypos, 0, 0, 0, 0, 0, 0);
							}
						}

						if(rb.route.size())
						{
							if(unsigned(rb.route.back()+3) % 6 == i)
								LOADER.GetMapImageN(67)->Draw(xpos,ypos, 0, 0, 0, 0, 0, 0);
						}
					}
				}
			}
		}
	}

	// Umherfliegende Katapultsteine zeichnen
	for(list<CatapultStone*>::iterator it = catapult_stones.begin();it.valid();++it)
		(*it)->Draw(*this,xoffset,yoffset);


	SoundManager::inst().PlayBirdSounds(noTree::QueryDrawCounter());
}

void GameWorldViewer::DrawBoundaryStone(const int x, const int y, const MapCoord tx, const MapCoord ty, const int xpos, const int ypos, Visibility vis)
{
	if(vis == VIS_INVISIBLE)
		// schwarz/unsichtbar, nichts zeichnen
		return;

	bool fow = !(vis == VIS_VISIBLE);

	unsigned char viewing_player = GameClient::inst().GetPlayerID();
	unsigned char owner = fow ? GetNode(tx,ty).fow[viewing_player].boundary_stones[0] : GetNode(tx,ty).boundary_stones[0];
	if(owner)
	{
		unsigned player_color = COLORS[GetPlayer(owner-1)->color];

		LOADER.GetNationImageN(GetPlayer(owner-1)->nation, 0)->Draw(xpos,ypos,0,0,0,0,0,0,fow ? FOW_DRAW_COLOR : COLOR_WHITE, fow ? CalcPlayerFOWDrawColor(player_color) : player_color);
		LOADER.GetNationImageN(GetPlayer(owner-1)->nation, 1)->Draw(xpos,ypos,0,0,0,0,0,0,COLOR_SHADOW);

		for(unsigned i = 0;i<3;++i)
		{
			if(fow ? GetNode(tx,ty).fow[viewing_player].boundary_stones[i+1] : GetNode(tx,ty).boundary_stones[i+1])
			{
				LOADER.GetNationImageN(GetPlayer(owner-1)->nation, 0)->Draw(xpos-static_cast<int>((tr.GetTerrainX(tx,ty)-tr.GetTerrainXAround(tx,ty,3+i))/2.0f),
					ypos-static_cast<int>((tr.GetTerrainY(tx,ty)-tr.GetTerrainYAround(tx,ty,3+i))/2.0f),0,0,0,0,0,0,
					(vis == VIS_VISIBLE) ? COLOR_WHITE : FOW_DRAW_COLOR, (vis == VIS_VISIBLE) ? player_color : CalcPlayerFOWDrawColor(player_color));
				LOADER.GetNationImageN(GetPlayer(owner-1)->nation, 1)->Draw(xpos-static_cast<int>((tr.GetTerrainX(tx,ty)-tr.GetTerrainXAround(tx,ty,3+i))/2.0f),
					ypos-static_cast<int>((tr.GetTerrainY(tx,ty)-tr.GetTerrainYAround(tx,ty,3+i))/2.0f),0,0,0,0,0,0,COLOR_SHADOW);
			}
		}
	}
}

/// Schaltet Produktivitäten/Namen komplett aus oder an
void GameWorldViewer::ShowNamesAndProductivity()
{
	if(show_productivity && show_names)
		show_productivity = show_names = false;
	else
		show_productivity = show_names = true;
}

unsigned GameWorldViewer::GetAvailableSoldiersForAttack(const unsigned char player_attacker,const MapCoord x, const MapCoord y)
{
	// Ist das angegriffenne ein normales Gebäude?
	nobBaseMilitary * attacked_building = GetSpecObj<nobBaseMilitary>(x,y);
	if(attacked_building->GetBuildingType() >= BLD_BARRACKS && attacked_building->GetBuildingType() <= BLD_FORTRESS)
	{
		// Wird es gerade eingenommen?
		if(static_cast<nobMilitary*>(attacked_building)->IsCaptured())
			// Dann darf es nicht angegriffen werden
			return 0;
	}

	// Militärgebäude in der Nähe finden
	list<nobBaseMilitary*> buildings;
	LookForMilitaryBuildings(buildings,x,y,3);

	unsigned total_count = 0;

	for(list<nobBaseMilitary*>::iterator it = buildings.begin();it.valid();++it)
	{
		// Muss ein Gebäude von uns sein und darf nur ein "normales Militärgebäude" sein (kein HQ etc.)
		if((*it)->GetPlayer() == player_attacker && (*it)->GetBuildingType() >= BLD_BARRACKS && (*it)->GetBuildingType() <= BLD_FORTRESS)
			total_count += static_cast<nobMilitary*>(*it)->GetSoldiersForAttack(x,y,player_attacker);
			
	}

	return total_count;
}



///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void GameWorldViewer::MouseDown(const MouseCoords& mc)
{
	sx = mc.x;
	sy = mc.y;

	scroll = true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void GameWorldViewer::MouseUp()
{
	scroll = false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void GameWorldViewer::MouseMove(const MouseCoords& mc)
{
	// Scrollen
	if(scroll)
	{
		if(SETTINGS.interface.revert_mouse)
			MoveTo( ( sx - mc.x)*2,  ( sy - mc.y)*2);
		else
			MoveTo(-( sx - mc.x)*2, -( sy - mc.y)*2);
		VideoDriverWrapper::inst().SetMousePos(sx, sy);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verschiebt das Bild zu einer bestimmten Stelle.
 *
 *  @author FloSoft
 */
void GameWorldViewer::MoveTo(int x, int y, bool absolute)
{
	if(absolute)
	{
		xoffset = x;
		yoffset = y;
	}
	else
	{
		xoffset += x;
		yoffset += y;
	}

	CalcFxLx();
}

void GameWorldViewer::MoveToMapObject(const MapCoord x, const MapCoord y)
{
	last_xoffset = xoffset;
	last_yoffset = yoffset;

	MoveTo(static_cast<int>(GetTerrainX(x, y))
		- displayWidth  / 2, static_cast<int>(GetTerrainY(x, y))
		- displayHeight / 2, true);
}

/// Springt zur letzten Position, bevor man "weggesprungen" ist
void GameWorldViewer::MoveToLastPosition()
{
	MoveTo(last_xoffset,last_yoffset,true);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void GameWorldViewer::CalcFxLx()
{
	if(xoffset < 0)
		xoffset = width * TR_W + xoffset;
	if(yoffset < 0)
		yoffset = height * TR_H + yoffset;
	if(xoffset > width * TR_W + displayWidth)
		xoffset -= (width * TR_W);
	if(yoffset > height * TR_H + displayHeight)
		yoffset -= (height * TR_H);

	fx = xoffset / TR_W-1;
	fy = (yoffset-0x20*HEIGHT_FACTOR) / TR_H;
	lx = (xoffset+displayWidth)/TR_W+2;
	ly = (yoffset+displayHeight+0x20*HEIGHT_FACTOR)/TR_H;
}

// Höhe wurde Verändert: TerrainRenderer Bescheid sagen, damit es entsprechend verändert werden kann
void GameWorldViewer::AltitudeChanged(const MapCoord x, const MapCoord y)
{
	tr.AltitudeChanged(x,y,this);
}

void GameWorldViewer::VisibilityChanged(const MapCoord x, const MapCoord y)
{
	tr.VisibilityChanged(x,y,this);
}


Visibility GameWorldViewer::GetVisibility(const MapCoord x, const MapCoord y) const
{
	/// Replaymodus und FoW aus? Dann alles sichtbar
	if(GameClient::inst().IsReplayModeOn() && GameClient::inst().IsReplayFOWDisabled())
		return VIS_VISIBLE;

	// Spieler schon tot? Dann auch alles sichtbar?
	if(GameClient::inst().GetLocalPlayer()->isDefeated())
		return VIS_VISIBLE;

	return CalcWithAllyVisiblity(x,y,GameClient::inst().GetPlayerID());
}


void GameWorldViewer::RecalcAllColors()
{
	tr.UpdateAllColors(this);
}

/// liefert sichtbare Straße, im FoW entsprechend die FoW-Straße
unsigned char GameWorldViewer::GetVisibleRoad(const MapCoord x, const MapCoord y, unsigned char dir, const Visibility visibility) const
{
	if(visibility == VIS_VISIBLE)
		// Normal die sichtbaren Straßen zurückliefern
		return GetPointRoad(x,y,dir,true);
	else if(visibility == VIS_FOW)
		// entsprechende FoW-Straße liefern
		return GetPointFOWRoad(x,y,dir,GameClient::inst().GetPlayerID());
	else
		// Unsichtbar -> keine Straße zeichnen
		return 0;
}


/// Gibt das erste Schiff, was gefunden wird von diesem Spieler, zurück, ansonsten NULL, falls es nicht
/// existiert
noShip * GameWorldViewer::GetShip(const MapCoord x, const MapCoord y, const unsigned char player) const
{
	for(unsigned i = 0;i<7;++i)
	{
		Point<MapCoord> pa(x,y);
		if(i > 0)
		{
			pa.x = GetXA(x,y,i-1);
			pa.y = GetYA(x,y,i-1);
		}
		for(list<noBase*>::iterator it = GetFigures(pa.x,pa.y).begin();it.valid();++it)
		{
			if((*it)->GetGOT() == GOT_SHIP)
			{
				noShip * ship = static_cast<noShip*>(*it);
				if(ship->GetPlayer() == player)
				{
					if((ship->GetX() == x && ship->GetY() ==y) ||
					ship->GetDestinationForCurrentMove() == Point<MapCoord>(x,y))
						return static_cast<noShip*>(*it);
				}
			}
		}
		
		
	}
	

	return NULL;
}

/// Gibt die verfügbar Anzahl der Angreifer für einen Seeangriff zurück
unsigned GameWorldViewer::GetAvailableSoldiersForSeaAttackCount(const unsigned char player_attacker, 
											const MapCoord x, const MapCoord y) const
{
	std::list<GameWorldBase::PotentialSeaAttacker> attackers;
	GetAvailableSoldiersForSeaAttack(player_attacker,x,y,&attackers);
	return unsigned(attackers.size());
}

void GameWorldViewer::Resize(unsigned short displayWidth, unsigned short displayHeight)
{
	this->displayWidth  = displayWidth;
	this->displayHeight = displayHeight;
	CalcFxLx();
}
