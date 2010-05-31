// $Id: ctrlBuildingIcon.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef CTRLBUILDINGICON_H_INCLUDED
#define CTRLBUILDINGICON_H_INCLUDED

#pragma once

#include "ctrlButton.h"
#include "GameConsts.h"

class ctrlBuildingIcon : public ctrlButton
{
public:
	/// Konstruktor von @p ctrlBuildingIcon.
	ctrlBuildingIcon(Window *const parent, const unsigned int id, const unsigned short x, const unsigned short y,
		const BuildingType type, const Nation nation, const unsigned short size = 36,  const std::string& tooltip = "");
	/// liefert den GebäudeTyp des Icons.
	BuildingType GetType() const { return type; }

protected:
	/// zeichnet das Fenster.
	virtual bool Draw_(void);
	void DrawContent() const;

protected:
	const BuildingType type;       ///< der GebäudeType des Icons.
	const Nation nation;	   ///< Volk
	const unsigned short size; ///< die Größe des Icons
	
};

#endif // !CTRLBUILDINGICON_H_INCLUDED
