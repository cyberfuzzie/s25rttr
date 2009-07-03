// $Id: iwShip.cpp 5125 2009-06-26 20:10:42Z OLiver $
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
#include "iwShip.h"

#include "dskGameInterface.h"

#include "Loader.h"
#include "VideoDriverWrapper.h"
#include "GameClient.h"
#include "controls.h"
#include "WindowManager.h"
#include "GameCommands.h"
#include "noShip.h"

#include "iwMsgbox.h"



///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/// IDs in der IO_DAT von Boot und Schiffs-Bild für den Umschaltebutton beim Schiffsbauer
const unsigned IODAT_BOAT_ID = 219;
const unsigned IODAT_SHIP_ID = 218;

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwShip.
 *
 *  @todo überprüfen und die restlichen Steuerelemente zur Funktion bringen
 *
 *  @author OLiver
 */
iwShip::iwShip(GameWorldViewer * const gwv,dskGameInterface *const gi,noShip *const ship)
: IngameWindow(CGI_SHIP,(unsigned short)-2, (unsigned short)-2, 252, 238, _("Ship register"), GetImage(resource_dat, 41)),
gwv(gwv), gi(gi), ship_id(GameClient::inst().GetPlayer(ship->GetPlayer())->GetShipID(ship)), player(ship->GetPlayer())
{
	AddImage(  0,126,101, GetImage(io_dat, 228));
	AddImageButton( 2, 18,192, 30, 35,TC_GREY,GetImage(io_dat, 225));		// Viewer: 226 - Hilfe
	AddImageButton( 3, 51,196, 30, 26,TC_GREY,GetImage(io_dat, 102));		// Viewer: 103 - Schnell zurück
	AddImageButton( 4, 81,196, 30, 26,TC_GREY,GetImage(io_dat, 103));		// Viewer: 104 - Zurück
	AddImageButton( 5,111,196, 30, 26,TC_GREY,GetImage(io_dat, 104));		// Viewer: 105 - Vor
	AddImageButton( 6,141,196, 30, 26,TC_GREY,GetImage(io_dat, 105));		// Viewer: 106 - Schnell vor

	// Die Expeditionsweiterfahrbuttons
	AddImageButton(10,60,81,18,18,TC_GREY,GetImage(io_dat,187),_("Found colony"));
	
	const int BUTTON_POS[6][2] =
	{
		60,61, 80,70, 80,90, 60,101, 40,90, 40,70
	};

	for(unsigned i = 0;i<6;++i)
		AddImageButton(11+i,BUTTON_POS[i][0],BUTTON_POS[i][1],18,18,TC_GREY,GetImage(io_dat,181+(i+4)%6));
}


void iwShip::Msg_PaintBefore()
{

}

void iwShip::Msg_PaintAfter()
{
	// Schiff holen
	noShip * ship = GameClient::inst().GetPlayer(player)->GetShipByID(ship_id);

	// Kein Schiff gefunden? Dann erstes Schiff holen
	if(!ship)
	{
		ship_id = 0;
		// Nochmal probieren
		ship = GameClient::inst().GetPlayer(ship->GetPlayer())->GetShipByID(ship_id);
		// Immer noch nicht? Dann gibt es keine Schiffe mehr und wir zeigen eine entsprechende Meldung an
		if(!ship)
		{
			GetFont(resource_dat, 0)->Draw(GetX()+width/2,GetY()+60,_("No ships available"),glArchivItem_Font::DF_CENTER,COLOR_WINDOWBROWN);
			return;
		}
	}


	// Schiffsname
	GetFont(resource_dat, 0)->Draw(GetX()+42,GetY()+42,ship->GetName(),0,COLOR_WINDOWBROWN);
	// Schiffs-Nr.
	char str[32];
	sprintf(str,"%u/%u",ship_id+1,GameClient::inst().GetPlayer(ship->GetPlayer())->GetShipCount());
	GetFont(resource_dat, 0)->Draw(GetX()+208,GetY()+42,str,glArchivItem_Font::DF_RIGHT,COLOR_WINDOWBROWN);
	// Das Schiffs-Bild
	GetImage(boot_lst,12)->Draw(GetX()+138,GetY()+117);

	// Expeditions-Buttons malen?
	if(ship->IsWaitingForExpeditionInstructions())
	{
		for(unsigned char i = 0;i<6;++i)
			GetCtrl<Window>(11+i)->SetVisible(gwv->GetNextFreeHarborPoint(ship->GetCurrentHarbor(),
			i,ship->GetPlayer(),ship->GetSeaID()) > 0);
	}
	else
	{
		// Alle Buttons inklusive Anker in der Mitte ausblenden
		for(unsigned i = 0;i<7;++i)
			GetCtrl<Window>(10+i)->SetVisible(false);
	}
}


void iwShip::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 4: // Hilfe
		{
		//	WindowManager::inst().Show(new iwHelp(GUI_ID(CGI_HELPBUILDING+ship->GetShipType()),_(BUILDING_NAMES[ship->GetShipType()]),
		//		_(BUILDING_HELP_STRINGS[ship->GetShipType()])));
		} break;
	}
}
