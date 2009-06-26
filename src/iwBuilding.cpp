// $Id: iwBuilding.cpp 5117 2009-06-26 14:35:48Z OLiver $
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
#include "iwBuilding.h"

#include "dskGameInterface.h"

#include "Loader.h"
#include "VideoDriverWrapper.h"
#include "GameClient.h"
#include "controls.h"
#include "WindowManager.h"
#include "GameCommands.h"

#include "iwMsgbox.h"

#include "nobUsual.h"
#include "iwDemolishBuilding.h"
#include "iwHelp.h"
#include "BuildingConsts.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwBuilding.
 *
 *  @todo �berpr�fen und die restlichen Steuerelemente zur Funktion bringen
 *
 *  @author OLiver
 */
iwBuilding::iwBuilding(GameWorldViewer * const gwv,dskGameInterface *const gi,nobUsual *const building)
: IngameWindow(building->CreateGUIID(),(unsigned short)-2, (unsigned short)-2, 226, 194, _(BUILDING_NAMES[building->GetBuildingType()]), GetImage(resource_dat, 41)),
	gwv(gwv), gi(gi), building(building)
{
	// Arbeitersymbol
	AddImage(0, 28, 39, GetImage(map_lst, 2298));
	AddImage(1, 28, 39, GetImage(map_lst, 2300 + USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].job));

	// Symbol der produzierten Ware (falls hier was produziert wird)
	if(USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].produced_ware != GD_NOTHING)
	{
		AddImage(2, 196, 39, GetImage(map_lst, 2298));
		AddImage(3, 196, 39, GetImage(map_lst, 2250 + USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].produced_ware));
	}

	// Info
	AddImageButton( 4,  16, 147, 30, 32, TC_GREY, GetImage(io_dat,  21), _("Help"));
	// Abrei�en
	AddImageButton( 5,  50, 147, 34, 32, TC_GREY, GetImage(io_dat,  23), _("Demolish house"));
	// Produktivit�t einstellen (196,197) (bei Sp�hturm ausblenden)
	Window * enable_productivity = AddImageButton( 6,  90, 147, 32, 32, TC_GREY, GetImage(io_dat, ((building->IsProductionDisabledVirtual())?197:196)));
	if(building->GetBuildingType() == BLD_LOOKOUTTOWER)
		enable_productivity->SetVisible(false);
	// "Gehe Zum Ort"
	AddImageButton( 7, 179, 147, 30, 32, TC_GREY, GetImage(io_dat, 107), _("Go to place"));

	// Geb�udebild und dessen Schatten
	AddImage( 8, 117, 114, GetBobImage(building->GetNation(), 250+5*building->GetBuildingType()));

	// Produktivit�tsanzeige (bei Katapulten und Sp�ht�rmen ausblenden)
	Window * productivity = AddPercent(9, 59, 31, 106, 16, TC_GREY, 0xFFFFFF00, GetFont(resource_dat, 2), building->GetProduktivityPointer());
	if(building->GetBuildingType() == BLD_CATAPULT || building->GetBuildingType() == BLD_LOOKOUTTOWER)
		productivity->SetVisible(false);


	ctrlText *text = AddText(10, 113, 50, _("(House unoccupied)"), COLOR_RED, glArchivItem_Font::DF_CENTER, NormalFont); 

	text->SetVisible(!building->HasWorker());
}


void iwBuilding::Msg_PaintBefore()
{
	// Schatten des Geb�udes (muss hier gezeichnet werden wegen schwarz und halbdurchsichtig)
	glArchivItem_Bitmap *bitmap = GetBobImage(building->GetNation(), 250+5*building->GetBuildingType()+1);

	if(bitmap)
		bitmap->Draw(GetX()+117, GetY()+114, 0, 0, 0, 0, 0, 0, COLOR_SHADOW);
}

void iwBuilding::Msg_PaintAfter()
{
	if(building->GetBuildingType() >= BLD_GRANITEMINE && building->GetBuildingType() <= BLD_GOLDMINE)
	{
		// Bei Bergwerken sieht die Nahrungsanzeige ein wenig anders aus (3x 2)

		// "Schwarzer Rahmen"
		DrawRectangle(GetX()+40, GetY()+60, 144, 24, 0x80000000);

		for(unsigned char i = 0; i < 3; ++i)
		{
			for(unsigned char z = 0; z < 2; ++z)
			{
				glArchivItem_Bitmap *bitmap = GetImage(map_lst, 2250+USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].wares_needed[i]);
				bitmap->Draw(GetX()+52 + 24 * (i*2 + z), GetY()+72, 0, 0, 0, 0, 0, 0, (z < building->GetWares(i) ? 0xFFFFFFFF : 0xFFA0A0A0) );
			}

		}
	}
	else
	{
		for(unsigned char i = 0; i < 2; ++i)
		{
			if(USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].wares_needed[i] == GD_NOTHING)
				break;


			// 6x Waren, je nachdem ob sie da sind, bei Katapult 4!
			unsigned wares_count = (building->GetBuildingType() == BLD_CATAPULT)?4:6;



			// "Schwarzer Rahmen"
			DrawRectangle(GetX() + width/2 - 24 * wares_count / 2, GetY()+60 + i*29, 24*wares_count, 24, 0x80000000);

			
			for(unsigned char z = 0; z < wares_count; ++z)
			{
				glArchivItem_Bitmap *bitmap = GetImage(map_lst, 2250+USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].wares_needed[i]);
				bitmap->Draw(GetX() + width/2 - 24 * wares_count / 2 + 24*z+12, GetY()+72 + i*28, 0, 0, 0, 0, 0, 0, (z < building->GetWares(i) ? 0xFFFFFFFF : 0xFFA0A0A0) );
			}
		}
	}
}


void iwBuilding::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 4: // Hilfe
		{
			WindowManager::inst().Show(new iwHelp(GUI_ID(CGI_HELPBUILDING+building->GetBuildingType()),_(BUILDING_NAMES[building->GetBuildingType()]),
				_(BUILDING_HELP_STRINGS[building->GetBuildingType()])));
		} break;
	case 5: // Geb�ude abbrennen
		{
			// Abrei�en?
			Close();
			WindowManager::inst().Show(new iwDemolishBuilding(gwv,GOT_NOB_USUAL,building->GetX(), building->GetY(),building->GetBuildingType(),building->GetNation(),building->CreateGUIID()));
		} break;
	case 6:
		{
			// Produktion einstellen/fortf�hren
			// NC senden
			if(GAMECLIENT.AddGC(new gc::StopProduction(building->GetX(), building->GetY())))
			{
				// visuell anzeigen, falls erfolgreich
				building->StopProductionVirtual();
				
				// anderes Bild auf dem Button
				if(building->IsProductionDisabledVirtual())
					GetCtrl<ctrlImageButton>(6)->SetImage(GetImage(io_dat, 197));
				else
					GetCtrl<ctrlImageButton>(6)->SetImage(GetImage(io_dat, 196));

				ctrlText *text = GetCtrl<ctrlText>(10);
				if(building->IsProductionDisabledVirtual() && building->HasWorker())
					text->SetText(_("(House unoccupied)"));
				else if(building->HasWorker())
					text->SetVisible(false);
			}

		} break;
	case 7: // "Gehe Zum Ort"
		{
			gwv->MoveToMapObject(building->GetX(), building->GetY());
		} break;
	}
}
