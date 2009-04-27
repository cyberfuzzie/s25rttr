// $Id: GameServer.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef GAMESERVER_H_
#define GAMESERVER_H_

#pragma once

#include "Socket.h"

#include "Singleton.h"
#include "GameProtocol.h"
#include "GlobalGameSettings.h"
#include <string>


class GameServerPlayer;
class GameMessage;
class GlobalGameSettings;
struct CreateServerInfo;

class GameServer : public Singleton<GameServer>
{
public:
	GameServer(void);
	~GameServer(void);

	/// "Versucht" den Server zu starten (muss ggf. erst um Erlaubnis beim LobbyClient fragen)
	bool TryToStart(const CreateServerInfo& csi, const std::string& map_path, const unsigned map_type);
	/// Startet den Server, muss vorher TryToStart aufgerufen werden!
	bool Start();

	void Run(void);
	void Stop(void);

	bool StartGame(void);

	bool TogglePause();

	void TogglePlayerNation(unsigned char client);
	void TogglePlayerTeam(unsigned char client);
	void TogglePlayerColor(unsigned char client);
	void TogglePlayerState(unsigned char client);
	void ChangeGlobalGameSettings(const GlobalGameSettings& ggs);

	/// Lässt einen Spieler wechseln (nur zu Debugzwecken)
	void ChangePlayer(const unsigned char old_id, const unsigned char new_id);

	/// Tauscht Spieler(positionen) bei Savegames in dskHostGame
	void SwapPlayer(const unsigned char player1, const unsigned char player2);

protected:

	void SendToAll(const GameMessage& msg);
	void KickPlayer(unsigned char playerid, unsigned char cause, unsigned short param);
	void KickPlayer(NS_PlayerKicked npk);

	void ClientWatchDog(void);
	void WaitForClients(void);
	void FillPlayerQueues(void);

	/// Sendet ein NC-Paket ohne Befehle
	void SendNothingNC(const unsigned char player);

	void HandleMessage(GameMessage *message, unsigned char client);
	void HandleGameMessage(GameMessage *message, unsigned char client);

	/// Generiert einen KI-Namen
	void SetKIName(const unsigned player_id);

private:
	void OnNMSPong(GameMessage *message, unsigned char client);

	void OnNMSServerType(GameMessage *message, unsigned char client);
	void OnNMSServerPassword(GameMessage *message, unsigned char client);
	void OnNMSServerChat(GameMessage *message, unsigned char client);

	void OnNMSPlayerName(GameMessage *message, unsigned char client);
	void OnNMSPlayerToggleNation(GameMessage *message, unsigned char client);
	void OnNMSPlayerToggleTeam(GameMessage *message, unsigned char client);
	void OnNMSPlayerToggleColor(GameMessage *message, unsigned char client);
	void OnNMSPlayerReady(GameMessage *message, unsigned char client);

	void OnNMSMapInfo(GameMessage *message, unsigned char client);
	void OnNMSMapData(GameMessage *message, unsigned char client);
	void OnNMSMapChecksum(GameMessage *message, unsigned char client);

	void OnNMSNfcCommand(GameMessage *message, unsigned char client);

private:
	enum ServerState
	{
		SS_STOPPED = 0,
		SS_CONFIG,
		SS_GAME
	} status;

	class FramesInfo
	{
	public:

		FramesInfo();
		void Clear();

		/// Aktueller nwf
		unsigned int nr;
		/// Länge der Network-Frames in ms (gf-Länge * nwf_length des Clients)
		unsigned int nwf_length;

		unsigned int lasttime;
		unsigned int lastmsgtime;
		unsigned int pausetime;
		bool pause;
	} framesinfo;

	class ServerConfig
	{
	public:

		ServerConfig();
		void Clear();

		unsigned char servertype;
		unsigned char playercount;
		std::string gamename;
		std::string password;
		std::string mapname;
		unsigned short port;
	} serverconfig;

	class MapInfo
	{
	public:

		MapInfo();
		void Clear();

		unsigned int partcount;
		unsigned int ziplength;
		unsigned int length;
		unsigned int checksum;
		std::string name;
		unsigned char *zipdata;
		unsigned map_type;
	} mapinfo;

	Socket serversocket;
	GameServerPlayer **players;
	GlobalGameSettings ggs;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define GAMESERVER GameServer::inst()

#endif
