// $Id: Socket.h 5606 2009-10-07 14:57:50Z FloSoft $
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
#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#pragma once

#ifdef _WIN32
	#include <winsock.h>
	typedef int socklen_t;
#else
	#include <arpa/inet.h>

	#define SOCKET int
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define HINSTANCE void*

	#define closesocket close
#endif // !_WIN32

///Socket-Wrapper-Klasse für portable TCP/IP-Verbindungen
class Socket
{
private:
	enum STATUS
	{
		INVALID = INVALID_SOCKET,
		VALID = 0,
		LISTEN,
		CONNECT
	} status;

public:
	/// Standardkonstruktor von @p Socket.
	Socket(void);

	/// Konstruktor von @p Socket.
	Socket(const SOCKET so, STATUS st);

	/// Setzt ein Socket auf übergebene Werte.
	void Set(const SOCKET so, STATUS st);

	/// Initialisiert die Socket-Bibliothek.
	static bool Initialize(void);

	/// räumt die Socket-Bibliothek auf.
	static void Shutdown(void);

	/// erstellt und initialisiert das Socket.
	bool Create(int family = AF_INET);

	/// schliesst das Socket.
	void Close(void);

	/// setzt das Socket auf Listen.
	bool Listen(unsigned short port, bool use_ipv6);

	/// akzeptiert eingehende Verbindungsversuche.
	bool Accept(Socket& client);

	/// versucht eine Verbindung mit einem externen Host aufzubauen.
	bool Connect(const char *const hostname, const unsigned short port, bool use_ipv6);

	/// liest Daten vom Socket in einen Puffer.
	int Recv(void *buffer, int length, bool block = true);

	/// schreibt Daten von einem Puffer auf das Socket.
	int Send(const void *buffer, const int length);

	/// setzt eine Socketoption.
	bool SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = IPPROTO_TCP);

	/// Zuweisungsoperator.
	Socket &operator =(const Socket &sock);

	/// Zuweisungsoperator.
	Socket &operator =(const SOCKET &sock);

	/// Größer-Vergleichsoperator.
	bool operator >(const Socket &sock);

	/// prüft auf wartende Bytes.
	int BytesWaiting(void);

	/// prüft auf wartende Bytes.
	int BytesWaiting(unsigned int *dwReceived);

	/// liefert die IP des Remote-Hosts.
	std::string GetPeerIP(void);

	/// liefert die IP des Lokalen-Hosts.
	std::string GetSockIP(void);
	
	/// liefert einen Zeiger auf das Socket.
	SOCKET* GetSocket(void);

	/// liefert einen string der übergebenen Ip.
	std::string &IpToString(const sockaddr *addr, std::string &buffer);

	/// liefert den Status des Sockets.
	bool isValid(void) { return (status != INVALID); }

private:
	SOCKET	sock; ///< Unser Socket
};

#endif // SOCKET_H_INCLUDED
