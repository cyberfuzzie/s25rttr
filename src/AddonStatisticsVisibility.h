// $Id: AddonStatisticsVisibility.h 6000 2010-02-11 17:19:50Z FloSoft $
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
#ifndef ADDONSTATISTICSVISIBILITY_H_INCLUDED
#define ADDONSTATISTICSVISIBILITY_H_INCLUDED

#pragma once

#include "Addons.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for changing the maximum length of waterways.
 *
 *  @author Divan
 */
class AddonStatisticsVisibility : public AddonList
{
public:
	AddonStatisticsVisibility() : AddonList(ADDON_STATISTICS_VISIBILITY, 
											ADDONGROUP_OTHER,
											gettext_noop("Change the visibility of your ingame statistics"), 
											gettext_noop("Decides to whom your statistics is visible.\n\n"
														 "\"Allies\" applies to team members as well as "
														 "to allies by treaty."),
											0
										   )
	{
		addOption(gettext_noop("Everyone"));
		addOption(gettext_noop("Allies"));
		addOption(gettext_noop("Nobody else but you"));	
	}
};

#endif // !ADDONSTATISTICSVISIBILITY_H_INCLUDED
