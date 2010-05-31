// $Id: ctrlRectangle.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef CTRL_RECTANGLE_H_
#define CTRL_RECTANGLE_H_

#include "Window.h"

/// Interface für Klassen, in denen eine Farbe angezeigt wird
class ColorControlInterface
{
public:
	virtual ~ColorControlInterface(void) { }

	/// Setzt die Farbe des Controls
	virtual void SetColor(const unsigned int fill_color) = 0; 
};


#endif //!CTRL_RECTANGLE_H_
