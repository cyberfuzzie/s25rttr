// $Id: iwBuildings.cpp 5253 2009-07-12 14:42:18Z FloSoft $
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
#include "iwBuildings.h"
#include "Loader.h"
#include "GameConsts.h"
#include "GameClient.h"

/// Reihenfolge der Gebäude
const BuildingType bts[31] =
{
	BLD_BARRACKS,
	BLD_GUARDHOUSE,
	BLD_WATCHTOWER,
	BLD_FORTRESS,
	BLD_GRANITEMINE,
	BLD_COALMINE,
	BLD_IRONMINE,
	BLD_GOLDMINE,
	BLD_LOOKOUTTOWER,
	BLD_CATAPULT,
	BLD_WOODCUTTER,
	BLD_FISHERY,
	BLD_QUARRY,
	BLD_FORESTER,
	BLD_SLAUGHTERHOUSE,
	BLD_HUNTER,
	BLD_BREWERY,
	BLD_ARMORY,
	BLD_METALWORKS,
	BLD_IRONSMELTER,
	BLD_PIGFARM,
	BLD_STOREHOUSE,
	BLD_MILL,
	BLD_BAKERY,
	BLD_SAWMILL,
	BLD_MINT,
	BLD_WELL,
	BLD_SHIPYARD,
	BLD_FARM,
	BLD_DONKEYBREEDER,
	BLD_HARBORBUILDING
};


// Abstand des ersten Icons vom linken oberen Fensterrand
const unsigned short first_x = 30;
const unsigned short first_y = 40;
// Abstand der einzelnen Symbole untereinander
const unsigned short icon_distance_x = 40;
const unsigned short icon_distance_y = 48;
// Abstand der Schriften unter den Icons
const unsigned short font_distance_y = 20;


/// Konstruktor von @p iwMilitary.
iwBuildings::iwBuildings() : IngameWindow(CGI_BUILDINGS, 0xFFFE, 0xFFFE, 185, 430, _("Buildings"), LOADER.GetImageN("resource", 41))
{
	// Symbole für die einzelnen Gebäude erstellen
	for(unsigned short y = 0;y<8;++y)
	{
		for(unsigned short x = 0;x<((y==7)?3:4);++x)
		{
			AddImage(y*4+x,first_x+icon_distance_x*x,first_y+icon_distance_y*y,
				LOADER.GetImageN(NATION_ICON_IDS[GameClient::inst().GetLocalPlayer()->nation],bts[y*4+x]),_(BUILDING_NAMES[bts[y*4+x]]));
		}
	}

	// Hilfe-Button
	AddImageButton(32,width-14-30,height-20-32,30,32,TC_GREY, LOADER.GetImageN("io",21), _("Help"));

}

/// Anzahlen der Gebäude zeichnen
void iwBuildings::Msg_PaintAfter()
{
	// Anzahlen herausfinden
	BuildingCount bc;

	GameClient::inst().GetLocalPlayer()->GetBuildingCount(bc);

	// Anzahlen unter die Gebäude schreiben
	for(unsigned short y = 0;y<8;++y)
	{
		for(unsigned short x = 0;x<((y==7)?3:4);++x)
		{
			char txt[64];
			sprintf(txt,"%u/%u",bc.building_counts[bts[y*4+x]],bc.building_site_counts[bts[y*4+x]]);
			NormalFont->Draw(GetX()+first_x+icon_distance_x*x,GetY()+first_y+icon_distance_y*y + font_distance_y, txt,
				glArchivItem_Font::DF_CENTER,COLOR_YELLOW);

		}
	}
}
