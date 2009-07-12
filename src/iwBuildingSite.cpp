// $Id: iwBuildingSite.cpp 5254 2009-07-12 15:49:16Z FloSoft $
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
#include "iwBuildingSite.h"

#include "VideoDriverWrapper.h"

#include "Loader.h"
#include "GameClient.h"
#include "WindowManager.h"
#include "noBuildingSite.h"

#include "iwDemolishBuilding.h"
#include "iwMsgbox.h"
#include "iwHelp.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwBuildingSite.
 *
 *  @author OLiver
 */
iwBuildingSite::iwBuildingSite(GameWorldViewer * const gwv,const noBuildingSite * const buildingsite) 
: IngameWindow(buildingsite->CreateGUIID(), 0xFFFE, 0xFFFE, 226, 194, _(BUILDING_NAMES[buildingsite->GetBuildingType()]), LOADER.GetImageN("resource",41)),
	gwv(gwv),buildingsite(buildingsite)
{
	// Bild des Gebäudes
	AddImage(0, 113, 130, LOADER.GetNationImageN(GAMECLIENT.GetLocalPlayer()->nation, 250+5*buildingsite->GetBuildingType()));
	// Gebäudename 
	AddText(1, 113, 44, _("Order of building site"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);

	// Hilfe
	AddImageButton( 2,  16, 147, 30, 32, TC_GREY, LOADER.GetImageN("io",  21));
	// Gebäude abbrennen
	AddImageButton( 3,  50, 147, 34, 32, TC_GREY, LOADER.GetImageN("io",  23));

	// "Gehe Zu Ort"
	AddImageButton( 4, 179, 147, 30, 32, TC_GREY, LOADER.GetImageN("io", 107), _("Go to place"));
}

void iwBuildingSite::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 2: // Hilfe
		{
			WindowManager::inst().Show(new iwHelp(GUI_ID(CGI_HELPBUILDING+buildingsite->GetBuildingType()),_(BUILDING_NAMES[buildingsite->GetBuildingType()]),
				_(BUILDING_HELP_STRINGS[buildingsite->GetBuildingType()])));
		} break;
	case 3: // Gebäude abbrennen
		{
			// Abreißen?
			Close();
			WindowManager::inst().Show(new iwDemolishBuilding(gwv,GOT_NOB_USUAL,buildingsite->GetX(), buildingsite->GetY(),buildingsite->GetBuildingType(),buildingsite->GetNation(),buildingsite->CreateGUIID()));
		} break;

	case 4: // "Gehe Zu Ort"
		{
			gwv->MoveToMapObject(buildingsite->GetX(), buildingsite->GetY());	
		} break;
	}
}
