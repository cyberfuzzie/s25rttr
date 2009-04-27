// $Id: ctrlScrollBar.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef CTRLSCROLLBAR_H_INCLUDED
#define CTRLSCROLLBAR_H_INCLUDED

#pragma once

#include "Window.h"

class ctrlScrollBar : public Window
{
public:
	ctrlScrollBar(Window *parent,unsigned int id, unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned short button_height, TextureColor tc, unsigned short pagesize);
	void SetPos(unsigned short scroll_pos);
	void SetRange(unsigned short scroll_range);
	void SetPageSize(unsigned short pagesize);

	unsigned short GetPos() { return scroll_pos; }

	void SetHeight(unsigned short height);
	unsigned short GetHeight() const { return height; }
	void SetWidth(unsigned short width) { this->width = width; }
	unsigned short GetWidth() const { return width; }

	virtual bool Msg_LeftUp(const MouseCoords& mc);
	virtual bool Msg_LeftDown(const MouseCoords& mc);
	virtual bool Msg_MouseMove(const MouseCoords& mc);
	virtual void Msg_ButtonClick(const unsigned int ctrl_id);

protected:
	virtual bool Draw_(void);

private:
	void CalculatePosition() { scroll_pos = (scrollbar_pos * scroll_range / scroll_height); }
	void CalculateScrollBar();

private:
	unsigned short width;
	unsigned short height;
	unsigned short button_height;
	TextureColor tc;
	unsigned short pagesize;

	bool move;
	unsigned short scroll_range;
	unsigned short scroll_pos;
	unsigned short scroll_height;
	unsigned short scrollbar_height;
	unsigned short scrollbar_pos;

	int last_y;
};

#endif // !CTRLSCROLLBAR_H_INCLUDED
