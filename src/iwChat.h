// $Id: iwChat.h 5853 2010-01-04 16:14:16Z FloSoft $
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
#ifndef iwCHAT_H_INCLUDED
#define iwCHAT_H_INCLUDED

#pragma once

#include "IngameWindow.h"

class iwChat : public IngameWindow
{
private:
	/// Chat-Destination auch merken, wenn das Fenster zugegangen ist
	static unsigned char chat_dest;

public:
	iwChat();

private:

	void Msg_PaintBefore();
	void Msg_OptionGroupChange(const unsigned int ctrl_id, const unsigned short selection);
	void Msg_EditEnter(const unsigned int ctrl_id);
};

#endif // !iwCHAT_H_INCLUDED
