// $Id: GameServerPlayer.cpp 4933 2009-05-24 12:29:23Z OLiver $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "GameServerPlayer.h"
#include "GameMessage.h"
#include "GameMessages.h"
#include "VideoDriverWrapper.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/// Konstruktor
GameServerPlayer::GameServerPlayer(const unsigned playerid)
: GamePlayerInfo(playerid),
connecttime(0),
last_command_timeout(0),
pinging(false),
lastping(0), 
temp_ul(0),
temp_ui(0),
send_queue(&GameMessage::create_game),
recv_queue(&GameMessage::create_game)
{
}

GameServerPlayer::GameServerPlayer(const unsigned playerid, Serializer * ser)
	: GamePlayerInfo(playerid,ser),
	connecttime(0),
	last_command_timeout(0),
	pinging(false),
	lastping(0), 
	temp_ul(0),
	temp_ui(0),
	send_queue(&GameMessage::create_game),
	recv_queue(&GameMessage::create_game)
{
}

///////////////////////////////////////////////////////////////////////////////
/// Destruktor
GameServerPlayer::~GameServerPlayer()
{
	so.Close();
}

///////////////////////////////////////////////////////////////////////////////
/// pingt ggf den Spieler
void GameServerPlayer::doPing()
{
	if( (ps == PS_OCCUPIED) && (pinging == false) && ( ( VideoDriverWrapper::inst().GetTickCount() - lastping ) > 1000 ) )
	{
		pinging = true;
		
		lastping = VideoDriverWrapper::inst().GetTickCount();

		// Ping Nachricht senden
		send_queue.push(new GameMessage_Ping(0xFF));
	}
}

///////////////////////////////////////////////////////////////////////////////
/// prüft auf Ping-Timeout beim verbinden
void GameServerPlayer::doTimeout()
{
	if( (ps == PS_RESERVED) && ( ( VideoDriverWrapper::inst().GetTickCount() - connecttime ) > PING_TIMEOUT ) )
	{
		puts("aaahh");
		LOG.lprintf("SERVER: Reserved slot %d freed due ping timeout\n", playerid);

		/*// Todesnachricht absetzen
		Message_Dead dm();
		dm.send(&so);*/

		// und aufräumen
		clear();
	}
}

/** /////////////////////////////////////////////////////////////////////////////
// setzt den Player auf "reserviert"
// @param sock Socket
// @param id Spieler-ID                                                        */
void GameServerPlayer::reserve(Socket *sock, unsigned char id)
{
	clear();
	
	playerid = id;

	connecttime = VideoDriverWrapper::inst().GetTickCount();

	so = *sock;
	
	ps = PS_RESERVED;
}

void GameServerPlayer::clear()
{
	GamePlayerInfo::clear();

	connecttime = 0;
	last_command_timeout = 0;
	pinging = false;
	send_queue.clear();
	recv_queue.clear();
	lastping = 0;
	temp_ul = 0;
	temp_ui = 0;
	so.Close();
	
}


unsigned GameServerPlayer::GetTimeOut() const
{
	// Nach 34 Sekunden kicken (34 damit ab 30 erst die Meldung kommt, sonst kommt sie andauernd)
	return unsigned(34-(TIME.CurrentTime()-last_command_timeout)/1000);
}

/// Tauscht Spieler
void GameServerPlayer::SwapPlayer(GameServerPlayer& two)
{
	GamePlayerInfo::SwapPlayer(two);

	Swap(this->connecttime,two.connecttime);
	Swap(this->last_command_timeout,two.last_command_timeout);

	Swap(this->so,two.so);
	Swap(this->pinging,two.pinging);

	Swap(this->send_queue,two.send_queue);
	Swap(this->recv_queue,two.recv_queue);

	Swap(this->lastping,two.lastping);

	Swap(this->temp_ul,two.temp_ul);
	Swap(this->temp_ui,two.temp_ui);
}

/// Spieler laggt
void GameServerPlayer::Lagging()
{
	// Laggt neu?
	if(!last_command_timeout)
		// Anfangs des Laggens merken
		last_command_timeout = TIME.CurrentTime();
}

/// Spieler laggt nicht (mehr)
void GameServerPlayer::NotLagging()
{
	/// Laggt nicht mehr
	last_command_timeout = 0;
}

