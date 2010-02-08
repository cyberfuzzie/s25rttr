// $Id$
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "AddonManager.h"

#include "Settings.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author FloSoft
 */
AddonManager::AddonManager()
{
	// register addons
	reset();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author FloSoft
 */
AddonManager::~AddonManager()
{
	// clear memory and dont register addons again
	reset(false);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  clears the addon memory.
 *
 *  if @p recreate is @p true then the addons are re-registered 
 *  and set to defaults
 *
 *  @author FloSoft
 */
void AddonManager::reset(bool recreate)
{
	for( std::vector<item>::iterator it = addons.begin(); it != addons.end(); ++it)
		delete it->addon;

	addons.clear();

	if(recreate)
	{
		registerAddon(new AddonChangeGoldDeposits);
		registerAddon(new AddonMaxWaterwayLength);
		registerAddon(new AddonCustomBuildSequence);
		registerAddon(new AddonStatisticsVisibility);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  loads the saved addon configuration from the SETTINGS.
 *
 *  @author FloSoft
 */
void AddonManager::LoadDefaults()
{
	reset();

	for( std::map<unsigned int, unsigned int>::iterator it = SETTINGS.addons.configuration.begin(); it != SETTINGS.addons.configuration.end(); ++it)
	{
		setSelection((Addons)it->first, it->second);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  saves the current addon configuration to the SETTINGS.
 *
 *  @author FloSoft
 */
void AddonManager::SetDefaults()
{
	for( std::vector<item>::iterator it = addons.begin(); it != addons.end(); ++it)
	{
		SETTINGS.addons.configuration.insert(std::make_pair(it->addon->getId(), it->status));
	}
}
