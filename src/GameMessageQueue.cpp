// $Id: GameMessageQueue.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "GameMessageQueue.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/// Löscht alle Nachrichten
void GameMessageQueue::clear()
{
	queue = std::queue<GameMessage*>();
}

/// Fügt neue leere Nachricht hinzu
GameMessage *GameMessageQueue::push()
{
	GameMessage * gm = new GameMessage;
	queue.push(gm);
	return gm;
}

/// Fügt ansonsten leere Nachricht mit einer ID hinzu
GameMessage *GameMessageQueue::push(unsigned short usID)
{
	GameMessage * gm = new GameMessage(usID);
	queue.push(gm);
	return gm;
}

/// Fügt fertige Nachricht hinzu
GameMessage *GameMessageQueue::push(const GameMessage& message)
{
	// Kopie erst erstellen!
	GameMessage * gm = new GameMessage(message);
	queue.push(gm);
	return gm;
}

/// Sendet alle Nachrichten und löscht sie
void GameMessageQueue::flush(Socket *sock)
{
	while(count() > 0)
	{
		GameMessage *m = front();
		if(m->m_usID > 0)
			m->send(sock);
		pop();
	}
	clear();
}

/// Empfängt eine Nachricht von sock und hängt sie in die queue mit ein
int GameMessageQueue::recv(Socket *sock, bool wait)
{
	if(!sock)
		return false;

	GameMessage *msg = push();

	int recvok = msg->recv(sock, wait);
	if(recvok != 0)
		pop();

	return recvok;
}
