// $Id: GamePlayerInfo.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef GAMEPLAYERINFO_H_INCLUDED
#define GAMEPLAYERINFO_H_INCLUDED

#include "GameProtocol.h"
#include "GameMessageQueue.h"
#include "Socket.h"
#include "GameConsts.h"
#include <string>


enum PlayerState
{
	PS_FREE = 0,
	PS_RESERVED,
	PS_OCCUPIED,
	PS_LOCKED,
	PS_KI
};

/*struct NFCQueueItem {
	GameMessage *nfc;
	unsigned int nr;
};*/

class GamePlayerInfo
{
public:

	GamePlayerInfo(const unsigned playerid);
	virtual ~GamePlayerInfo();

	void clear();
	bool isValid();

	/// Ist Spieler besiegt?
	bool IsDefeated() const { return defeated; }
	

protected:

	/// Wechselt Spieler
	void SwapPlayer(GamePlayerInfo& two);
	
protected:

	/// Player-ID
	unsigned playerid;
	/// Besiegt?
	bool defeated;

public:
	/// Spielertyp (Mensch, KI oder geschlossen..?)
	PlayerState ps;
	
	/// Spielername
	std::string name;
	/// ehemaliger Spielername bei einem geladenen Spiel
	std::string origin_name;

	bool is_host;

	Nation nation;
	unsigned char team;
	unsigned char color;

	unsigned short ping;
	unsigned short rating;
	
	int checksum;
	bool ready;


	list<GameMessage*> nfc_queue;

};
#endif // GAMEPLAYERINFO_H_INCLUDED
