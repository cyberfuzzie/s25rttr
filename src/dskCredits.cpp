// $Id: dskCredits.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "dskCredits.h"

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
/** @class dskCredits
 *
 *  Klasse des Credits Desktops.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p dskCredits.
 *
 *  @author FloSoft
 */
dskCredits::dskCredits(void) : Desktop(GetImage(backgrounds, 1))
{

	// "Zurück"
	AddTextButton(0, 300, 560, 200, 22, TC_RED1, _("Back"),NormalFont);

	// "Die Siedler II.5 RTTR"
	AddText(1, 400, 10, _("The Settlers II.5 RTTR"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, LargeFont);

	// "Credits"
	AddText(2, 400, 33, _("Credits"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, LargeFont);
	
	/// @todo Bilder von uns + Siedler laufen unten am Bildschirmrand

	// "Programmiert von:"
	AddText(3, 400, 90, _("Programming"), COLOR_RED, glArchivItem_Font::DF_CENTER, LargeFont);
	// "OLiver"
	AddText(4, 400, 120, "Oliver Siebert (OLiver)", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "FloSoft"
	AddText(5, 400, 140, "Florian Doersch (FloSoft)", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);

	// "Spieletester"
	AddText( 6, 400, 180, _("Game testers"), COLOR_RED, glArchivItem_Font::DF_CENTER, LargeFont);
	// "Demophobie"
	AddText(7, 400, 210, "Patrick Haak (Demophobie)", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "NastX"
	AddText(8, 400, 230, "Jonas Trampe (NastX)", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);


	// "Sonstige Unterstützung"
	AddText( 9, 400, 270, _("Other Support"), COLOR_RED, glArchivItem_Font::DF_CENTER, LargeFont);
	// "muhahahaha"
	AddText(10, 330, 300, "muhahahaha", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Zwoks-Stef"
	AddText(11, 470, 300, "Zwoks-Stef", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Devil"
	AddText(12, 330, 320, "Devil", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Cat666"
	AddText(13, 470, 320, "Cat666", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Sotham"
	AddText(14, 330, 340, "Sotham", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "liwo"
	AddText(15, 470, 340, "liwo", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Airhardt"
	AddText(16, 330, 360, "Airhardt", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
    // "Divan"
    AddText(17, 470, 360, "Divan", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
    // "Fenan"
    AddText(18, 330, 380, "Fenan", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);

	// "Spender:"
	AddText( 19, 400, 420, _("Donators"), COLOR_RED, glArchivItem_Font::DF_CENTER, LargeFont);
	// "diverse anonyme Spenden"
	AddText(20, 400, 450, _("various anonymous donators"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "morlock"
	AddText(21, 400, 470, "morlock", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Jan Montag"
	AddText(22, 400, 490, "Jan Montag", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);

	// "Vielen Dank an alle Spender!"
	AddText(23, 400, 520, _("Thank you for your donations!"), COLOR_RED, glArchivItem_Font::DF_CENTER, NormalFont);
}

void dskCredits::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 0: // "Zurück"
		{
			WindowManager::inst().Switch(new dskMainMenu);
		} break;
	}
}
