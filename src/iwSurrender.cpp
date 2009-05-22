// $Id: iwEndgame.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "iwSurrender.h"

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
 *  Konstruktor von @p iwSurrender.
 *
 *  @author jh
 */
iwSurrender::iwSurrender(void)
	: IngameWindow(CGI_ENDGAME, 0xFFFF, 0xFFFF, 240, 100, _("Surrender game?"), GetImage(resource_dat, 41))
{
	// Ok
	AddImageButton(0,  85, 24, 68, 57, TC_GREEN2, GetImage(io_dat, 32), _("Surrender"));
	// Ok + Abbrennen
	AddImageButton(2,  16, 24, 68, 57, TC_GREEN2, GetImage(io_dat, 23), _("Destroy all buildings and surrender"));	
	// Abbrechen
	AddImageButton(1,  158, 24, 68, 57, TC_RED1, GetImage(io_dat, 40), _("Don't surrender"));
}


void iwSurrender::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 0: // OK
		{
      GameClient::inst().NC_Surrender();
      Close();
		} break;
	case 1: // Abbrechen
		{
			Close();
		} break;
	case 2: // OK + Alles abbrennen
		{
      GameClient::inst().NC_DestroyAll();
      Close();
		} break;
	}
}
