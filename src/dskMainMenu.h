// $Id: dskMainMenu.h 4793 2009-05-04 15:37:10Z OLiver $
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
#ifndef dskMAINMENU_H_INCLUDED
#define dskMAINMENU_H_INCLUDED

#pragma once

#include "Desktop.h"

/// Klasse des Hauptmenü Desktops.
class dskMainMenu : public Desktop
{
	void Msg_PaintAfter();
public:
	/// Konstruktor von @p dskMainMenu.
	dskMainMenu(void);

	void Msg_ButtonClick(const unsigned int ctrl_id);
};

#endif // !dskMAINMENU_H_INCLUDED
