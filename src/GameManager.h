// $Id: GameManager.h 5989 2010-02-10 14:13:58Z FloSoft $
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
#ifndef GAMEMANAGER_H_INCLUDED
#define GAMEMANAGER_H_INCLUDED

#pragma once

#include "Singleton.h"

/// "Die" GameManager-Klasse
class GameManager : public Singleton<GameManager>
{
public:
	GameManager(void);

	bool Start();
	void Stop();
	bool Run();

	bool StartMenu();
	bool ShowMenu();

	/// Average FPS Zähler zurücksetzen.
	inline void ResetAverageFPS(void)
	{
		run_time = 0;
		frame_count = 0;
	}

	inline unsigned int GetRuntime(void)
	{ 
		return run_time;
	}

	inline unsigned int GetFrameCount(void)
	{ 
		return frame_count;
	}

	inline unsigned int GetAverageFPS(void) 
	{
		if(run_time == 0) 
			return 0;
		return (frame_count / run_time);
	}

	inline unsigned int GetFPS(void) 
	{
		return framerate;
	}

private:
	unsigned int frames;
	unsigned int frame_count;
	unsigned int framerate;
	unsigned int frame_time;
	unsigned int run_time;
	unsigned int last_time;
};

#define GAMEMANAGER GameManager::inst()

#endif // GAMEMANAGER_H_INCLUDED
