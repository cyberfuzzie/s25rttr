// $Id: Desktop.cpp 5853 2010-01-04 16:14:16Z FloSoft $
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
#include "Desktop.h"

#include "WindowManager.h"
#include "VideoDriverWrapper.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Konstruktor für einen Spieldesktop
 *
 *  @param[in] background Hintergrund des Desktops
 *
 *  @author OLiver
 */
Desktop::Desktop(glArchivItem_Bitmap *background)
	: Window(), background(background)
{
	SetScale(true);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeichenmethode zum Zeichnen des Desktops
 *  und der ggf. enthaltenen Steuerelemente.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author OLiver
 */
bool Desktop::Draw_(void)
{
	if(background)
		background->Draw(0, 0, VideoDriverWrapper::inst().GetScreenWidth(), VideoDriverWrapper::inst().GetScreenHeight(), 0, 0, 0, 0);

	DrawControls();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  Wechselt den aktuellen Desktop im WindowManager auf diesen Desktop.
 *
 *  @author OLiver
 */
void Desktop::Show(void)
{
	WindowManager::inst().Switch(this);
}
