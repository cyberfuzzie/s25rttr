// $Id: ctrlEdit.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef CTRLEDIT_H_INCLUDED
#define CTRLEDIT_H_INCLUDED

#pragma once

#include "Window.h"

class ctrlEdit : public Window
{
public:
	/// Konstruktor von @p ctrlEdit.
	ctrlEdit(Window *parent, unsigned int id, unsigned short x, unsigned short y, unsigned short width, unsigned short height, TextureColor tc, glArchivItem_Font *font, unsigned short maxlength = 0, bool password = false, bool disabled = false, bool notify = false);
	/// setzt den Text.
	void SetText(const std::string& text);

	const std::string& GetText(void) const { return text; }
	void SetFocus(bool focus = true) { newfocus = focus; }
	void SetDisabled(bool disabled = true) { this->disabled = disabled; }
	void SetNotify(bool notify = true) { this->notify = notify; }
	void SetMaxLength(unsigned short maxlength = 0) { this->maxlength = maxlength; }

	virtual void Msg_PaintAfter();
	virtual bool Msg_LeftDown(const MouseCoords& mc);
	virtual bool Msg_LeftDown_After(const MouseCoords& mc);
	virtual bool Msg_KeyDown(const KeyEvent& ke);

protected:
	virtual bool Draw_(void);

private:
	void AddChar(char c);
	void RemoveChar(void);
	void Notify(void);

	void CursorLeft() { if(cursor_pos == 0) return; --cursor_pos; Notify(); };
	void CursorRight() { if(cursor_pos == text.length()) return; ++cursor_pos; Notify(); };

private:
	unsigned short width;
	unsigned short height;
	unsigned short maxlength;
	TextureColor tc;
	glArchivItem_Font *font;
	bool password;
	bool disabled;
	bool focus;
	bool newfocus;
	bool notify;

	std::string text;
	unsigned cursor_pos;
	unsigned view_start;
};

#endif // !CTRLEDIT_H_INCLUDED
