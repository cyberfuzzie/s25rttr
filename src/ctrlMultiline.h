// $Id: ctrlMultiline.h 4840 2009-05-09 09:16:39Z OLiver $
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
#ifndef CTRL_MULTILINE_H_INCLUDED
#define CTRL_MULTILINE_H_INCLUDED

#include <vector>

#include "ctrlRectangle.h"


class ctrlMultiline : public ctrlRectangle
{
public:

	/// Breite der Scrollbar
	static const unsigned short SCROLLBAR_WIDTH = 20;

public:
	ctrlMultiline(Window *parent, unsigned int id, unsigned short x, unsigned short y, unsigned short width, unsigned short height, TextureColor tc, glArchivItem_Font *font, unsigned int format = 0);

	//void AddText(const std::string& text, unsigned int color);
	void AddString(const std::string& str, unsigned int color, bool scroll = true);
	unsigned GetLineCount() { return unsigned(lines.size()); }
	/// Gibt den index-ten Eintrag zurück
	const std::string& GetLine(const unsigned index) const { return lines[index].str; }

	/// Schaltet Box ein und aus
	void EnableBox(const bool enable) { draw_box = enable; }

	virtual bool Msg_LeftDown(const MouseCoords& mc);
	virtual bool Msg_LeftUp(const MouseCoords& mc);
	virtual bool Msg_WheelUp(const MouseCoords& mc);
	virtual bool Msg_WheelDown(const MouseCoords& mc);
	virtual bool Msg_MouseMove(const MouseCoords& mc);

	/// SetWidth überschreiben, damit auch die Scrollbar-Position angepasst werden kann
	void SetWidth(const unsigned short width);

protected:
	virtual bool Draw_(void);

private:

	TextureColor tc;
	glArchivItem_Font *font;
	unsigned int format;

	
	struct Line
	{
		std::string str;
		unsigned int color;
	};
	/// Die ganzen Strings
	std::vector<Line> lines;
	/// Anzahl der Zeilen, die in das Control passen
	unsigned lines_in_control;

	/// Soll die Box gezeichnet werden?
	bool draw_box;
};

#endif /// !CTRLMULTILINE_H_INCLUDED
