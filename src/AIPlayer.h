// $Id: AIPlayer.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef AIPLAYER_H_INCLUDED
#define AIPLAYER_H_INCLUDED

#pragma once

#include "AIBase.h"

/// Klasse für die standardmäßige (vorerst) KI
class AIPlayer : public AIBase
{
public:
	AIPlayer(const unsigned char playerid, const GameWorldBase * const gwb, const GameClientPlayer * const player,
		const GameClientPlayerList * const players, const GlobalGameSettings * const ggs,
		const AI::Level level);

	/// Wird jeden GF aufgerufen und die KI kann hier entsprechende Handlungen vollziehen
	/// gf ist die GF-Zahl vom Spiel
	void RunGF(const unsigned gf);
};

#endif //!AIPLAYER_H_INCLUDED
