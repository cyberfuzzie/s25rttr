// $Id: iwBuilding.cpp 4933 2009-05-24 12:29:23Z OLiver $
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
#include "iwDiplomacy.h"

#include "dskGameInterface.h"

#include "Loader.h"
#include "VideoDriverWrapper.h"
#include "GameClient.h"
#include "controls.h"
#include "WindowManager.h"
#include "GameCommands.h"
#include "GameClientPlayer.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/// Position des Headers der Tabelle (Y)
const unsigned short HEADER_Y = 30;
/// Position der ersten Zeile (Y)
const unsigned short FIRST_LINE_Y = 55;
/// Höhe der einzelnen farbigen Zeilen
const unsigned short CELL_HEIGHT = 50;
/// Abstand zwischen den farbigen Zeilen
const unsigned short SPACE_HEIGHT = 20;
/// Abstand vom Rand der Zeilen
const unsigned short LINE_DISTANCE_TO_MARGINS = 20;
/// Größe der Pingfelder
const unsigned short PING_FIELD_WIDTH = 40;
/// Position der Pingfelder vom linken Rand aus (relativ zur Mitte)
const unsigned short PING_FIELD_POS = 150;
/// Position der Bündnisse vom linken Rand aus (relativ zur Mitte)
const unsigned short TREATIES_POS = 240;
/// Abstand zwischen den beiden Bündnis-Buttons (Achtung: von Mittelpunkten aus!)
const unsigned short TREATIE_BUTTON_SPACE = 20;


///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwDiplomacy.
 *
 *
 *  @author OLiver
 */
iwDiplomacy::iwDiplomacy()
: IngameWindow(CGI_DIPLOMACY,(unsigned short)-1, (unsigned short)-1, 500, FIRST_LINE_Y+GameClient::inst().GetPlayerCount()*(CELL_HEIGHT+SPACE_HEIGHT)+20, _("Diplomacy"), 
			   GetImage(resource_dat, 41))
{
	// "Header" der Tabelle
	AddText(0,LINE_DISTANCE_TO_MARGINS+PING_FIELD_POS,HEADER_Y,_("Ping"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	AddText(1,LINE_DISTANCE_TO_MARGINS+TREATIES_POS,HEADER_Y,_("Treaties"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);



	for(unsigned i = 0;i<GameClient::inst().GetPlayerCount();++i)
	{
		// Einzelne Spielernamen
		AddText(100+i,LINE_DISTANCE_TO_MARGINS+10,FIRST_LINE_Y + i*(CELL_HEIGHT+SPACE_HEIGHT) + CELL_HEIGHT/2,
			GameClient::inst().GetPlayer(i)->name,COLORS[GameClient::inst().GetPlayer(i)->color],glArchivItem_Font::DF_VCENTER,
			NormalFont);

		// Ping
		AddDeepening(200+i,LINE_DISTANCE_TO_MARGINS+PING_FIELD_POS-PING_FIELD_WIDTH/2,
			FIRST_LINE_Y + i*(CELL_HEIGHT+SPACE_HEIGHT) + CELL_HEIGHT/2 - 11,PING_FIELD_WIDTH,22,TC_GREY,"0",NormalFont,COLOR_YELLOW);

		// Bündnisvertrag-Button
		glArchivItem_Bitmap * image = GetImage(io_dat,61);
		AddImageButton(300+i,LINE_DISTANCE_TO_MARGINS+TREATIES_POS-TREATIE_BUTTON_SPACE/2-(image->getWidth()+8),
			FIRST_LINE_Y + i*(CELL_HEIGHT+SPACE_HEIGHT) + CELL_HEIGHT/2 - (image->getHeight()+8)/2,image->getWidth()+8,
			image->getHeight()+8,TC_GREY,image,_("Treaty of alliance"));

		// Nichtangriffspakt
		image = GetImage(io_dat,100);
		AddImageButton(400+i,LINE_DISTANCE_TO_MARGINS+TREATIES_POS+TREATIE_BUTTON_SPACE/2,
			FIRST_LINE_Y + i*(CELL_HEIGHT+SPACE_HEIGHT) + CELL_HEIGHT/2 - (image->getHeight()+8)/2,image->getWidth()+8,
			image->getHeight()+8,TC_GREY,image,_("Non-aggression pact"));
	}


}

void iwDiplomacy::Msg_PaintBefore()
{
	// Die farbigen Zeilen malen
	for(unsigned i = 0;i<GameClient::inst().GetPlayerCount();++i)
	{
		// Rechtecke in Spielerfarbe malen mit entsprechender Transparenz
		Window::DrawRectangle(GetX()+LINE_DISTANCE_TO_MARGINS,GetY()+FIRST_LINE_Y+i*(CELL_HEIGHT+SPACE_HEIGHT),width-2*LINE_DISTANCE_TO_MARGINS,CELL_HEIGHT,
			(COLORS[GameClient::inst().GetPlayer(i)->color]&0x00FFFFFF)|0x40000000);
	}
}

void iwDiplomacy::Msg_PaintAfter()
{
}

void iwDiplomacy::Msg_ButtonClick(const unsigned int ctrl_id)
{
}
