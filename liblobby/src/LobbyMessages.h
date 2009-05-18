// $Id: LobbyMessages.h 4884 2009-05-18 16:52:52Z FloSoft $
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
#ifndef LOBBYMESSAGES_H_INCLUDED
#define LOBBYMESSAGES_H_INCLUDED

#pragma once

#include "Messages.h"

#include "LobbyMessage.h"
#include "LobbyMessageInterface.h"

#include "LobbyServerList.h"
#include "LobbyPlayerList.h"

#include "LobbyProtocol.h"

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Login-Nachricht
class LobbyMessage_Login : public LobbyMessage
{
private:
	unsigned int revision;
	std::string user;
	std::string pass;
	std::string version;

public:
	LobbyMessage_Login(void) : LobbyMessage(NMS_LOBBY_LOGIN) { }
	LobbyMessage_Login(const std::string &user, const std::string &pass, const std::string &version) : LobbyMessage(NMS_LOBBY_LOGIN)
	{
		LOG.write(">>> NMS_LOBBY_LOGIN(%d, %s, %s, %s)\n", LOBBYPROTOCOL_VERSION, user.c_str(), "********", version.c_str());

		//alloc( 4 + user.length()+1 + pass.length()+1 + version.length()+1 );

		pushUI(LOBBYPROTOCOL_VERSION);
		pushStr(user);
		pushStr(pass);
		pushStr(version);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		unsigned char rev[4];
		popUC(rev, 4);

		// haben wir eine gültige Revision erhalten?
		if(rev[0] != 0xFF || rev[3] != 0xFF)
			revision = 0;
		else
			revision = htonl(*((unsigned int*)rev));

		user = popStr();
		pass = popStr();
		version = popStr();

		LOG.write("<<< NMS_LOBBY_LOGIN(%d, %s, %s, %s)\n", revision, user.c_str(), "********", version.c_str());
		cb->OnNMSLobbyLogin(id, revision, user, pass, version);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Login-Done-Nachricht.
class LobbyMessage_Login_Done : public LobbyMessage
{
private:
	std::string email;

public:
	LobbyMessage_Login_Done(void) : LobbyMessage(NMS_LOBBY_LOGIN_DONE) { }
	LobbyMessage_Login_Done(const std::string &email) : LobbyMessage(NMS_LOBBY_LOGIN_DONE)
	{
		LOG.write(">>> NMS_LOBBY_LOGIN_DONE(%s)\n", email.c_str());

		//alloc( email.length()+1 );

		pushStr(email);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		email = popStr();

		LOG.write("<<< NMS_LOBBY_LOGIN_DONE(%s)\n", email.c_str());
		cb->OnNMSLobbyLoginDone(id, email);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Login-Error-Nachricht.
class LobbyMessage_Login_Error : public LobbyMessage
{
private:
	std::string error;

public:
	LobbyMessage_Login_Error(void) : LobbyMessage(NMS_LOBBY_LOGIN_ERROR) { }
	LobbyMessage_Login_Error(const std::string &error) : LobbyMessage(NMS_LOBBY_LOGIN_ERROR)
	{
		LOG.write(">>> NMS_LOBBY_LOGIN_ERROR(%s)\n", error.c_str());

		//alloc(error.length()+1);

		pushStr(error);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		error = popStr();
		
		LOG.write("<<< NMS_LOBBY_LOGIN_ERROR(%s)\n", error.c_str());
		cb->OnNMSLobbyLoginError(id, error);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Register-Nachricht.
class LobbyMessage_Register : public LobbyMessage
{
private:
	std::string user;
	std::string pass;
	std::string email;

public:
	LobbyMessage_Register(void) : LobbyMessage(NMS_LOBBY_REGISTER) {}
	LobbyMessage_Register(const std::string &user, const std::string &pass, const std::string &email) : LobbyMessage(NMS_LOBBY_REGISTER)
	{
		LOG.write(">>> NMS_LOBBY_REGISTER(%s, %s, %s)\n", user.c_str(), "********", email.c_str());

		//alloc(3 + (unsigned int)user.length() + pass.length() + email.length());

		pushStr(user);
		pushStr(pass);
		pushStr(email);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		user = popStr();
		pass = popStr();
		email = popStr();

		LOG.write("<<< NMS_LOBBY_REGISTER(%s, %s, %s)\n", user.c_str(), "********", email.c_str());
		cb->OnNMSLobbyRegister(id, user, pass, email);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Register-Done-Nachricht.
class LobbyMessage_Register_Done : public LobbyMessage
{
public:
	LobbyMessage_Register_Done(void) : LobbyMessage(NMS_LOBBY_REGISTER_DONE) {}
	LobbyMessage_Register_Done(bool reserved) : LobbyMessage(NMS_LOBBY_REGISTER_DONE) 
	{
		LOG.write(">>> NMS_LOBBY_REGISTER_DONE\n");
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		LOG.write("<<< NMS_LOBBY_REGISTER_DONE\n");
		cb->OnNMSLobbyRegisterDone(id);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Register-Error-Nachricht
class LobbyMessage_Register_Error : public LobbyMessage
{
private:
	std::string error;

public:
	LobbyMessage_Register_Error(void) : LobbyMessage(NMS_LOBBY_REGISTER_ERROR) {}
	LobbyMessage_Register_Error(const std::string &error) : LobbyMessage(NMS_LOBBY_LOGIN_ERROR)
	{
		LOG.write(">>> NMS_LOBBY_REGISTER_ERROR(%s)\n", error.c_str());

		//alloc( error.length()+1 );

		pushStr(error);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		error = popStr();

		LOG.write("<<< NMS_LOBBY_REGISTER_ERROR(%s)\n", error.c_str());
		cb->OnNMSLobbyRegisterError(id, error);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende ServerList-Nachricht.
class LobbyMessage_ServerList : public LobbyMessage
{
private:
	LobbyServerList list;

public:
	LobbyMessage_ServerList(void) : LobbyMessage(NMS_LOBBY_SERVERLIST) {}
	LobbyMessage_ServerList(bool reserved) : LobbyMessage(NMS_LOBBY_SERVERLIST)
	{
		LOG.write(">>> NMS_LOBBY_SERVERLIST\n");
	}
	LobbyMessage_ServerList(const LobbyServerList &list) : LobbyMessage(NMS_LOBBY_SERVERLIST)
	{
		LOG.write(">>> NMS_LOBBY_SERVERLIST(%d)\n", list.getCount());
		for(unsigned int i = 0; i < list.getCount(); ++i)
		{
			const LobbyServerInfo *server = list.getElement(i);
			LOG.write("    %d: %d %s %s:%d %s %d %s %d %d\n", i, server->getId(), server->getName().c_str(), server->getHost().c_str(), server->getPort(), server->getVersion().c_str(), server->getPing(), server->getMap().c_str(), server->getCurPlayers(), server->getMaxPlayers() );
		}

		list.serialize(this);
	}
	void run(MessageInterface *callback, unsigned int id) 
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		if(getSize() == 0) // Anfrage
		{
			LOG.write("<<< NMS_LOBBY_SERVERLIST\n");
			cb->OnNMSLobbyServerList(id);
		}
		else // Antwort
		{
			list.deserialize(this);

			LOG.write("<<< NMS_LOBBY_SERVERLIST(%d)\n", list.getCount());
			for(unsigned int i = 0; i < list.getCount(); ++i)
			{
				const LobbyServerInfo *server = list.getElement(i);
				LOG.write("    %d: %d %s %s:%d %s %d %s %d %d\n", i, server->getId(), server->getName().c_str(), server->getHost().c_str(), server->getPort(), server->getVersion().c_str(), server->getPing(), server->getMap().c_str(), server->getCurPlayers(), server->getMaxPlayers() );
			}
			cb->OnNMSLobbyServerList(id, list);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende ServerInfo-Nachricht.
class LobbyMessage_ServerInfo : public LobbyMessage
{
private:
	LobbyServerInfo info;
	unsigned int server;

public:
	LobbyMessage_ServerInfo(void) : LobbyMessage(NMS_LOBBY_SERVERINFO) { }
	LobbyMessage_ServerInfo(const unsigned int &server) : LobbyMessage(NMS_LOBBY_SERVERINFO)
	{
		LOG.write(">>> NMS_LOBBY_SERVERINFO(%d)\n", server);

		//alloc(sizeof(unsigned int));

		pushUI(server);
	}
	LobbyMessage_ServerInfo(const LobbyServerInfo &info) : LobbyMessage(NMS_LOBBY_SERVERINFO)
	{
		LOG.write(">>> NMS_LOBBY_SERVERINFO\n");
		LOG.write("    %d %s %s:%d %s %d %s %d %d\n", info.getId(), info.getName().c_str(), info.getHost().c_str(), info.getPort(), info.getVersion().c_str(), info.getPing(), info.getMap().c_str(), info.getCurPlayers(), info.getMaxPlayers() );

		info.serialize(this);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		if(getSize() == sizeof(unsigned int)) // Anfrage
		{
			server = popUI();

			LOG.write("<<< NMS_LOBBY_SERVERINFO(%d)\n", server);
			cb->OnNMSLobbyServerInfo(id, server);
		}
		else // Antwort
		{
			info.deserialize(this);

			LOG.write("<<< NMS_LOBBY_SERVERINFO\n");
			LOG.write("    %d %s %s:%d %s %d %s %d %d\n", info.getId(), info.getName().c_str(), info.getHost().c_str(), info.getPort(), info.getVersion().c_str(), info.getPing(), info.getMap().c_str(), info.getCurPlayers(), info.getMaxPlayers() );
			cb->OnNMSLobbyServerInfo(id, info);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende Rankinglist-Nachricht.
class LobbyMessage_RankingList : public LobbyMessage
{
private:
	LobbyPlayerList list;

public:
	LobbyMessage_RankingList(void) : LobbyMessage(NMS_LOBBY_RANKINGLIST) {}
	LobbyMessage_RankingList(bool reserved) : LobbyMessage(NMS_LOBBY_RANKINGLIST)
	{
		LOG.write(">>> NMS_LOBBY_RANKINGLIST\n");
	}
	LobbyMessage_RankingList(const LobbyPlayerList &list) : LobbyMessage(NMS_LOBBY_RANKINGLIST)
	{
		LOG.write(">>> NMS_LOBBY_RANKINGLIST(%d)\n", list.getCount());

		for(unsigned int i = 0; i < list.getCount(); ++i)
		{
			const LobbyPlayerInfo *player = list.getElement(i);
			LOG.write("    %d: %d %s %s %d %d %d\n", i, player->getId(), player->getName().c_str(), player->getVersion().c_str(), player->getPunkte(), player->getGewonnen(), player->getVerloren());
		}

		list.serialize(this);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		if(getSize() == 0) // Anfrage
		{
			LOG.write("<<< NMS_LOBBY_RANKINGLIST\n");
			cb->OnNMSLobbyRankingList(id);
		}
		else // Antwort
		{
			list.deserialize(this);

			LOG.write("<<< NMS_LOBBY_RANKINGLIST(%d)\n", list.getCount());
			for(unsigned int i = 0; i < list.getCount(); ++i)
			{
				const LobbyPlayerInfo *player = list.getElement(i);
				LOG.write("    %d: %d %s %s %d %d %d\n", i, player->getId(), player->getName().c_str(), player->getVersion().c_str(), player->getPunkte(), player->getGewonnen(), player->getVerloren());
			}

			cb->OnNMSLobbyRankingList(id, list);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende PlayerList-Nachricht.
class LobbyMessage_PlayerList : public LobbyMessage
{
private:
	LobbyPlayerList list;

public:
	LobbyMessage_PlayerList(void) : LobbyMessage(NMS_LOBBY_PLAYERLIST) { }
	LobbyMessage_PlayerList(bool reserved) : LobbyMessage(NMS_LOBBY_PLAYERLIST)
	{
		LOG.write(">>> NMS_LOBBY_PLAYERLIST\n");
	}
	LobbyMessage_PlayerList(const LobbyPlayerList &list) : LobbyMessage(NMS_LOBBY_PLAYERLIST)
	{
		LOG.write(">>> NMS_LOBBY_PLAYERLIST(%d)\n", list.getCount());

		for(unsigned int i = 0; i < list.getCount(); ++i)
		{
			const LobbyPlayerInfo *player = list.getElement(i);
			LOG.write("    %d: %d %s %s %d %d %d\n", i, player->getId(), player->getName().c_str(), player->getVersion().c_str(), player->getPunkte(), player->getGewonnen(), player->getVerloren());
		}

		list.serialize(this);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		if(getSize() == 0) // Anfrage
		{
			LOG.write("<<< NMS_LOBBY_PLAYERLIST\n");
			cb->OnNMSLobbyPlayerList(id);
		}
		else // Antwort
		{
			list.deserialize(this);

			LOG.write("<<< NMS_LOBBY_PLAYERLIST(%d)\n", list.getCount());
			for(unsigned int i = 0; i < list.getCount(); ++i)
			{
				const LobbyPlayerInfo *player = list.getElement(i);
				LOG.write("    %d: %d %s %s %d %d %d\n", i, player->getId(), player->getName().c_str(), player->getVersion().c_str(), player->getPunkte(), player->getGewonnen(), player->getVerloren());
			}

			cb->OnNMSLobbyPlayerList(id, list);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Id-Nachricht.
class LobbyMessage_Id : public LobbyMessage
{
private:
	unsigned int playerid;

public:
	LobbyMessage_Id(void) : LobbyMessage(NMS_LOBBY_ID) {}
	LobbyMessage_Id(const unsigned int playerid) : LobbyMessage(NMS_LOBBY_ID)
	{
		LOG.write(">>> NMS_LOBBY_ID(%d)\n", playerid);

		//alloc( sizeof(unsigned int) );

		pushUI(playerid);
	}
	void run(MessageInterface *callback, unsigned int id)
	{
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		playerid = popUI();

		LOG.write("<<< NMS_LOBBY_ID(%d)\n", playerid);

		cb->OnNMSLobbyID(id, playerid);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende Chat-Nachricht.
class LobbyMessage_Chat : public LobbyMessage
{
private:
	std::string player;
	std::string text;

public:
	LobbyMessage_Chat(void) : LobbyMessage(NMS_LOBBY_CHAT) { }
	LobbyMessage_Chat(const std::string &text) : LobbyMessage(NMS_LOBBY_CHAT)
	{
		LOG.write(">>> NMS_LOBBY_CHAT(%s)\n", text.c_str());
		
		//alloc( text.length()+1 + 1 );

		// leeren playernamen senden
		pushStr("");
		pushStr(text);
	}
	LobbyMessage_Chat(const std::string &player, const std::string &text) : LobbyMessage(NMS_LOBBY_CHAT)
	{
		LOG.write(">>> NMS_LOBBY_CHAT(%s, %s)\n", player.c_str(), text.c_str());

		//alloc( text.length()+1 + player.length()+1 );

		pushStr(player);
		pushStr(text);
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		player = popStr();
		text = popStr();

		LOG.write("<<< NMS_LOBBY_CHAT(%s, %s)\n", player.c_str(), text.c_str());

		cb->OnNMSLobbyChat(id, player, text);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Ping-Nachricht.
class LobbyMessage_Ping : public LobbyMessage
{
public:
	LobbyMessage_Ping(void) : LobbyMessage(NMS_LOBBY_PING) { }
	LobbyMessage_Ping(bool reserved) : LobbyMessage(NMS_LOBBY_PING)
	{
		//LOG.write(">>> NMS_LOBBY_PING\n");
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		//LOG.write("<<< NMS_LOBBY_PING\n");
		cb->OnNMSLobbyPing(id);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Pong-Nachricht.
class LobbyMessage_Pong : public LobbyMessage
{
public:
	LobbyMessage_Pong(void) : LobbyMessage(NMS_LOBBY_PONG) { }
	LobbyMessage_Pong(bool reserved) : LobbyMessage(NMS_LOBBY_PONG)
	{
		//LOG.write(">>> NMS_LOBBY_PONG\n");
	}
	void run(MessageInterface *callback, unsigned int id)
	{ 
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		//LOG.write("<<< NMS_LOBBY_PONG\n");
		cb->OnNMSLobbyPong(id);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende Server-Add-Nachricht.
class LobbyMessage_Server_Add : public LobbyMessage
{
private:
	LobbyServerInfo info;

public:
	LobbyMessage_Server_Add(void) : LobbyMessage(NMS_LOBBY_SERVER_ADD) { }
	LobbyMessage_Server_Add(const LobbyServerInfo &info) : LobbyMessage(NMS_LOBBY_SERVER_ADD)
	{
		LOG.write(">>> NMS_LOBBY_SERVER_ADD\n");
		LOG.write("    %d %s %s:%d %s %d %s %d %d\n", info.getId(), info.getName().c_str(), info.getHost().c_str(), info.getPort(), info.getVersion().c_str(), info.getPing(), info.getMap().c_str(), info.getCurPlayers(), info.getMaxPlayers() );

		// Daten serialisieren
		info.serialize(this);
	}
	void run(MessageInterface *callback, unsigned int id)
	{
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		info.deserialize(this);

		LOG.write("<<< NMS_LOBBY_SERVER_ADD\n");
		LOG.write("    %d %s %s:%d %s %d %s %d %d\n", info.getId(), info.getName().c_str(), info.getHost().c_str(), info.getPort(), info.getVersion().c_str(), info.getPing(), info.getMap().c_str(), info.getCurPlayers(), info.getMaxPlayers() );

		cb->OnNMSLobbyServerAdd(id, info);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Server-Add-Failed.
class LobbyMessage_Server_Add_Failed : public LobbyMessage
{
private:
	std::string error;

public:
	LobbyMessage_Server_Add_Failed(void) : LobbyMessage(NMS_LOBBY_SERVER_ADD_FAILED) { }
	LobbyMessage_Server_Add_Failed(const std::string &error) : LobbyMessage(NMS_LOBBY_SERVER_ADD_FAILED)
	{
		LOG.write(">>> NMS_LOBBY_SERVER_ADD_FAILED(%s)\n", error.c_str());

		//alloc( error.length()+1 );

		pushStr(error);
	}
	void run(MessageInterface *callback, unsigned int id)
	{
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		error = popStr();

		LOG.write("<<< NMS_LOBBY_SERVER_ADD_FAILED(%s)\n", error.c_str());
		cb->OnNMSLobbyServerAddFailed(id, error);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Delete-Nachricht.
class LobbyMessage_Server_Delete : public LobbyMessage
{
public:
	LobbyMessage_Server_Delete(void) : LobbyMessage(NMS_LOBBY_SERVER_DELETE) { }
	LobbyMessage_Server_Delete(bool reserved) : LobbyMessage(NMS_LOBBY_SERVER_DELETE) 
	{
		LOG.write(">>> NMS_LOBBY_SERVER_DELETE\n");
	}
	void run(MessageInterface *callback, unsigned int id)
	{
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		LOG.write("<<< NMS_LOBBY_SERVER_DELETE\n");
		cb->OnNMSLobbyServerDelete(id);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Update-Player-Nachricht.
class LobbyMessage_Server_Update_Player : public LobbyMessage
{
private:
	unsigned int curplayer;
	unsigned int maxplayer;

public:
	LobbyMessage_Server_Update_Player(void) : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_PLAYER) { }
	LobbyMessage_Server_Update_Player(const unsigned int curplayer, const unsigned int maxplayer) : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_PLAYER)
	{
		LOG.write(">>> NMS_LOBBY_SERVER_UPDATE_PLAYER(%d,%d)\n", curplayer, maxplayer);

		//alloc(sizeof(unsigned int)*2);

		pushUI(curplayer);
		pushUI(maxplayer);
	}
	void run(MessageInterface *callback, unsigned int id)
	{
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);
		
		curplayer = popUI();
		maxplayer = popUI();

		LOG.write("<<< NMS_LOBBY_SERVER_UPDATE_PLAYER(%d,%d)\n", curplayer, maxplayer);
		cb->OnNMSLobbyServerUpdatePlayer(id, curplayer, maxplayer);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Update-Map-Nachricht.
class LobbyMessage_Server_Update_Map : public LobbyMessage
{
private:
	std::string map;

public:
	LobbyMessage_Server_Update_Map(void) : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_MAP) { }
	LobbyMessage_Server_Update_Map(const std::string &map) : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_MAP)
	{
		LOG.write(">>> NMS_LOBBY_SERVER_UPDATE_MAP(%s)\n", map.c_str());

		//alloc( map.length()+1 );

		pushStr(map);
	}
	void run(MessageInterface *callback, unsigned int id)
	{
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		map = popStr();

		LOG.write("<<< NMS_LOBBY_SERVER_UPDATE_MAP(%s)\n", map.c_str());
		cb->OnNMSLobbyServerUpdateMap(id, map);
	}
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Join-Nachricht.
class LobbyMessage_Server_Join : public LobbyMessage
{
public:
	LobbyMessage_Server_Join(void) : LobbyMessage(NMS_LOBBY_SERVER_JOIN) { }
	LobbyMessage_Server_Join(bool reserved) : LobbyMessage(NMS_LOBBY_SERVER_JOIN)
	{
		LOG.write(">>> NMS_LOBBY_SERVER_JOIN\n");
	}
	void run(MessageInterface *callback, unsigned int id)
	{
		LobbyMessageInterface *cb = dynamic_cast<LobbyMessageInterface*>(callback);

		LOG.write("<<< NMS_LOBBY_SERVER_JOIN\n");
		cb->OnNMSLobbyServerJoin(id);
	}
};

#endif //!LOBBYMESSAGES_H_INCLUDED
