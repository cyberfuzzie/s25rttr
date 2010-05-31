// $Id: AddonLimitCatapults.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef ADDONLIMITCATAPULTS_H_INCLUDED
#define ADDONLIMITCATAPULTS_H_INCLUDED

#pragma once

#include "Addons.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for limiting the count of catapults
 *
 *  @author FloSoft
 */
class AddonLimitCatapults : public AddonList
{
public:
	AddonLimitCatapults() : AddonList(ADDON_LIMIT_CATAPULTS, 
									  ADDONGROUP_MILITARY,
									  gettext_noop("Limit count of catapults"), 
									  gettext_noop("Limits the count of catapults per player.\n\n"
												   "Proportional uses 0.125 catapult per barrack (you need 8 baracks for one catapult)\n"
												   "                  0.25           per guardhouse (4 guardhouses per catapult)\n"
												   "                  0.5            per watchtower (2 watchtower per catapult)\n"
												   "                  1              per fortress (1 fortress per catapult)\n"),
									  0
									 )
	{
		addOption(gettext_noop("Unlimited"));
		addOption(gettext_noop("Proportional"));
		addOption(gettext_noop("No catapults"));
		addOption(gettext_noop("10 catapults"));
		addOption(gettext_noop("20 catapults"));
		addOption(gettext_noop("30 catapults"));
		addOption(gettext_noop("40 catapults"));
		addOption(gettext_noop("50 catapults"));
	}
};

#endif // !ADDONLIMITCATAPULTS_H_INCLUDED
