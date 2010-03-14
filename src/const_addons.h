// $Id: const_addons.h 6150 2010-03-13 23:17:32Z jh $
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
#ifndef CONST_ADDONS_H_INCLUDED
#define CONST_ADDONS_H_INCLUDED

#pragma once

// Addon Author List
// 
// 000 FloSoft
// 001 OLiver
// 002 Divan
// 003 jh
//

// Do not forget to add your Addon to AddonManager::reset!
// Never use a number twice!

enum AddonId
{
	// AAA = Author
	// NNNNN = Number
	//                                  AAANNNNN
	ADDON_LIMIT_CATAPULTS			= 0x00000000,
	ADDON_INEXHAUSTIBLE_MINES		= 0x00000001,
	ADDON_REFUND_MATERIALS			= 0x00000002,
	ADDON_EXHAUSTIBLE_WELLS			= 0x00000003,
	ADDON_REFUND_ON_EMERGENCY		= 0x00000004,
	ADDON_MANUAL_ROAD_ENLARGEMENT	= 0x00000005,

	ADDON_DEMOLITION_PROHIBITION	= 0x00100000,

	ADDON_CHANGE_GOLD_DEPOSITS		= 0x00200000,
	ADDON_MAX_WATERWAY_LENGTH		= 0x00200001,
	ADDON_CUSTOM_BUILD_SEQUENCE		= 0x00200002,
	ADDON_STATISTICS_VISIBILITY		= 0x00200003,

	ADDON_DEFENDER_BEHAVIOR			= 0x00300000
};

enum AddonGroup
{
	ADDONGROUP_ALL = 1,
	ADDONGROUP_MILITARY = 2,
	ADDONGROUP_ECONOMY = 4,
	ADDONGROUP_GAMEPLAY = 8,
	ADDONGROUP_OTHER = 16
};

#endif // !CONST_ADDONS_H_INCLUDED
