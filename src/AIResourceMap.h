// $Id: AIResourceMap.h 5881 2010-01-10 02:06:52Z jh $
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
#ifndef AIRESOURCEMAP_H_INCLUDED
#define AIRESOURCEMAP_H_INCLUDED

#pragma once

#include <vector>
#include "AIPlayerJH.h"


class AIResourceMap
{
public:
	AIResourceMap(const GameWorldBase * const gwb, const std::vector<AIJH::Node> &nodes);
	~AIResourceMap();

	/// Initialize the resource map
	void Init();

	/// Changes a single resource map around point x,y in radius; to every point around x,y distanceFromCenter * value is added
	void ChangeResourceMap(MapCoord x, MapCoord y, unsigned radius, int value);

	/// Finds a good position for a specific resource in an area using the resource maps, 
	/// first position satisfying threshold is returned, returns false if no such position found
	bool FindGoodPosition(MapCoord &x, MapCoord &y, int threshold, BuildingQuality size, int radius = -1, bool inTerritory = true);

	/// Finds the best position for a specific resource in an area using the resource maps, 
	/// satisfying the minimum value, returns false if no such position is found
	bool FindBestPosition(MapCoord &x, MapCoord &y, BuildingQuality size, int minimum, int radius = -1, bool inTerritory = true);
	bool FindBestPosition(MapCoord &x, MapCoord &y, BuildingQuality size, int radius = -1, bool inTerritory = true) 
	{ return FindBestPosition(x,y,size,1,radius,inTerritory); }

private:
	std::vector<int> map;
	const GameWorldBase * const gwb;
	const std::vector<AIJH::Node> &nodes;
	AIJH::Resource res;
};

#endif //! AIRESOURCEMAP_H_INCLUDED
