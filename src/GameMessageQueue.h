// $Id: GameMessageQueue.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef GAMEMESSAGEQUEUE_H_INCLUDED
#define GAMEMESSAGEQUEUE_H_INCLUDED

#pragma once

#include "GameMessage.h"
#include <queue>

class Socket;

class GameMessageQueue
{
public:

	/// Löscht alle Nachrichten
	void clear();
	/// Entfernt das zuletzt gepushte element
	void pop() { queue.pop(); }
	/// Sendet alle Nachrichten und löscht sie
	void flush(Socket *sock);
	/// Gibt Anzahl der Nachrichten zurück
	unsigned int count() { return static_cast<unsigned>(queue.size()); }
	/// Empfängt eine Nachricht von sock und hängt sie in die queue mit ein
	int recv(Socket *sock, bool wait = false);

	/// Fügt neue leere Nachricht hinzu
	GameMessage *push(void);
	/// Fügt ansonsten leere Nachricht mit einer ID hinzu
	GameMessage *push(unsigned short usID);
	/// Fügt fertige Nachricht hinzu
	GameMessage *push(const GameMessage& message);

	/// Gibt erste Nachricht zurück (die als LETZTES eingefügt wurde)
	GameMessage *front(void) { return queue.front(); }

private:
	std::queue<GameMessage*> queue;
};

#endif // GAMEMESSAGEQUEUE_H_INCLUDED
