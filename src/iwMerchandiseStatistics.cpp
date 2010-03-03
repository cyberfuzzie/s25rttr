// $Id: iwMerchandiseStatistics.cpp 5972 2010-02-08 18:47:05Z FloSoft $
//
// Copyright (c) 2005-2010 Settlers Freaks (sf-team at siedler25.org)
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
#include "iwMerchandiseStatistics.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwMerchandiseStatistics.
 *
 *  @author jh
 */
iwMerchandiseStatistics::iwMerchandiseStatistics()
: IngameWindow(CGI_MERCHANDISE_STATISTICS, 0xFFFE, 0xFFFE, 252, 310, _("Merchandise"), LOADER.GetImageN("resource", 41))
{
	// Statistikfeld
	AddImage(0, 10+115, 23+81, LOADER.GetImageN("io", 228));

	// Waren-Buttons
	// obere Reihe
	AddImageButton(1, 17, 192, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_WOOD), _("Wood"));
	AddImageButton(2, 48, 192, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_BOARDS), _("Boards"));
	AddImageButton(3, 79, 192, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_STONES), _("Stones"));
	AddImageButton(4, 110, 192, 30, 30, TC_GREY, LOADER.GetImageN("io",80), _("Food"));
	AddImageButton(5, 141, 192, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_WATER), _("Water"));
	AddImageButton(6, 172, 192, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_BEER), _("Beer"));
	AddImageButton(7, 203, 192, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_COAL), _("Coal"));

	// untere Reihe
	AddImageButton(8, 17, 227, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_IRONORE), _("Ironore"));
	AddImageButton(9, 48, 227, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_GOLD), _("Gold"));
	AddImageButton(10, 79, 227, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_IRON), _("Iron"));
	AddImageButton(12, 110, 227, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_COINS), _("Coins"));
	AddImageButton(13, 141, 227, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_HAMMER), _("Tools"));
	AddImageButton(14, 172, 227, 30, 30, TC_GREY, LOADER.GetImageN("io",111), _("Weapons"));
	AddImageButton(15, 203, 227, 30, 30, TC_GREY, LOADER.GetMapImageN(2250+GD_BOAT), _("Boats"));

	// Hilfe
	AddImageButton(16, 17, 261, 30, 32, TC_GREY, LOADER.GetImageN("io",21), _("Help"));

	// Mülleimer
	AddImageButton(17, 49, 263, 30, 28, TC_GREY, LOADER.GetImageN("io", 106), _("Delete all"));

	// Zeiten
	AddTextButton(18, 81, 263, 36, 28, TC_GREY, _("15 m"), NormalFont);
	AddTextButton(19, 119, 263, 36, 28, TC_GREY, _("1 h"), NormalFont);
	AddTextButton(20, 155, 263, 36, 28, TC_GREY, _("4 h"), NormalFont);
	AddTextButton(21, 191, 263, 36, 28, TC_GREY, _("16 h"), NormalFont);



}

iwMerchandiseStatistics::~iwMerchandiseStatistics()
{

}
