// $Id: iwBuilding.cpp 8030 2012-07-08 20:54:57Z jh $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#include "iwBuilding.h"

#include "dskGameInterface.h"

#include "Loader.h"
#include "VideoDriverWrapper.h"
#include "GameClient.h"
#include "controls.h"
#include "WindowManager.h"
#include "GameCommands.h"

#include "iwMsgbox.h"

#include "nobShipYard.h"
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

/// IDs in der IO_DAT von Boot und Schiffs-Bild f�r den Umschaltebutton beim Schiffsbauer
const unsigned IODAT_BOAT_ID = 219;
const unsigned IODAT_SHIP_ID = 218;

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwBuilding.
 *
 *  @todo �berpr�fen und die restlichen Steuerelemente zur Funktion bringen
 *
 *  @author OLiver
 */
iwBuilding::iwBuilding(GameWorldViewer * const gwv,dskGameInterface *const gi,nobUsual *const building)
: IngameWindow(building->CreateGUIID(),(unsigned short)-2, (unsigned short)-2, 226, 194, _(BUILDING_NAMES[building->GetBuildingType()]), LOADER.GetImageN("resource", 41)),
	gwv(gwv), gi(gi), building(building)
{
	// Arbeitersymbol
	AddImage(0, 28, 39, LOADER.GetMapImageN(2298));

        // Exception: charburner
	// TODO: ugly!
        if(building->GetBuildingType() != BLD_CHARBURNER)
                AddImage(1, 28, 39, LOADER.GetMapImageN(2300 + USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].job));
        else
	        AddImage(1, 28, 39, LOADER.GetImageN("charburner",51));

	// Geb�udesymbol
	AddImage(1, 117, 114, building->GetBuildingImage());

	// Symbol der produzierten Ware (falls hier was produziert wird)
	if(USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].produced_ware != GD_NOTHING)
	{
		AddImage(2, 196, 39, LOADER.GetMapImageN(2298));
		AddImage(3, 196, 39, LOADER.GetMapImageN(2250 + USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].produced_ware));
	}

	// Info
	AddImageButton( 4,  16, 147, 30, 32, TC_GREY, LOADER.GetImageN("io",  21), _("Help"));
	// Abrei�en
	AddImageButton( 5,  50, 147, 34, 32, TC_GREY, LOADER.GetImageN("io",  23), _("Demolish house"));
	// Produktivit�t einstellen (196,197) (bei Sp�hturm ausblenden)
	Window * enable_productivity = AddImageButton( 6,  90, 147, 34, 32, TC_GREY, LOADER.GetImageN("io", ((building->IsProductionDisabledVirtual())?197:196)));
	if(building->GetBuildingType() == BLD_LOOKOUTTOWER)
		enable_productivity->SetVisible(false);
	// Bei Bootsbauer Button zum Umw�hlen von Booten und Schiffen
	if(building->GetBuildingType() == BLD_SHIPYARD)
	{
		// Jenachdem Boot oder Schiff anzeigen
		unsigned io_dat_id = (static_cast<nobShipYard*>(building)->GetMode() == nobShipYard::BOATS) 
			? IODAT_BOAT_ID : IODAT_SHIP_ID;
		AddImageButton(11,130,147,43,32,TC_GREY,LOADER.GetImageN("io",io_dat_id));
	}

	// "Gehe Zum Ort"
	AddImageButton( 7, 179, 147, 30, 32, TC_GREY, LOADER.GetImageN("io", 107), _("Go to place"));

	// Geb�udebild und dessen Schatten
	AddImage( 8, 117, 114, LOADER.GetNationImageN(building->GetNation(), 250+5*building->GetBuildingType()));

	// Produktivit�tsanzeige (bei Katapulten und Sp�ht�rmen ausblenden)
	Window * productivity = AddPercent(9, 59, 31, 106, 16, TC_GREY, 0xFFFFFF00, SmallFont, building->GetProduktivityPointer());
	if(building->GetBuildingType() == BLD_CATAPULT || building->GetBuildingType() == BLD_LOOKOUTTOWER)
		productivity->SetVisible(false);

	AddText(10, 113, 50, _("(House unoccupied)"), COLOR_RED, glArchivItem_Font::DF_CENTER, NormalFont); 
}


void iwBuilding::Msg_PaintBefore()
{
	// Schatten des Geb�udes (muss hier gezeichnet werden wegen schwarz und halbdurchsichtig)
	glArchivItem_Bitmap *bitmap = building->GetBuildingImageShadow();

	if(bitmap)
		bitmap->Draw(GetX()+117, GetY()+114, 0, 0, 0, 0, 0, 0, COLOR_SHADOW);

	// Haus unbesetzt ggf ausblenden
	GetCtrl<ctrlText>(10)->SetVisible(!building->HasWorker());
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
				glArchivItem_Bitmap *bitmap = LOADER.GetMapImageN(2250+USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].wares_needed[i]);
				bitmap->Draw(GetX()+52 + 24 * (i*2 + z), GetY()+72, 0, 0, 0, 0, 0, 0, (z < building->GetWares(i) ? 0xFFFFFFFF : 0xFF404040) );
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
				glArchivItem_Bitmap *bitmap = LOADER.GetMapImageN(2250+USUAL_BUILDING_CONSTS[building->GetBuildingType()-10].wares_needed[i]);
				bitmap->Draw(GetX() + width/2 - 24 * wares_count / 2 + 24*z+12, GetY()+72 + i*28, 0, 0, 0, 0, 0, 0, (z < building->GetWares(i) ? 0xFFFFFFFF : 0xFF404040) );
				
			}

			std::stringstream text;
			text << (unsigned int)building->GetWares(i) << "/" << wares_count;
			NormalFont->Draw(GetX() + width/2, GetY()+60+12 + i*29, text.str(), glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_VCENTER);
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
			WindowManager::inst().Show(new iwDemolishBuilding(gwv,building));
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
					GetCtrl<ctrlImageButton>(6)->SetImage(LOADER.GetImageN("io", 197));
				else
					GetCtrl<ctrlImageButton>(6)->SetImage(LOADER.GetImageN("io", 196));

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
	case 11: // Schiff/Boot umstellen bei Schiffsbauer
		{
			if(GameClient::inst().AddGC(new gc::ChangeShipYardMode(building->GetX(), building->GetY())))
			{
				// Auch optisch den Button umstellen
				ctrlImageButton * button = GetCtrl<ctrlImageButton>(11);
				if(button->GetButtonImage() == LOADER.GetImageN("io",IODAT_BOAT_ID))
					button->SetImage(LOADER.GetImageN("io",IODAT_SHIP_ID));
				else
					button->SetImage(LOADER.GetImageN("io",IODAT_BOAT_ID));
			}

		} break;
	}
}
