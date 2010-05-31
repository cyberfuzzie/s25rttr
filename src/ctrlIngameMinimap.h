// $Id: ctrlIngameMinimap.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef CTRL_INGAMEMINIMAP_H_
#define CTRL_INGAMEMINIMAP_H_

#include "ctrlMinimap.h"

class GameWorldViewer;
class IngameMinimap;

/// Minimap-Control für Ingame
class ctrlIngameMinimap : public ctrlMinimap
{
	/// Zeiger auf Minimap (die im Spiel dauerhaft!! gespeichert werden muss)
	IngameMinimap * minimap;
	/// Referenz auf GameWorldViewer, für das Gescrolle
	GameWorldViewer& gwv;

public:

	ctrlIngameMinimap( Window *parent, 
				 const unsigned int id, 
				 const unsigned short x, 
				 const unsigned short y, 
				 const unsigned short width, 
				 const unsigned short height,
				 const unsigned short padding_x,
				 const unsigned short padding_y,
				 IngameMinimap * minimap,
				 GameWorldViewer& gwv);

	/// Zeichnet die MapPreview
	bool Draw_();

	bool Msg_LeftDown(const MouseCoords& mc);
	bool Msg_MouseMove(const MouseCoords& mc);

	/// Setzt Breite und Höhe des Controls
	void SetDisplaySize(const unsigned short width, const unsigned short height);

	/// Die einzelnen Dinge umschalten
	void ToggleTerritory();
	void ToggleHouses();
	void ToggleRoads();
};


#endif
