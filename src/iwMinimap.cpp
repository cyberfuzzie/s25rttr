// $Id: iwMinimap.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "iwMinimap.h"
#include "ctrlIngameMinimap.h"
#include "Loader.h"
#include "ctrlButton.h"


/// (maximale) Gr��e des Minimapfensters normal
const unsigned short MINIMAP_WINDOW_WIDTH = 200;
const unsigned short MINIMAP_WINDOW_HEIGHT = 200;
/// (maximale) Gr��e des Minimapfensters gro�
const unsigned short MINIMAP_WINDOW_BIG_WIDTH = 400;
const unsigned short MINIMAP_WINDOW_BIG_HEIGHT = 400;

/// Abstand der Kartenr�nder zum Fensterrand
const unsigned short WINDOW_MAP_SPACE = 5;
/// Minimale Fensterbreite
const unsigned short MIN_WINDOW_WIDTH = 200;
/// Breite der unteren Buttons
const unsigned short BUTTON_WIDTH = 36;
/// H�he der unteren Buttons
const unsigned short BUTTON_HEIGHT = 36;
/// Abstand zwischen Buttons und Karte (Y)
const unsigned short BUTTON_MAP_SPACE = 3;
/// Abstand zwischen Buttons und unteren Fensterrand
const unsigned short BUTTON_WINDOW_SPACE = 5;



iwMinimap::iwMinimap(IngameMinimap * minimap, GameWorldViewer& gwv) 
: IngameWindow(CGI_MINIMAP,0xFFFF,0xFFF,MINIMAP_WINDOW_WIDTH,
	MINIMAP_WINDOW_HEIGHT,_("Outline map"),GetImage(resource_dat, 41)), extended(false)								   
{


	AddCtrl(0,new ctrlIngameMinimap(this,0,10,20,WINDOW_MAP_SPACE,WINDOW_MAP_SPACE,WINDOW_MAP_SPACE,WINDOW_MAP_SPACE,minimap,gwv));


	// Land, H�user, Stra�en an/aus
	for(unsigned i = 0;i<3;++i)
		AddImageButton(i+1,10+WINDOW_MAP_SPACE+BUTTON_WIDTH*i,0,BUTTON_WIDTH,BUTTON_HEIGHT,TC_GREY,GetImage(io_dat,85+i));

	// Fenster vergr��ern/verkleinern
	AddImageButton(4,0,0,BUTTON_WIDTH,BUTTON_HEIGHT,TC_GREY,GetImage(io_dat,109));


	ChangeWindowSize(width,height);

	// In die Mitte verschieben
	MoveToCenter();
}

/// Ver�ndert die Gr��e des Fensters und positioniert alle Controls etc. neu
void iwMinimap::ChangeWindowSize(const unsigned short width, const unsigned short height)
{
	SetWidth(width);
	SetHeight(height);

	ctrlIngameMinimap * im = GetCtrl<ctrlIngameMinimap>(0);

	im->SetDisplaySize(width-20,height-30);

	// Control k�rzen in der H�he
	im->RemoveBoundingBox(BUTTON_WIDTH*4+WINDOW_MAP_SPACE*2,0);

	////// Und nach ganz oben verschieben
	//im->Move(im->GetX(false),20);

	// Fensterbreite anpassen
	SetWidth(im->GetWidth()+20);
	SetHeight(im->GetHeight()+30+BUTTON_HEIGHT+BUTTON_MAP_SPACE+BUTTON_WINDOW_SPACE);


	// Buttonpositionen anpassen, nach unten verschieben
	for(unsigned i = 1;i<4;++i)
		GetCtrl<ctrlImageButton>(i)->Move(GetCtrl<ctrlImageButton>(i)->GetX(false),GetHeight()-10-BUTTON_HEIGHT-BUTTON_WINDOW_SPACE);

	// Vergr��ern/Verkleinern-Button nach unten rechts verschieben
	GetCtrl<ctrlImageButton>(4)->Move(GetWidth()-10-BUTTON_WIDTH-WINDOW_MAP_SPACE,GetHeight()-10-BUTTON_HEIGHT-BUTTON_WINDOW_SPACE);

	// Bild vom Vergr��ern/Verkleinern-Button anpassen
	GetCtrl<ctrlImageButton>(4)->SetImage(GetImage(io_dat,extended ? 108 : 109));
}

void iwMinimap::Msg_ButtonClick(const unsigned ctrl_id)
{
	switch(ctrl_id)
	{
	case 1: GetCtrl<ctrlIngameMinimap>(0)->ToggleTerritory(); break;
	case 2: GetCtrl<ctrlIngameMinimap>(0)->ToggleHouses(); break;
	case 3: GetCtrl<ctrlIngameMinimap>(0)->ToggleRoads(); break;
	case 4: 
		{
			// Fenster vergr��ern/verkleinern
			this->extended = !extended;

			ChangeWindowSize(extended ? MINIMAP_WINDOW_BIG_WIDTH : MINIMAP_WINDOW_WIDTH,
				extended ? MINIMAP_WINDOW_BIG_HEIGHT : MINIMAP_WINDOW_HEIGHT);
		} break;
	}
}

