// $Id: AIJHHelper.h 5344 2009-07-29 16:23:40Z jh $
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
#ifndef AIJHHELPER_H_INCLUDED
#define AIJHHELPER_H_INCLUDED

#pragma once

//#define DEBUG_AI

#include "main.h"
#include "MapConsts.h"
#include "GameConsts.h"
#include "AIEventManager.h"


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
const unsigned RES_RADIUS[RES_TYPE_COUNT] = 
{
	8, // Wood
	8, // Stones
	2, // Gold
	2, // Ironore
	2, // Coal
	2, // Granite
	3, // Plantspace
	5, // Borderland
	5 // Fish
};

struct Node
{
	bool owned;
	bool reachable;
	BuildingQuality bq;
	Resource res;
	bool border;
	bool farmed;
};

enum JobStatus
{
	JOB_WAITING,
	JOB_EXECUTING_START,
	JOB_EXECUTING_ROAD1,
	JOB_EXECUTING_ROAD2,
	JOB_EXECUTING_ROAD2_2,
	JOB_FINISHED,
	JOB_FAILED
};

class Job
{
public:
	Job(AIPlayerJH *aijh) : aijh(aijh), status(AIJH::JOB_WAITING) { }
	virtual ~Job() { }
	virtual void ExecuteJob() { return; }
	JobStatus GetStatus() { return status; }
protected:
	AIPlayerJH *aijh;
	JobStatus status;
};

class BuildJob : public Job
{
public:
	BuildJob(AIPlayerJH *aijh, BuildingType type) 
		: Job(aijh), type(type), target_x(0xFFFF), target_y(0xFFFF), around_x(0xFFFF), around_y(0xFFFF) { }
	BuildJob(AIPlayerJH *aijh, BuildingType type, MapCoord around_x, MapCoord around_y) 
		: Job(aijh), type(type), target_x(0xFFFF), target_y(0xFFFF), around_x(around_x), around_y(around_y) { }
	~BuildJob() { }
	void ExecuteJob();
private:
	BuildingType type;
	MapCoord target_x, target_y;
	MapCoord around_x, around_y;
	std::vector<unsigned char> route;

	void TryToBuild();
	void BuildMainRoad();
	void TryToBuildSecondaryRoad();
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
	ConnectJob(AIPlayerJH *aijh, MapCoord flag_x, MapCoord flag_y, MapCoord target_x, MapCoord target_y) 
		: Job(aijh), flag_x(flag_x), flag_y(flag_y), target_x(target_x), target_y(target_y) { }
	~ConnectJob() { }
	void ExecuteJob();
private:
	MapCoord flag_x, flag_y, target_x, target_y;
};

class EventJob : public Job
{
public:
	EventJob(AIPlayerJH *aijh, AIEvent::Base *ev) : Job(aijh), ev(ev) { }
	~EventJob() { delete ev; }
	void ExecuteJob();
private:
	AIEvent::Base *ev;
};

}


#endif //!AIJHHELPER_H_INCLUDED
