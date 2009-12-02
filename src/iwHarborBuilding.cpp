// $Id: iwHarborBuilding.cpp 5041 2009-06-13 11:52:29Z OLiver $
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
#include "iwHarborBuilding.h"

#include "Loader.h"
#include "nobHarborBuilding.h"
#include "ctrlGroup.h"
#include "GameClient.h"
#include "GameCommands.h"
#include "ctrlButton.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwHarborBuilding.
 *
 *  @author OLiver
 */
iwHarborBuilding::iwHarborBuilding(GameWorldViewer * const gwv,nobHarborBuilding *hb)
	: iwBaseWarehouse(gwv,_("Harbor building"), 3, hb)
{
	// Zusätzliche Hafenseite
	ctrlGroup * harbor_page = AddGroup(102);

	// "Expedition"-Überschrift
	harbor_page->AddText(0,83,70,_("Expedition"),0xFFFFFF00,glArchivItem_Font::DF_CENTER,NormalFont);

	// Button zum Expedition starten
	harbor_page->AddImageButton(1,65,100,30,30,TC_GREY,LOADER.GetImageN("io",176),_("Start expedition"));
	AdjustExpeditionButton(false);

	harbor_page->SetVisible(false);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Expeditionsknopf korrekt
 *
 *  falls @p flip gesetzt, dann umgekehrt einfärben
 *
 *  @author FloSoft
 */
void iwHarborBuilding::AdjustExpeditionButton(bool flip)
{
	ctrlImageButton *button = GetCtrl<ctrlGroup>(102)->GetCtrl<ctrlImageButton>(1);

	// Visuelle Rückmeldung, grün einfärben, wenn Expedition gestartet wurde
	// Jeweils umgekehrte Farbe nehmen, da die Änderung ja spielerisch noch nicht 
	// in Kraft getreten ist!
	bool exp = static_cast<nobHarborBuilding*>(wh)->IsExpeditionActive();

	// "flip xor exp", damit korrekt geswitcht wird, falls expedition abgebrochen werden soll
	// und dies direkt dargestellt werden soll (flip)
	if( (flip || exp) && !(flip && exp))
	{
		button->SetModulationColor(COLOR_WHITE);
		button->SetTooltip("Cancel expedition");
	}
	else
	{
		button->SetModulationColor(COLOR_RED);
		button->SetTooltip("Start expedition");
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void iwHarborBuilding::Msg_Group_ButtonClick(const unsigned int group_id, const unsigned int ctrl_id)
{
	switch(group_id)
	{
	default:
		break;

	case 102: // Hafengruppe?
		{
			switch(ctrl_id)
			{
			default:
				break;

			case 1: // Expedition starten
				{
					// Entsprechenden GC senden
					if(GameClient::inst().AddGC(new gc::StartExpedition(wh->GetX(),wh->GetY())))
						AdjustExpeditionButton(true);
				} break;
			}
		} break;
	}

	 // an Basis weiterleiten
	iwBaseWarehouse::Msg_Group_ButtonClick(group_id,ctrl_id);
}

