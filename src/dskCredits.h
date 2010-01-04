// $Id: dskCredits.h 5853 2010-01-04 16:14:16Z FloSoft $
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
#ifndef dskCREDITS_H_INCLUDED
#define dskCREDITS_H_INCLUDED

#pragma once

#include "Desktop.h"

#include <list>

/// Klasse des Credits Desktops.
class dskCredits : public Desktop
{
public:
	/// Konstruktor von @p dskCredits.
	dskCredits();
	~dskCredits();

private:
	bool Msg_LeftUp(const MouseCoords& mc);
	bool Msg_RightUp(const MouseCoords& mc);
	bool Msg_KeyDown(const KeyEvent& ke);
	void Msg_PaintAfter();

	bool Close(void);

	struct CreditsEntry {
		std::string title;
		std::string lastLine;
		int picId;
		struct Line {
			Line(std::string l, unsigned int c = 0) : line(l), column(c) { }
			std::string line;
			unsigned int column;
		};
		std::list<Line> lines;
	};

	std::list<CreditsEntry> entries;
	std::list<dskCredits::CreditsEntry>::iterator it;

	struct Bob {
		unsigned int id;
		bool hasWare;
		bool isFat;
		unsigned int direction;
		unsigned char speed;
		unsigned int animationStep;
		short x;
		short y;
		unsigned int color;
	};

	std::list<Bob> bobs;

	unsigned int startTime;
};

#endif // !dskCREDITS_H_INCLUDED
