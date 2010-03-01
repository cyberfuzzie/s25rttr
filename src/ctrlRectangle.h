// $Id: ctrlRectangle.h 5853 2010-01-04 16:14:16Z FloSoft $
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
	unsigned short GetWidth(const bool scale = false) const { return (scale) ? ScaleX(width) : width; }
	unsigned short GetHeight(const bool scale = false) const { return (scale) ? ScaleY(height) : height; }

protected:

	/// Breite und Höhe des Controls
	unsigned short width, height;
};

/// Interface für Klassen, in denen eine Farbe angezeigt wird
class ColorControlInterface
{
public:
	virtual ~ColorControlInterface(void) { }

	/// Setzt die Farbe des Controls
	virtual void SetColor(const unsigned int fill_color) = 0; 
};


#endif //!CTRL_RECTANGLE_H_
