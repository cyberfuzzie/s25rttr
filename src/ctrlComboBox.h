// $Id: ctrlComboBox.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef CTRLCOMBOBOX_H_INCLUDED
#define CTRLCOMBOBOX_H_INCLUDED

#pragma once

#include "Window.h"
#include "ctrlList.h"

class ctrlComboBox : public Window
{	
public:
	ctrlComboBox(Window *parent, unsigned int id, unsigned short x, unsigned short y, unsigned short width, unsigned short height, TextureColor tc, glArchivItem_Font *font, unsigned short max_list_height, bool readonly);
	
	void Resize_(unsigned short width, unsigned short height);

	void AddString(const std::string& text);
	void DeleteAllItems();

	void SetSelection(unsigned short selection);
	unsigned short GetSelection() const { return selection; };
	unsigned short GetCount() const { return GetCtrl<ctrlList>(0)->GetLineCount(); }
	const std::string& GetText(unsigned short item) const { return GetCtrl<ctrlList>(0)->GetItemText(item); }

	virtual void Msg_PaintAfter();
	virtual bool Msg_MouseMove(const MouseCoords& mc);
	virtual bool Msg_LeftDown(const MouseCoords& mc);
	virtual bool Msg_LeftUp(const MouseCoords& mc);
	virtual bool Msg_RightDown(const MouseCoords& mc);
	virtual bool Msg_WheelUp(const MouseCoords& mc);
	virtual bool Msg_WheelDown(const MouseCoords& mc);
	virtual void Msg_ListSelectItem(const unsigned int ctrl_id, const unsigned short selection);

protected:
	virtual bool Draw_(void);

	void ShowList(bool show);

private:
	TextureColor tc;
	glArchivItem_Font *font;
	unsigned short max_list_height;
	bool readonly;
	unsigned short selection;
	bool last_show;
};

#endif // CTRLCOMBOBOX_H_INCLUDED
