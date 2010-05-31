// $Id: AIBase.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef AIBASE_H_INCLUDED
#define AIBASE_H_INCLUDED

#pragma once

#include "AIEventManager.h"

class GameWorldBase;
class GameClientPlayer;
class GlobalGameSettings;
class GameClientPlayerList;
namespace gc { class GameCommand; }

namespace AI
{
	enum Level
	{
		EASY = 0,
		MEDIUM,
		HARD
	};
}

/// Basisklasse für sämtliche KI-Spieler
class AIBase
{
protected:
	/// Eigene PlayerID, die der KI-Spieler wissen sollte, z.B. wenn er die Karte untersucht
	const unsigned char playerid;
	/// Verweis auf die Spielwelt, um entsprechend Informationen daraus zu erhalten
	const GameWorldBase * const gwb;
	/// Verweis auf den eigenen GameClientPlayer, d.h. die Wirtschaft, um daraus entsprechend Informationen zu gewinnen
	const GameClientPlayer * const player;
	/// Verweis auf etwaige andere Spieler, bspw. um deren Bündnisse zu überprüfen etc.
	const GameClientPlayerList * const players;
	/// Verweis auf die Globalen Spieleinstellungen, da diese auch die weiteren Entscheidungen beeinflussen können
	/// (beispielsweise Siegesbedingungen, FOW usw.)
	const GlobalGameSettings * const ggs;
	/// Queue der GameCommands, die noch bearbeitet werden müssen
	std::vector<gc::GameCommand*> gcs;
	/// Stärke der KI
	const AI::Level level;

public:

	AIBase(const unsigned char playerid, const GameWorldBase * const gwb, const GameClientPlayer * const player,
		const GameClientPlayerList * const players, const GlobalGameSettings * const ggs, const AI::Level level)
		: playerid(playerid), gwb(gwb), player(player), players(players), ggs(ggs), level(level) {}

	virtual ~AIBase() {}

	/// Wird jeden GF aufgerufen und die KI kann hier entsprechende Handlungen vollziehen
	virtual void RunGF(const unsigned gf) = 0;

	/// Zugriff auf die GameCommands, um diese abarbeiten zu können
	const std::vector<gc::GameCommand*>& GetGameCommands() const { return gcs; }
	/// Markiert die GameCommands als abgearbeitet
	void FetchGameCommands() { gcs.clear(); }

	virtual void SendAIEvent(AIEvent::Base *ev) { delete ev; }
};

#endif //!AIBASE_H_INCLUDED
