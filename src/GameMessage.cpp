// $Id: GameMessage.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "GameMessage.h"

#include "VideoDriverWrapper.h"
#include "Socket.h"
#include "SocketSet.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p GameMessage.
 *
 *  @author FloSoft
 */
GameMessage::GameMessage(void)
	: m_usID(0), m_uiLength(0), m_pData(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p GameMessage.
 *  Setzt die ID.
 *
 *  @param[in] id ID der Nachricht
 *
 *  @author FloSoft
 */
GameMessage::GameMessage(unsigned short id)
	: m_usID(id), m_uiLength(0), m_pData(NULL)
{ 
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p GameMessage.
 *  Setzt die ID und Länge der Nachricht.
 *
 *  @param[in] id     ID der Nachricht
 *  @param[in] length Länge der Nachricht.
 *
 *  @author FloSoft
 */
GameMessage::GameMessage(unsigned short id, unsigned int length)
	: m_usID(id), m_uiLength(length), m_pData(NULL)
{
	alloc();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p GameMessage.
 *
 *  @param[in] msg Die andere Nachricht
 *
 *  @author OLiver
 */
GameMessage::GameMessage(const GameMessage &msg)
	: m_usID(msg.m_usID), m_uiLength(msg.m_uiLength), m_pData(NULL)
{
	alloc(msg.m_pData);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p GameMessage.
 *
 *  @author FloSoft
 */
GameMessage::~GameMessage(void) 
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void GameMessage::clear(void)
{
	delete[] ((char*)m_pData);

	m_usID      = 0;
	m_uiLength  = 0;
	m_pData     = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void GameMessage::alloc(void)
{
	delete[] ((char*)m_pData);

	m_pData = new char[m_uiLength];
	memset(m_pData, 0, sizeof(char)*m_uiLength);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void GameMessage::alloc(const void *data)
{
	delete[] ((char*)m_pData);

	m_pData = new char[m_uiLength];
	memcpy(m_pData, data, sizeof(char)*m_uiLength);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Erzeugt Speicher mit der Länge @p length
 *
 *  @param[in] length Länge des gewünschten Nachrichtenblocks.
 *
 *  @return gibt Adresse des Speicherblocks ( @p m_pData ) zurück
 *
 *  @author FloSoft
 */
void *GameMessage::alloc(unsigned int length)
{
	m_uiLength = length;
	alloc();

	return m_pData;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
bool GameMessage::send(Socket *sock)
{
	static char buffer[1024001];
		
	if(NULL == sock)
		return false;

	// WTF? so große Nachricht darfs nicht geben
	if(m_uiLength > 1024000 - 6)
	{
		LOG.lprintf("BIG OOPS! Message with length %u exceeds maximum of %d!\n", m_uiLength, 1024000 - 6);
		return false;
	}

	unsigned short *id = (unsigned short*)&buffer[0];
	unsigned int *length = (unsigned int*)&buffer[2];
	unsigned char *data = (unsigned char*)&buffer[6];

	*id = m_usID;
	*length = m_uiLength;
	memcpy(data, m_pData, m_uiLength);

	if(6+m_uiLength != (unsigned int)sock->Send(buffer, 6+m_uiLength))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
int GameMessage::recv(Socket *sock, bool wait)
{
	if(NULL == sock)
		return -1;

	int time = VideoDriverWrapper::inst().GetTickCount();

	int timeout = time;
	unsigned int received;
	SocketSet set;
 
	while(true)
	{
		// Warten wir schon 5s auf Antwort?
		if(time - timeout > 5000)
			wait = false;

		time = VideoDriverWrapper::inst().GetTickCount();

		// SocketSet "saubermachen"
		set.Clear();

		// Socket hinzufgen
		set.Add(*sock);

		// liegen daten an?
		int retval = set.Select(0, 0);

		if(retval <= 0)
		{
			if(wait)
				continue;
			if(retval == -1)
				return -1;
			return 2;
		}

		// liegen diese Daten an unserem Socket?
		if(!set.InSet(*sock))
		{
			if(wait)
				continue;
			return 2;
		}

		// wieviele Bytes liegen an?
		if(sock->BytesWaiting(&received) != 0)
		{
			if(wait)
				continue;
			return 2;
		}

		// socket ist geschlossen worden
		if(received == 0)
			return -1;

		// haben wir schon eine vollständige nachricht? (kleinste nachricht: 6 bytes)
		if(received < 6)
		{
			if(wait)
				continue;
			return 2;
		}
		break;
	}

	int read = -1;

	char block[6];
	unsigned short *id = (unsigned short*)&block[0];
	unsigned int *length = (unsigned int*)&block[2];

	// block empfangen
	read = sock->Recv(block, 6, false);
	if(read != 6)
	{
		LOG.lprintf("recv: block: only got %d bytes instead of %d, waiting for next try\n", read, 6);
		if(read == -1)
			return -1;
		return 2;
	}
	
	read = sock->BytesWaiting();

	if(read < (signed)((*length) + 6) )
	{
		LOG.lprintf("recv: block-waiting: not enough input (%d/%d) for message (0x%04X), waiting for next try\n", read, (*length)+6, *id);
		if(read == -1)
			return -1;
		return 2;
	}

	// ID empfangen
	read = sock->Recv(&m_usID, sizeof(unsigned short));
	if(sizeof(unsigned short) != read )
	{
		LOG.lprintf("recv: id: only got %d bytes instead of %d\n", read, 2);
		return -1;
	}

	// Length empfangen
	read = sock->Recv(&m_uiLength, sizeof(unsigned int));
	if(sizeof(unsigned int) != read )
	{
		LOG.lprintf("recv: length: only got %d bytes instead of %d\n", read, 2);
		return -1;
	}

	if(m_uiLength > 0)
	{
		// Daten empfangen
		alloc();

		read = sock->Recv(m_pData, m_uiLength);
		if(m_uiLength != (unsigned int)read )
		{
			clear();

			LOG.lprintf("recv: data: only got %d bytes instead of %d\n", read, m_uiLength);

			return -1;
		}
	}

	return 0;
}
