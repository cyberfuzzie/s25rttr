// $Id: dskAboutRTTR.cpp 5918 2010-01-21 20:59:19Z FloSoft $
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
#include "dskAboutRTTR.h"

#include "WindowManager.h"
#include "Loader.h"

#include "dskMainMenu.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class dskAboutRTTR
 *
 *  Klasse des Über-Siedler-RTTR Desktops.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p dskAboutRTTR.
 *
 *  @author FloSoft
 */
dskAboutRTTR::dskAboutRTTR(void) : Desktop(LOADER.GetImageN("menu", 0))
{
	// "Zurück"
	AddTextButton(0, 300, 550, 200, 22, TC_RED1, _("Back"),NormalFont);

	AddImage(11, 20, 20, LOADER.GetImageN("logo", 0));
}


void dskAboutRTTR::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 0: // "Zurück"
		{
			WindowManager::inst().Switch(new dskMainMenu);
		} break;
	}
}

