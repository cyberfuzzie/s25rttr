// $Id: iwRoadWindow.cpp 5853 2010-01-04 16:14:16Z FloSoft $
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
#include "iwRoadWindow.h"

#include "Loader.h"
#include "VideoDriverWrapper.h"
#include "Settings.h"
#include "controls.h"

#include "dskGameInterface.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwRoadWindow.
 *
 *  @author OLiver
 */
iwRoadWindow::iwRoadWindow(dskGameInterface *const GameInterface, bool flagpossible, int mouse_x, int mouse_y)
	: IngameWindow(CGI_ROADWINDOW, mouse_x, mouse_y, 200, 100, _("Activity window"), LOADER.GetImageN("io", 1)),
	GameInterface(GameInterface), last_x(mouse_x), last_y(mouse_y)
{
	// Bau abbrechen
	ctrlButton *cancel = AddImageButton(1, 10, 20, 36, 36, TC_GREY, LOADER.GetImageN("io", 110), _("Interrupt road building"));

	if(flagpossible)
	{
		// Flagge platzieren
		AddImageButton(0, 10, 20, 36, 36, TC_GREY, LOADER.GetImageN("io", 70), _("Erect flag"));
		// Abbrechenbutton daneben schieben
		cancel->Move(46, 20);
	}

	if(x + GetWidth() > SETTINGS.video.width)
		x = mouse_x - GetWidth() - 40;
	if(y + GetHeight() > SETTINGS.video.height)
		y = mouse_y - GetHeight() - 40;

	if(x+GetWidth() > SETTINGS.video.width)
		x = mouse_x-GetWidth()-40;
	if(y+GetHeight() > SETTINGS.video.height)
		y = mouse_y-GetHeight()-40;

	VideoDriverWrapper::inst().SetMousePos(GetX()+20,GetY()+45);
}

iwRoadWindow::~iwRoadWindow()
{
	GameInterface->RoadWindowClosed();
}

void iwRoadWindow::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 0: // Flagge & Weg bauen
		{
			GameInterface->CommandBuildRoad();
		} break;
	case 1: // Bau abbrechen
		{
			GameInterface->ActivateRoadMode(RM_DISABLED);
		} break;
	}

	// Maus an vorherige Stelle setzen
	VideoDriverWrapper::inst().SetMousePos(last_x, last_y);

	// und fenster schlieﬂen
	Close();
}
