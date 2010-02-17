// $Id: dskSplash.h 6037 2010-02-17 11:26:49Z FloSoft $
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
#ifndef dskSPLASH_H_INCLUDED
#define dskSPLASH_H_INCLUDED

#pragma once

#include "Desktop.h"

/// Klasse des Splashscreen Desktops.
class dskSplash : public Desktop
{
public:
	/// Konstruktor von @p dskSplash.
	dskSplash();
	/// Destruktor von @p dskSplash.
	~dskSplash();

private:

	void Msg_Timer(const unsigned int ctrl_id);
	bool Msg_LeftDown(const MouseCoords& mc);
};

#endif // !dskSPLASH_H_INCLUDED
