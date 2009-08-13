// $Id: AIJHHelper.h 5344 2009-07-29 16:23:40Z jh $
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
#ifndef AIJHHELPER_H_INCLUDED
#define AIJHHELPER_H_INCLUDED

#pragma once

//#define DEBUG_AI

#include "MapConsts.h"
#include "GameConsts.h"

#include <vector>

class AIPlayerJH;

namespace AIJH
{

enum Resource
{
	WOOD,
	STONES,
	GOLD,
	IRONORE,
	COAL,
	GRANITE,
	PLANTSPACE,
	BORDERLAND,
	FISH,
	NOTHING = 255
};

const unsigned RES_TYPE_COUNT = 9;
const unsigned RES_RADIUS[RES_TYPE_COUNT] = {8, 8, 2, 2, 2, 2, 5, 5, 6};

struct Node
{
	bool owned;
	bool reachable;
	BuildingQuality bq;
	Resource res;
	bool border;
};

enum JobStatus
{
	JOB_WAITING,
	JOB_EXECUTING,
	JOB_FINISHED,
	JOB_FAILED
};

class Job
{
public:
	Job(AIPlayerJH *aijh) : aijh(aijh), status(AIJH::JOB_WAITING) { }
	~Job() { }
	virtual void ExecuteJob() { return; }
	JobStatus GetStatus() { return status; }
protected:
	JobStatus status;
	AIPlayerJH *aijh;
};

class BuildJob : public Job
{
public:
	BuildJob(AIPlayerJH *aijh, BuildingType type) : Job(aijh), type(type), target_x(0xFFFF), target_y(0xFFFF), around_x(0xFFFF), around_y(0xFFFF) { }
	BuildJob(AIPlayerJH *aijh, BuildingType type, MapCoord around_x, MapCoord around_y) : Job(aijh), type(type), target_x(0xFFFF), target_y(0xFFFF), around_x(around_x), around_y(around_y) { }
	~BuildJob() { }
	void ExecuteJob();
private:
	BuildingType type;
	MapCoord around_x, around_y;
	MapCoord target_x, target_y;
	std::vector<unsigned char> route;
};

class ExpandJob : public Job
{
public:
	ExpandJob(AIPlayerJH *aijh) : Job(aijh) { }
	~ExpandJob() { }
	void ExecuteJob();
private:
	BuildingType type;
	MapCoord target_x, target_y;
	std::vector<unsigned char> route;
};

class ConnectJob : public Job
{
public:
	ConnectJob(AIPlayerJH *aijh, MapCoord x1, MapCoord y1, MapCoord x2, MapCoord y2);
	~ConnectJob() { }
	void ExecuteJob();
private:
	MapCoord x1, y1, x2, y2;
};

}


#endif //!AIJHHELPER_H_INCLUDED
