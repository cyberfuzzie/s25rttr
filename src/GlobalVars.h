// $Id: GlobalVars.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef GLOBAL_VARS_H_INCLUDED
#define GLOBAL_VARS_H_INCLUDED

#pragma once

#include "Singleton.h"

/// Klasse für alle "globalen" Variablen/Objekte
class GlobalVars : public Singleton<GlobalVars>
{
public:
	/// Konstruktor von @p GlobalVars.
	GlobalVars(void);

public:
	bool notdone;
	bool ingame;
	bool ext_vbo;
	bool ext_swapcontrol;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define GLOBALVARS GlobalVars::inst()

#endif // GLOBAL_VARS_H_INCLUDED
