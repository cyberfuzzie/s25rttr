// $Id: GameMessage.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef GAMEMESSAGE_H_INCLUDED
#define GAMEMESSAGE_H_INCLUDED

#pragma once

class Socket;

class GameMessage
{
public:
	/// Konstruktor von @p GameMessage.
	GameMessage(void);
	/// Konstruktor von @p GameMessage.
	GameMessage(unsigned short id);
	/// Konstruktor von @p GameMessage.
	GameMessage(unsigned short id, unsigned int length);
	/// Kopierkonstruktor von @p GameMessage.
	GameMessage(const GameMessage &msg); // Copykonstruktor
	/// Destruktor von @p GameMessage.
	~GameMessage(void);

public:
	void clear();
	void alloc(void);
	void alloc(const void *data);
	void *alloc(unsigned int length);

	bool send(Socket *sock);
	int recv(Socket *sock, bool wait = false);

public:
	unsigned short m_usID;
	unsigned int m_uiLength;
	void *m_pData;
};

#endif // GAMEMESSAGE_H_INCLUDED
