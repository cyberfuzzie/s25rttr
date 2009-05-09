// $Id: ctrlRectangle.h 4840 2009-05-09 09:16:39Z OLiver $
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
#ifndef CTRL_RECTANGLE_H_
#define CTRL_RECTANGLE_H_

#include "Window.h"

/// Control, welche ein eine feste Breite und Höhe besitzen
class ctrlRectangle : public Window
{
public:

	ctrlRectangle(Window *parent, unsigned int id, unsigned short x, unsigned short y,
		unsigned short width, unsigned short height);

	/// Setzt Breite und Höhe
	virtual void SetWidth(const unsigned short width) { this->width = width; }
	virtual void SetHeight(const unsigned short height) { this->height = height; }

	/// Liefert Breite und Höhe zurück
	unsigned short GetWidth() const { return width; }
	unsigned short GetHeight() const { return height; }

protected:

	/// Breite und Höhe des Controls
	unsigned short width, height;
};


#endif //!CTRL_RECTANGLE_H_
