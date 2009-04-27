// $Id: iwPostWindow.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "iwPostWindow.h"
#include "ctrlText.h"
#include "ctrlButton.h"
#include "ctrlTable.h"
#include "WindowManager.h"
#include "Loader.h"
#include "const_gui_ids.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

iwPostWindow::iwPostWindow(void) : IngameWindow(CGI_POSTOFFICE, 0xFFFF, 0xFFFF, 254, 295, _("Post office"), GetImage(resource_dat, 41))
{
	AddImageButton( 0, 18, 25, 35, 35,TC_GREY,GetImage(io_dat, 190));		// Viewer: 191 - Papier
	AddImageButton( 1, 56, 25, 35, 35,TC_GREY,GetImage(io_dat, 30));		// Viewer:  31 - Soldat
	AddImageButton( 2, 91, 25, 35, 35,TC_GREY,GetImage(io_dat, 20));		// Viewer:  21 - Geologe
	AddImageButton( 3,126, 25, 35, 35,TC_GREY,GetImage(io_dat, 28));		// Viewer:  29 - Wage
	AddImageButton( 4,161, 25, 35, 35,TC_GREY,GetImage(io_dat, 189));		// Viewer: 190 - Neue Nachricht
	AddImageButton( 5,199, 25, 35, 35,TC_GREY,GetImage(io_dat, 79));		// Viewer:  80 - Notiz
	AddImage(  6,126,151, GetImage(io_dat, 228));
	AddImageButton( 7, 18,242, 30, 35,TC_GREY,GetImage(io_dat, 225));		// Viewer: 226 - Hilfe
	AddImageButton( 8, 51,246, 30, 26,TC_GREY,GetImage(io_dat, 102));		// Viewer: 103 - Schnell zurck
	AddImageButton( 9, 81,246, 30, 26,TC_GREY,GetImage(io_dat, 103));		// Viewer: 104 - Zurck
	AddImageButton(10,111,246, 30, 26,TC_GREY,GetImage(io_dat, 104));		// Viewer: 105 - Vor
	AddImageButton(11,141,246, 30, 26,TC_GREY,GetImage(io_dat, 105));		// Viewer: 106 - Schnell vor
	AddText(  12,126,151, _("No letters!"), 0xFF886034, glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_BOTTOM, NormalFont);
}
