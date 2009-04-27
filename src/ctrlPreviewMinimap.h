// $Id: ctrlPreviewMinimap.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef PREVIEWMINIMAP_H_
#define PREVIEWMINIMAP_H_

#include "ctrlMinimap.h"
#include "Minimap.h"
#include "MapConsts.h"

/// Übersichtskarte (MapPreview)
class ctrlPreviewMinimap : public ctrlMinimap
{
	/// Minimap
	PreviewMinimap minimap;
	/// Maximale Anzahl der Spieler
	static const unsigned MAX_PLAYERS = 8;
	/// Startpositionen der Spieler
	struct Player
	{
		Player();
		/// Koordinaten relativ zur Control-Position
		unsigned short x,y;
		/// Farbe
		unsigned color;
	} players[MAX_PLAYERS];

public:

	ctrlPreviewMinimap( Window *parent, 
				 const unsigned int id, 
				 const unsigned short x, 
				 const unsigned short y, 
				 const unsigned short width, 
				 const unsigned short height,
				 glArchivItem_Map *s2map);

	/// Zeichnet die MapPreview
	bool Draw_();

	/// Setzt die (Start-)Farbe eines Spielers bzw. löscht diesen (color = 0)
	void SetPlayerColor(const unsigned id, const unsigned color)
	{
		players[id].color = color;
	}

	void SetMap(glArchivItem_Map *s2map)
	{
		if(s2map)
			SetDisplaySize(width, height, s2map->getHeader().getWidth(), s2map->getHeader().getHeight());
		minimap.SetMap(s2map);
	}
};


#endif // !MapPreview_H_

