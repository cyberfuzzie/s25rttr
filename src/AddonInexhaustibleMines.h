// $Id: AddonInexhaustibleMines.h 6000 2010-02-11 17:19:50Z FloSoft $
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
#ifndef ADDONINEXHAUSTIBLEMINES_H_INCLUDED
#define ADDONINEXHAUSTIBLEMINES_H_INCLUDED

#pragma once

#include "Addons.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for allowing to have unlimited resources.
 *
 *  @author Divan
 */
class AddonInexhaustibleMines : public AddonBool
{
public:
	AddonInexhaustibleMines() : AddonBool(ADDON_INEXHAUSTIBLE_MINES,
										  ADDONGROUP_ECONOMY,
										  gettext_noop("Inexhaustible Mines"), 
										  gettext_noop("Allows to have unlimited resources\n\n"
													   "Mines will never be depleted"),
										  0
										 )
	{
	}
};

#endif // !ADDONINEXHAUSTIBLEMINES_H_INCLUDED
