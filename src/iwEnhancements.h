// $Id: iwEnhancements.h 4800 2009-05-11 10:10:02Z Divan $
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
#ifndef iwENHANCEMENTS_H_INCLUDED
#define iwENHANCEMENTS_H_INCLUDED

#pragma once

#include "IngameWindow.h"
#include "enhOption.h"
#include "dskHostGame.h"

#define ENH_POLICY_ALL 0
#define ENH_POLICY_HOSTMENU 1
#define ENH_POLICY_SERVERINGAME 2
#define ENH_POLICY_READONLY 255

class iwEnhancements : public IngameWindow
{
public:
	iwEnhancements(enhOptions * original, const unsigned char policy, void (dskHostGame::*notifyFunc)() = NULL, dskHostGame * notifyInst = NULL);
	~iwEnhancements(void);

private:
	void SetText(const std::string& text, unsigned int color, bool button);

	void Msg_CheckboxChange(const unsigned int ctrl_id, const bool check);
	void Msg_ComboSelectItem(const unsigned int ctrl_id, const  unsigned short selection);
	void Msg_EditChange(const unsigned int ctrl_id);
	void Msg_EditEnter(const unsigned int ctrl_id);
	void Msg_ButtonClick(const unsigned int ctrl_id);

private:
	enhOptions *enhs, *original;
	unsigned char policy;
	void (dskHostGame::*notifyFunc)();
	dskHostGame * notifyInst;

};

#endif // !iwENHANCEMENTS_H_INCLUDED
