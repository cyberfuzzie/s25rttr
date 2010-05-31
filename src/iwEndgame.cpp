// $Id: iwEndgame.cpp 6458 2010-05-31 11:38:51Z FloSoft $
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
#include "iwEndgame.h"

#include "Loader.h"
#include "GameManager.h"

#include "dskMainMenu.h"
#include "iwSave.h"
#include "WindowManager.h"
#include "GameClient.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwEndgame.
 *
 *  @author OLiver
 */
iwEndgame::iwEndgame(void)
	: IngameWindow(CGI_ENDGAME, 0xFFFF, 0xFFFF, 240, 100, _("End game?"), LOADER.GetImageN("resource", 41))
{
	// Ok
	AddImageButton(0,  16, 24, 71, 57, TC_GREEN2, LOADER.GetImageN("io", 32));
	// Abbrechen
	AddImageButton(1,  88, 24, 71, 57, TC_RED1, LOADER.GetImageN("io", 40));
	// Ok + Speichern
	AddImageButton(2, 160, 24, 65, 57, TC_GREY, LOADER.GetImageN("io", 47));
}


void iwEndgame::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 0: // OK
		{
			GAMEMANAGER.ShowMenu();
			GameClient::inst().ExitGame();
		} break;
	case 1: // Abbrechen
		{
			Close();
		} break;
	case 2: // OK + Speichern
		{
			WindowManager::inst().Show(new iwSave());
		} break;
	}
}
