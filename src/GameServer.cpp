// $Id: GameServer.cpp 7091 2011-03-27 10:57:38Z OLiver $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include <stdafx.h>
#include "main.h"
#include "GameServer.h"

#include "SocketSet.h"

#include "Loader.h"
#include "Random.h"
#include "VideoDriverWrapper.h"
#include "GameMessage.h"
#include "GameMessages.h"
#include "GameClient.h"

#include "FileChecksum.h"
#include "GlobalGameSettings.h"
#include "LobbyClient.h"
#include "iwDirectIPCreate.h"

#include "GameServerPlayer.h"

#include "GameSavegame.h"
#include "GameReplay.h"
#include "AIPlayer.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



GameServer::FramesInfo::FramesInfo()
{
	Clear();
}

void GameServer::FramesInfo::Clear()
{
	nr = 0;
	nwf_length = 0;
	gf_nr = 0;
	gf_length = 0;
	lasttime = 0;
	lastmsgtime = 0;
	pausetime = 0;
	pause = false;
}
GameServer::ServerConfig::ServerConfig()
{
	Clear();
}

void GameServer::ServerConfig::Clear()
{
	servertype = 0;
	playercount = 0;
	gamename = "";
	password = "";
	mapname = "";
	port = 0;
}
GameServer::MapInfo::MapInfo()
{
	Clear();
}

void GameServer::MapInfo::Clear()
{
	partcount = 0;
	ziplength = 0;
	length = 0;
	checksum = 0;
	name ="";
	delete [] zipdata;
	zipdata = 0;
	map_type = MAPTYPE_OLDMAP;
}

GameServer::CountDown::CountDown()
{
	Clear();
}

void GameServer::CountDown::Clear()
{
	do_countdown = false;
	countdown = 2;
	lasttime = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
GameServer::GameServer(void)
{
	status = SS_STOPPED;

	framesinfo.Clear();
	serverconfig.Clear();
	mapinfo.Clear();
	countdown.Clear();
}

///////////////////////////////////////////////////////////////////////////////
//
GameServer::~GameServer(void)
{
	Stop();
}

///////////////////////////////////////////////////////////////////////////////
// Spiel hosten
bool GameServer::TryToStart(const CreateServerInfo& csi, const std::string& map_path, const MapType map_type)
{
	Stop();

	// Name, Password und Kartenname kopieren
	serverconfig.gamename = csi.gamename;
	serverconfig.password = csi.password;
	serverconfig.mapname = map_path;
	serverconfig.servertype = csi.type;
	serverconfig.port = csi.port;
	serverconfig.ipv6 = csi.ipv6;
	mapinfo.map_type = map_type;

	// Titel der Karte (nicht der Dateiname!)
	std::string map_title;

	// Maps, Random-Maps, Savegames - Header laden und relevante Informationen rausschreiben (Map-Titel, Spieleranzahl)
	switch(mapinfo.map_type)
	{
	default: LOG.lprintf("GameServer::Start: ERROR: Map-Type %u not supported!\n",mapinfo.map_type); return false;
	// Altes S2-Mapformat von BB
	case MAPTYPE_OLDMAP:
		{
			libsiedler2::ArchivInfo map;

			// Karteninformationen laden
			if(libsiedler2::loader::LoadMAP(serverconfig.mapname.c_str(), &map, true) != 0)
			{
				LOG.lprintf("GameServer::Start: ERROR: Map \"%s\", couldn't load header!\n",serverconfig.mapname.c_str()); 
				return false;
			}
			const libsiedler2::ArchivItem_Map_Header *header = &(dynamic_cast<const glArchivItem_Map *>(map.get(0))->getHeader()); 
			assert(header);

			serverconfig.playercount = header->getPlayer();
			map_title = header->getName();
		} break;
	// Gespeichertes Spiel
	case MAPTYPE_SAVEGAME:
		{
			Savegame save;

			if(!save.Load(serverconfig.mapname.c_str(),false,false))
				return false;

			// Spieleranzahl
			serverconfig.playercount = save.player_count;


		} break;
	}

	// Von Lobby abhängig? Dann der Bescheid sagen und auf eine Antwort warten, dass wir den Server
	// erstellen dürfen
	if(serverconfig.servertype == NP_LOBBY)
	{
		LOBBYCLIENT.AddServer(serverconfig.gamename.c_str(), map_title.c_str(), (serverconfig.password.length() != 0), serverconfig.port);
		return true;
	}
	else
		// ansonsten können wir sofort starten
		return Start();
}

bool GameServer::Start()
{
	// map-shortname füllen
	size_t pos = serverconfig.mapname.find_last_of('/');
	if(pos == std::string::npos)
		mapinfo.name =  serverconfig.mapname;
	else
		mapinfo.name = serverconfig.mapname.substr(pos+1);

	// mapinfo einlesen
	FILE *map_f = fopen(serverconfig.mapname.c_str(), "rb");

	// größe der map
	fseek(map_f, 0, SEEK_END);
	mapinfo.length = ftell(map_f);
	fseek(map_f, 0, SEEK_SET);

	char *map_data = new char[mapinfo.length+1];
	mapinfo.zipdata = new unsigned char[mapinfo.length*2 + 600]; // + 1prozent + 600 ;)

	mapinfo.ziplength = mapinfo.length*2 + 600;

	bool read_succeeded = ((unsigned int)libendian::le_read_c(map_data, mapinfo.length, map_f) == mapinfo.length);
	fclose(map_f);
	
	if(!read_succeeded)
		return false;

	// map mit bzip2 komprimieren
	int err = BZ_OK;
	if( (err = BZ2_bzBuffToBuffCompress( (char*)mapinfo.zipdata, (unsigned int*)&mapinfo.ziplength, map_data, mapinfo.length, 9, 0, 250)) != BZ_OK)
	{
		LOG.lprintf("FATAL ERROR: BZ2_bzBuffToBuffCompress failed with error: %d\n", err);
		return false;
	}

	mapinfo.checksum = CalcChecksumOfBuffer((unsigned char*)map_data, mapinfo.length);

	delete[] map_data;

	mapinfo.partcount = mapinfo.ziplength / MAP_PART_SIZE +  ( (mapinfo.ziplength%MAP_PART_SIZE) ? 1 : 0);

	// Speicher für Spieler anlegen
	for(unsigned i =0;i<serverconfig.playercount;++i)
		players.push_back(GameServerPlayer(i));

	// Potentielle KI-Player anlegen
	ai_players.resize(serverconfig.playercount);

	bool host_found = false;

	//// Spieler 0 erstmal der Host
	switch(mapinfo.map_type)
	{
	default:
		break;

	case MAPTYPE_OLDMAP:
		{
			// Host bei normalen Spieler der erste Spieler
			players[0].is_host = true;

			// Standardeinstellungen aus den SETTINGS für die Addons laden
			ADDONMANAGER.LoadSettings();
		} break;
	case MAPTYPE_SAVEGAME:
		{
			Savegame save;
			if(!save.Load(serverconfig.mapname.c_str(),true,false))
				return false;

			// Bei Savegames die Originalspieldaten noch mit auslesen
			for(unsigned char i = 0;i<serverconfig.playercount;++i)
			{
				// PlayerState
				players[i].ps = PlayerState(save.players[i].ps);

				if(players[i].ps != PS_LOCKED)
				{
					// (ehemaliger) Spielername
					players[i].origin_name = save.players[i].name;

					// Volk, Team und Farbe
					players[i].nation = save.players[i].nation;
					players[i].color = save.players[i].color;
					players[i].team = Team(save.players[i].team);
					
				}

				// Besetzt --> freigeben, damit auch jemand reinkann
				if(players[i].ps == PS_OCCUPIED)
				{
					players[i].ps = PS_FREE;
					// Erster richtiger Spieler? Dann ist das der Host später
					if(!host_found)
					{
						players[i].is_host = true;
						host_found = true;
					}
				}
				// KI-Spieler? Namen erzeugen
				else if(players[i].ps == PS_KI)
					SetAIName(i);
			}

			// Einstellungen aus dem Savegame für die Addons werden in Load geladen

			// Und die GGS
			ggs = save.ggs;
		} break;
	}


	// ab in die Konfiguration
	status = SS_CONFIG;

	// und das socket in listen-modus schicken
	if(!serversocket.Listen(serverconfig.port, serverconfig.ipv6))
	{
		LOG.lprintf("GameServer::Start: ERROR: Listening on port %d failed!\n", serverconfig.port);
		LOG.getlasterror("Fehler");
		return false;
	}

	// Zu sich selbst connecten als Host
	GAMECLIENT.Connect("localhost", serverconfig.password, serverconfig.servertype, serverconfig.port, true, serverconfig.ipv6);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Hauptschleife
void GameServer::Run(void)
{
	if(status == SS_STOPPED)
		return;

	// auf tote Clients prüfen
	ClientWatchDog();

	// auf neue Clients warten
	if(status == SS_CONFIG)
		WaitForClients();

	// post zustellen
	FillPlayerQueues();

	if(countdown.do_countdown)
	{
		// countdown erzeugen
		if(VideoDriverWrapper::inst().GetTickCount() - countdown.lasttime > 1000)
		{
			// nun echt starten
			if(countdown.countdown < 0)
			{
				countdown.Clear();
				StartGame();
			}
			else
			{
				SendToAll(GameMessage_Server_Countdown(countdown.countdown));
				LOG.write("SERVER >>> BROADCAST: NMS_SERVER_COUNTDOWN(%d)\n", countdown.countdown);
				
				countdown.lasttime = VideoDriverWrapper::inst().GetTickCount();

				--countdown.countdown;
			}
		}
	}

	// queues abarbeiten
	for(unsigned int client = 0; client < serverconfig.playercount; ++client)
	{
		GameServerPlayer *player = &players[client];
		//unsigned char count = 0;

		// maximal 10 Pakete verschicken
		player->send_queue.send(&player->so, 10);

		// recv-queue abarbeiten
		while(player->recv_queue.count() > 0)
		{
			player->recv_queue.front()->run(this, client);
			player->recv_queue.pop();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// stoppt den server
void GameServer::Stop(void)
{
	// player verabschieden
	players.clear();

	// aufräumen
	framesinfo.Clear();
	serverconfig.Clear();
	mapinfo.Clear();
	countdown.Clear();

	// KI-Player zerstören
	for(unsigned i = 0;i<ai_players.size();++i)
		delete ai_players[i];
	ai_players.clear();

	// laden dicht machen
	serversocket.Close();

	if(status != SS_STOPPED)
		LOG.lprintf("server state changed to stop\n");

	// status
	status = SS_STOPPED;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  startet den Spielstart-Countdown
 *
 *  @author FloSoft
 */
bool GameServer::StartCountdown()
{
	GameServerPlayer *player = NULL;
	unsigned char client = 0xFF;

	// Alle Spieler da?
	for(client = 0; client < serverconfig.playercount; ++client)
	{
		player = &players[client];

		// noch nicht alle spieler da -> feierabend!
		if( (player->ps == PS_FREE) || (player->ps == PS_RESERVED) )
			return false;
		else if(player->ps != PS_LOCKED && player->ps != PS_KI && !player->ready)
			return false;
	}

	bool reserved_colors[PLAYER_COLORS_COUNT];
	memset(reserved_colors, 0, sizeof(bool) * PLAYER_COLORS_COUNT);

	// Alle ne andere Farbe?
	for(client = 0; client < serverconfig.playercount; ++client)
	{
		player = &players[client];

		if( (player->ps == PS_OCCUPIED) || (player->ps == PS_KI) )
		{
			// farbe schon belegt -> und tschüss
			if(reserved_colors[player->color])
				return false;

			reserved_colors[player->color] = true;
		}
	}

	// Countdown starten
	countdown.Clear();
	countdown.do_countdown = true;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  stoppt den Spielstart-Countdown
 *
 *  @author FloSoft
 */
void GameServer::CancelCountdown()
{
	// Countdown-Stop allen mitteilen
	countdown.Clear();
	SendToAll(GameMessage_Server_CancelCountdown(true));
	LOG.write("SERVER >>> BROADCAST: NMS_SERVER_CANCELCOUNTDOWN\n");
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  startet das Spiel.
 *
 *  @author OLiver
 *  @author FloSoft
 */
bool GameServer::StartGame()
{
	// Bei Savegames wird der Startwert von den Clients aus der Datei gelesen!
	unsigned random_init = (mapinfo.map_type==MAPTYPE_SAVEGAME)? 0xFFFFFFFF : VideoDriverWrapper::inst().GetTickCount();

	// Höchsten Ping ermitteln
	unsigned highest_ping = 0;
	GameServerPlayer *player = NULL;
	unsigned char client = 0xFF;

	for(client = 0; client < serverconfig.playercount; ++client)
	{
		player = &players[client];

		if(player->ps == PS_OCCUPIED)
		{
			if(player->ping > highest_ping)
				highest_ping = player->ping;
		}
	}

	framesinfo.gf_length = SPEED_GF_LENGTHS[ggs.game_speed];

	// NetworkFrame-Länge bestimmen, je schlechter (also höher) die Pings, desto länger auch die Framelänge
	unsigned i = 1;
	for( ; i < 20; ++i)
	{
		if(i * framesinfo.gf_length > highest_ping + 200)
			break;
	}

	framesinfo.nwf_length = i;

	// Mond malen
	LOADER.GetImageN("resource", 33)->Draw(VideoDriverWrapper::inst().GetMouseX(), VideoDriverWrapper::inst().GetMouseY() - 40, 0, 0, 0, 0, 0, 0);
	VideoDriverWrapper::inst().SwapBuffers();

	GameMessage_Server_Start start_msg(random_init, framesinfo.nwf_length);

	LOG.lprintf("SERVER: Using gameframe length of %dms\n", framesinfo.gf_length);
	LOG.lprintf("SERVER: Using networkframe length of %ums\n", framesinfo.nwf_length);

	// Spielstart allen mitteilen
	SendToAll(start_msg);
	LOG.write("SERVER >>> BROADCAST: NMS_SERVER_START(%d)\n", random_init);

	// ab ins game wechseln
	status = SS_GAME;

	framesinfo.lasttime = VideoDriverWrapper::inst().GetTickCount();

	// Spielstart allen mitteilen
	SendToAll(GameMessage_Server_NWFDone(0xff));

	// GameClient soll erstmal starten, damit wir von ihm die benötigten Daten für die KIs bekommen
	GAMECLIENT.StartGame(random_init);

	// Erste KI-Nachrichten schicken
	for(unsigned i = 0;i < this->serverconfig.playercount; ++i)
	{
		if(players[i].ps == PS_KI)
		{
			SendNothingNC(i);
			ai_players[i] = GAMECLIENT.CreateAIPlayer(i);
		}
	}

	LOG.write("SERVER >>> BROADCAST: NMS_NFC_DONE\n");

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// wechselt Spielerstatus durch
void GameServer::TogglePlayerState(unsigned char client)
{
	GameServerPlayer *player = &players[client];

	// oh ein spieler, weg mit ihm!
	if(player->ps == PS_OCCUPIED)
	{
		KickPlayer(client, NP_NOCAUSE, 0);

		return;
	}

	// playerstatus weiterwechseln
	switch(player->ps)
	{
	default: break;
	case PS_FREE: 
		{
			player->ps = PS_KI; 
			player->aiType = AI_JH;
			// Baby mit einem Namen Taufen ("Name (KI)")
			SetAIName(client);
		} break;
	case PS_KI:
		{
			// Verschiedene KIs durchgehen
			switch(player->aiType)
			{
			case AI_JH:
				player->aiType = AI_DUMMY;
				SetAIName(client);
				break;
			case AI_DUMMY:
				if(mapinfo.map_type != MAPTYPE_SAVEGAME)
					player->ps = PS_LOCKED;
				else
					player->ps = PS_FREE;
				break;
			default:
				if(mapinfo.map_type != MAPTYPE_SAVEGAME)
					player->ps = PS_LOCKED;
				else
					player->ps = PS_FREE;
				break;
			}
			break;
		}
		

	case PS_LOCKED: 
		{
			// Im Savegame können auf geschlossene Slots keine Spieler
			// gesetzt werden, der entsprechende Spieler existierte ja gar nicht auf 
			// der Karte!
			if(mapinfo.map_type != MAPTYPE_SAVEGAME)
				player->ps = PS_FREE;  
		} break;
	}
	player->ready = (player->ps == PS_KI);

	// Tat verkünden
	SendToAll(GameMessage_Player_Toggle_State(client));

	// freie farbe suchen lassen
	bool reserved_colors[PLAYER_COLORS_COUNT];
	memset(reserved_colors, 0, sizeof(bool) * PLAYER_COLORS_COUNT);

	unsigned char rc = 0, cc = 0;
	for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
	{
		GameServerPlayer *ki = &players[cl];

		if( (player != ki) && ( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) ) )
			reserved_colors[ki->color] = true;
		if( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) )
			++rc;
		if( ki->ps == PS_LOCKED )
			++cc;
	}
	if(serverconfig.servertype == NP_LOBBY)
		LOBBYCLIENT.UpdateServerPlayerCount(rc, serverconfig.playercount-cc);

	// bis wir eine freie farbe gefunden haben!
	if(reserved_colors[player->color] && (player->ps == PS_KI || player->ps == PS_OCCUPIED) )
		OnNMSPlayerToggleColor(GameMessage_Player_Toggle_Color(client,player->color));
}

///////////////////////////////////////////////////////////////////////////////
// Team der KI ändern
void GameServer::TogglePlayerTeam(unsigned char client)
{
	GameServerPlayer *player = &players[client];

	// nur KI
	if(player->ps != PS_KI)
		return;

	// team wechseln
	OnNMSPlayerToggleTeam(GameMessage_Player_Toggle_Team(client,Team((player->team+1)%TEAM_COUNT)));
}

///////////////////////////////////////////////////////////////////////////////
// Farbe der KI ändern
void GameServer::TogglePlayerColor(unsigned char client)
{
	GameServerPlayer *player = &players[client];

	// nur KI
	if(player->ps != PS_KI)
		return;

	// Farbe suchen lassen
	OnNMSPlayerToggleColor(GameMessage_Player_Toggle_Color(client,(player->color+1)%PLAYER_COLORS_COUNT));
}

///////////////////////////////////////////////////////////////////////////////
// Nation der KI ändern
void GameServer::TogglePlayerNation(unsigned char client)
{
	GameServerPlayer *player = &players[client];

	// nur KI5
	if(player->ps != PS_KI)
		return;

	// Nation wechseln
	OnNMSPlayerToggleNation(GameMessage_Player_Toggle_Nation(client,Nation((player->nation+1)%NATION_COUNT)));
}

///////////////////////////////////////////////////////////////////////////////
// Spieleinstellungen verschicken
void GameServer::ChangeGlobalGameSettings(const GlobalGameSettings& ggs)
{
	this->ggs = ggs;
	SendToAll(GameMessage_GGSChange(ggs));
	LOG.write("SERVER >>> BROADCAST: NMS_GGS_CHANGE\n");
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Nachricht an Alle
 *
 *  @author FloSoft
 *  @author OLiver
 */
void GameServer::SendToAll(const GameMessage& msg)
{
	for(unsigned int id = 0; id < players.getCount(); ++id)
	{
		GameServerPlayer *player = players.getElement(id);

		// ist der Slot Belegt, dann Nachricht senden
		if(player->ps == PS_OCCUPIED)
			player->send_queue.push(msg.duplicate());
	}
}

///////////////////////////////////////////////////////////////////////////////
// kickt einen spieler und räumt auf
void GameServer::KickPlayer(unsigned char playerid, unsigned char cause, unsigned short param)
{
	NS_PlayerKicked npk;
	npk.playerid = playerid;
	npk.cause = cause;
	npk.param = param;

	KickPlayer(npk);
}

void GameServer::KickPlayer(NS_PlayerKicked npk)
{
	GameServerPlayer *player = &players[npk.playerid];

	// send-queue flushen
	player->send_queue.flush(&player->so);

	// töten, falls außerhalb
	if(status == SS_GAME)
	{
		// KI-Spieler muss übernehmen
		player->ps = PS_KI;
		ai_players[npk.playerid] = GameClient::inst().CreateAIPlayer(npk.playerid);
		// Und Socket schließen, das brauchen wir nicht mehr
		player->so.Close();
	}
	else
		player->clear();

	// trauern
	// beleidskarte verschicken
	SendToAll(GameMessage_Player_Kicked(npk.playerid,npk.cause,npk.param));

	if(serverconfig.servertype == NP_LOBBY)
	{
		unsigned char rc = 0;
		for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
		{
			GameServerPlayer *ki = &players[cl];
			if( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) )
				++rc;
		}
		LOBBYCLIENT.UpdateServerPlayerCount(rc, serverconfig.playercount);
	}

	
	LOG.write("SERVER >>> BROADCAST: NMS_PLAYERKICKED(%d,%d,%d)\n", npk.playerid, npk.cause, npk.param);

	if(status == SS_GAME)
		SendNothingNC(npk.playerid);
}

///////////////////////////////////////////////////////////////////////////////
// testet, ob in der Verbindungswarteschlange Clients auf Verbindung warten
void GameServer::ClientWatchDog()
{
	GameServerPlayer *player = NULL;
	unsigned char client = 0xFF;
	SocketSet set;

	// auf Fehler prüfen
	set.Clear();

	// sockets zum set hinzufügen
	for(client = 0; client < serverconfig.playercount; ++client)
	{
		if( players[client].isValid() )
		{
			// zum set hinzufügen
			set.Add(players[client].so);
		}
	}

	// auf fehler prüfen
	if(set.Select(0, 2) > 0)
	{
		for(client = 0; client < serverconfig.playercount; ++client)
		{
			if(set.InSet(players[client].so))
			{
				LOG.lprintf("SERVER: Error on socket of player %d, bye bye!\n", client);
				KickPlayer(client, NP_CONNECTIONLOST, 0);
			}
		}
	}

	for(client = 0; client < serverconfig.playercount; ++client)
	{
		 player = &players[client];

		 // player anpingen
		 player->doPing();

		 // auf timeout prüfen
		 player->doTimeout();
	}

	// prüfen ob GF vergangen
	if(status == SS_GAME)
	{
		unsigned int currenttime = VideoDriverWrapper::inst().GetTickCount();

		if(!framesinfo.pause)
		{
			// network frame durchführen
			if(currenttime - framesinfo.lasttime > framesinfo.gf_length)
			{
				++framesinfo.gf_nr;
				// KIs ausführen
				for(unsigned i = 0;i<ai_players.size();++i)
				{
					if(ai_players[i])
						ai_players[i]->RunGF(framesinfo.gf_nr);
				}

				// NWF vergangen?
				if(framesinfo.gf_nr % framesinfo.nwf_length == 0)
				{
					// auf laggende spieler prüfen und evtl Kommandos der KI-Spieler senden
					unsigned char lagging_player = 0xFF;

					for(client = 0; client < serverconfig.playercount; ++client)
					{
						player = &players[client];

						if(player->ps == PS_OCCUPIED)
						{
							if(player->gc_queue.size() == 0)
							{
								// der erste, der erwischt wird, bekommt den zuschlag ;-)
								lagging_player = client;
								break;
							}
						}
					}

					if(lagging_player == 0xFF)
					{
						// Diesen Zeitpunkt merken
						framesinfo.lasttime = currenttime - ( currenttime - framesinfo.lasttime - framesinfo.gf_length);

						// Bei evtl. Spielerwechsel die IDs speichern, die "gewechselt" werden sollen
						unsigned char player_switch_old_id = 255,player_switch_new_id= 255;

						// Checksumme des ersten Spielers als Richtwert
						bool took_checksum = false;
						int checksum = 0;

						for(client = 0; client < serverconfig.playercount; ++client)
						{
							player = &players[client];

							if(player->gc_queue.size() > 0)
							{
								// Spieler laggt nicht (mehr ggf)
								player->NotLagging();

								// Checksumme des ersten Spielers als Richtwert
								if(!took_checksum)
								{
									checksum = player->gc_queue.front().checksum;
									took_checksum = true;
								}

								// Checksumme merken, da die Nachricht dann wieder entfernt wird
								player->checksum = player->gc_queue.front().checksum;

								for(std::vector<gc::GameCommand*>::iterator it = player->gc_queue.front().gcs.begin();
									it != player->gc_queue.front().gcs.end();++it)
								{
									if((*it)->GetType() == gc::SWITCHPLAYER)
									{
										// Dann merken und NACH der Schleife erst wechseln!
										player_switch_old_id = client;
										player_switch_new_id = dynamic_cast<gc::SwitchPlayer*>(*it)->GetNewPlayerId();
									}

								}
			
								player->gc_queue.pop_front();
								
								//LOG.lprintf("%d = %d - %d\n", framesinfo.nr, checksum, Random::inst().GetCurrentRandomValue());

								// Checksummen nicht gleich?
								if(player->checksum != checksum)
								{
									// Checksummenliste erzeugen
									std::vector<int> checksums;
									for(unsigned int i = 0; i < players.getCount(); ++i)
										checksums.push_back(players.getElement(i)->checksum);

									// Async-Meldung rausgeben.
									SendToAll(GameMessage_Server_Async(checksums));

									// Spiel pausieren
									TogglePause();

									// diesen Spieler kicken
									KickPlayer(client, NP_ASYNC, 0);

									// rausgehen
									continue;
								}

								
							}

							// Befehle der KI senden
							if(player->ps == PS_KI)
							{
								SendToAll(GameMessage_GameCommand(client,0,ai_players[client]->GetGameCommands()));
								ai_players[client]->FetchGameCommands();
							}
						}

						// Evtl den Spieler wechseln?
						if(player_switch_old_id != 255)
							ChangePlayer(player_switch_old_id,player_switch_new_id);

			
						SendToAll(GameMessage_Server_NWFDone(0xff));
						// Framecounter erhöhen
						++framesinfo.nr;
					}
					else
					{
						if((currenttime - framesinfo.lasttime) > framesinfo.nwf_length*framesinfo.gf_length)
						{
							//// ein spieler laggt, spiel pausieren
							GameServerPlayer *player = &players[lagging_player];
							player->Lagging();
							if(player->GetTimeOut() == 0)
								KickPlayer(lagging_player, NP_PINGTIMEOUT, 0);

							if(!(player->GetTimeOut() % 5))
								LOG.lprintf("SERVER: Kicke Spieler %d in %u Sekunden\n", lagging_player, player->GetTimeOut());
			
						}
					}
				}
				else
				{
					// Diesen Zeitpunkt merken
					framesinfo.lasttime = currenttime;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Sendet ein NC-Paket ohne Befehle.
 *
 *  @author OLiver
 */
void GameServer::SendNothingNC(const unsigned int &id)
{
	SendToAll(GameMessage_GameCommand(id,0,std::vector<gc::GameCommand*>()));
}

///////////////////////////////////////////////////////////////////////////////
// testet, ob in der Verbindungswarteschlange Clients auf Verbindung warten
void GameServer::WaitForClients(void)
{
	SocketSet set;

	set.Add(serversocket);
	if(set.Select(0, 0) > 0)
	{
		if(set.InSet(serversocket))
		{
			Socket socket;
			unsigned char playerid = 0xFF;

			// Verbindung annehmen
			if(!serversocket.Accept(socket))
				return;

			// Geeigneten Platz suchen
			for(unsigned int client = 0; client < serverconfig.playercount; ++client)
			{
				if(players[client].ps == PS_FREE)
				{
					// platz reservieren
					players[client].reserve(&socket, client);
					playerid = client;

					// schleife beenden
					break;
				}
			}

			GameMessage_Player_Id msg(playerid);
			msg.send(&socket);

			// war kein platz mehr frei, wenn ja dann verbindung trennen?
			if(playerid == 0xFF)
				socket.Close();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// füllt die warteschlangen mit "paketen"
void GameServer::FillPlayerQueues(void)
{
	SocketSet set;
	unsigned char client = 0xFF;
	bool not_empty = false;

	// erstmal auf Daten überprüfen
	do
	{
		// sockets zum set hinzufügen
		for(client = 0; client < serverconfig.playercount; ++client)
		{
			if( players[client].isValid() )
			{
				// zum set hinzufügen
				set.Add(players[client].so);
			}
		}

		not_empty = false;

		// ist eines der Sockets im Set lesbar?
		if(set.Select(0, 0) > 0)
		{
			for(client = 0; client < serverconfig.playercount; ++client)
			{
				if( players[client].isValid() && set.InSet(players[client].so) )
				{
					// nachricht empfangen
					if(!players[client].recv_queue.recv(&players[client].so))
					{
						LOG.lprintf("SERVER: Receiving Message for player %d failed, kick it like Beckham!\n", client);
						KickPlayer(client, NP_CONNECTIONLOST, 0);
					}

					if(players[client].ps == PS_OCCUPIED)
						not_empty = true;
				}
			}
		}
	} while(not_empty);
}

///////////////////////////////////////////////////////////////////////////////
// pongnachricht
inline void GameServer::OnNMSPong(const GameMessage_Pong& msg)
{
	GameServerPlayer *player = &players[msg.player];

	unsigned int currenttime = VideoDriverWrapper::inst().GetTickCount();

	player->ping = (unsigned short)(currenttime - player->lastping);
	player->pinging = false;
	player->lastping = currenttime;

	// Den neuen Ping allen anderen Spielern Bescheid sagen
	GameMessage_Player_Ping ping_msg(msg.player,player->ping);
	SendToAll(ping_msg);
	//LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_PING(%d,%u)\n", client, *ping);
}

///////////////////////////////////////////////////////////////////////////////
// servertype
inline void GameServer::OnNMSServerType(const GameMessage_Server_Type& msg)
{
	GameServerPlayer *player = &players[msg.player];

	int typok = 0;
	if(msg.type != serverconfig.servertype)
		typok = 1;
	else if(msg.version != GetWindowVersion())
		typok = 2;

	player->send_queue.push(new GameMessage_Server_TypeOK(typok));

	if(typok != 0)
		KickPlayer(msg.player, NP_CONNECTIONLOST, 0);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Server-Passwort-Nachricht
 *
 *  @author FloSoft
 */
void GameServer::OnNMSServerPassword(const GameMessage_Server_Password& msg)
{
	GameServerPlayer *player = &players[msg.player];

	std::string passwordok = (serverconfig.password == msg.password ? "true" : "false");

	player->send_queue.push(new GameMessage_Server_Password(passwordok));

	if(passwordok == "false")
		KickPlayer(msg.player, NP_WRONGPASSWORD, 0);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Server-Chat-Nachricht.
 *
 *  @author FloSoft
 *  @author OLiver
 */
void GameServer::OnNMSServerChat(const GameMessage_Server_Chat& msg)
{
	//GameServerPlayer *player = &players[msg.player];
	SendToAll(msg);

	/*switch(msg.destination)
	{
	default:
	case CD_ALL: // Alle
		{
			SendToAll(msg);
		} break;
	case CD_ALLIES: // Verbündete
		{
			// Besiegte dürfen nicht mehr heimlich mit Verbündeten reden
			if(!player->isDefeated())
			{
				for(unsigned int i = 0; i < players.getCount(); ++i)
				{
					GameServerPlayer *p = players.getElement(i);
					
					if(p->ps == PS_OCCUPIED && p->team == player->team && p->team != TM_NOTEAM)
						p->send_queue.push(msg.duplicate());
				}
			}
		} break;
	case CD_ENEMIES: // Feinde
		{
			// Besiegte dürfen nicht mehr heimlich mit Feinden reden
			if(!player->isDefeated())
			{
				for(unsigned int i = 0; i < players.getCount(); ++i)
				{
					GameServerPlayer *p = players.getElement(i);

					// Nur an Feinde aber auch an sich selber senden, damit man sieht, was man geschrieben hat
					if(p->ps == PS_OCCUPIED && ( p->team != player->team || i == msg.player ) )
						p->send_queue.push(msg.duplicate());
				}
			}
		} break;
	}*/
}

///////////////////////////////////////////////////////////////////////////////
// Spielername
inline void GameServer::OnNMSPlayerName(const GameMessage_Player_Name& msg)
{
	GameServerPlayer *player = &players[msg.player];

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_NAME(%s)\n", msg.player, msg.playername.c_str());

	player->name = msg.playername;

	// Als Antwort Karteninformationen übertragen
	player->temp_ul = 0;

	player->send_queue.push(new GameMessage_Map_Info(mapinfo.name, mapinfo.map_type, mapinfo.partcount, 
		mapinfo.ziplength, mapinfo.length));

	// Und Kartendaten
	for(unsigned i = 0;i<mapinfo.partcount;++i)
	{
		unsigned data_size = ( (mapinfo.ziplength - player->temp_ul) > MAP_PART_SIZE )
			? MAP_PART_SIZE : (mapinfo.ziplength - player->temp_ul);

		player->send_queue.push(new GameMessage_Map_Data(&mapinfo.zipdata[player->temp_ul], data_size));
		player->temp_ul += data_size;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Nation weiterwechseln
inline void GameServer::OnNMSPlayerToggleNation(const GameMessage_Player_Toggle_Nation& msg)
{
	GameServerPlayer *player = &players[msg.player];

	player->nation = msg.nation;

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_TOGGLENATION\n", msg.player);

	// Nation-Change senden
	SendToAll(GameMessage_Player_Toggle_Nation(msg.player,msg.nation));

	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_TOGGLENATION(%d, %d)\n", msg.player, player->nation);
}

///////////////////////////////////////////////////////////////////////////////
// Team weiterwechseln
inline void GameServer::OnNMSPlayerToggleTeam(const GameMessage_Player_Toggle_Team& msg)
{
	GameServerPlayer *player = &players[msg.player];

	player->team = msg.team;

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_TOGGLETEAM\n", msg.player);
	SendToAll(GameMessage_Player_Toggle_Team(msg.player,msg.team));
	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_TOGGLETEAM(%d, %d)\n", msg.player, player->team);
}

///////////////////////////////////////////////////////////////////////////////
// Farbe weiterwechseln
inline void GameServer::OnNMSPlayerToggleColor(const GameMessage_Player_Toggle_Color& msg)
{
	GameServerPlayer *player = &players[msg.player];

	// ist die farbe auch frei, wenn nicht, "überspringen"?
	bool reserved_colors[PLAYER_COLORS_COUNT];
	memset(reserved_colors, 0, sizeof(bool) * PLAYER_COLORS_COUNT);

	for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
	{
		GameServerPlayer *ki = &players[cl];

		if( (player != ki) && ( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) ) )
			reserved_colors[ki->color] = true;
	}
	do {
		player->color = (player->color + 1) % PLAYER_COLORS_COUNT;
	} while(reserved_colors[player->color]);

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_TOGGLECOLOR\n", msg.player);
	SendToAll(GameMessage_Player_Toggle_Color(msg.player,player->color));
	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_TOGGLECOLOR(%d, %d)\n", msg.player, player->color);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Spielerstatus wechseln
 *
 *  @author FloSoft
 */
inline void GameServer::OnNMSPlayerReady(const GameMessage_Player_Ready& msg)
{
	GameServerPlayer *player = &players[msg.player];

	player->ready = msg.ready;

	// countdown ggf abbrechen
	if(!player->ready && countdown.do_countdown)
		CancelCountdown();

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_READY(%s)\n", msg.player, (player->ready ? "true" : "false"));
	// Ready-Change senden
	SendToAll(GameMessage_Player_Ready(msg.player, msg.ready));
	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_READY(%d, %s)\n", msg.player, (player->ready ? "true" : "false"));
}


///////////////////////////////////////////////////////////////////////////////
// Checksumme
inline void GameServer::OnNMSMapChecksum(const GameMessage_Map_Checksum& msg)
{
	GameServerPlayer *player = &players[msg.player];

	bool checksumok = false;

	if(msg.checksum == mapinfo.checksum)
		checksumok = true;

	LOG.write("CLIENT%d >>> SERVER: NMS_MAP_CHECKSUM(%u) expected: %u, ok: %s\n", msg.player, msg.checksum, mapinfo.checksum, checksumok ? "yes" : "no");

	// Antwort senden
	player->send_queue.push(new GameMessage_Map_ChecksumOK(checksumok));

	LOG.write("SERVER >>> CLIENT%d: NMS_MAP_CHECKSUM(%d)\n", msg.player, checksumok);

	if(!checksumok)
		KickPlayer(msg.player, NP_WRONGCHECKSUM, 0);
	else
	{
		// den anderen Spielern mitteilen das wir einen neuen haben
		SendToAll(GameMessage_Player_New(msg.player,player->name));

		LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_NEW(%d, %s)\n", msg.player, player->name.c_str());

		// belegt markieren
		player->ps = PS_OCCUPIED;
		player->lastping = VideoDriverWrapper::inst().GetTickCount();

		// Servername senden
		player->send_queue.push(new GameMessage_Server_Name(serverconfig.gamename));

		// freie farbe suchen lassen
		bool reserved_colors[PLAYER_COLORS_COUNT];
		memset(reserved_colors, 0, sizeof(bool) * PLAYER_COLORS_COUNT);

		unsigned char rc = 0;
		for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
		{
			GameServerPlayer *ki = &players[cl];

			if( (player != ki) && ( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) ) )
				reserved_colors[ki->color] = true;
			if( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) )
				++rc;
		}
		if(serverconfig.servertype == NP_LOBBY)
			LOBBYCLIENT.UpdateServerPlayerCount(rc, serverconfig.playercount);

		// Spielerliste senden
		player->send_queue.push(new GameMessage_Player_List(players));

		// bis wir eine freie farbe gefunden haben!
		// (while-schleife obwohl OnNMSPlayerToggleColor selbst schon eine freie sucht)
		while(reserved_colors[player->color])
			OnNMSPlayerToggleColor(GameMessage_Player_Toggle_Color(msg.player,player->color));

		// GGS senden
		player->send_queue.push(new GameMessage_GGSChange(ggs));

		LOG.write("SERVER >>> BROADCAST: NMS_GGS_CHANGE\n");
	}
}

void GameServer::OnNMSGameCommand(const GameMessage_GameCommand& msg)
{
	GameServerPlayer *player = &players[msg.player];

	// NFCs speichern
	player->gc_queue.push_back(msg);

	//// Command schließlich an alle Clients weiterleiten, aber nicht in der Pause und nicht, wenn derjenige Spieler besiegt wurde!
	if(!this->framesinfo.pause && !players[msg.player].isDefeated())
		SendToAll(msg);
	else
		SendToAll(GameMessage_GameCommand(msg.player,msg.checksum,std::vector<gc::GameCommand*>()));
}

void GameServer::ChangePlayer(const unsigned char old_id, const unsigned char new_id)
{
	// old_id muss richtiger Spieler, new_id KI sein, ansonsten geht das natürlich nicht
	if( !(players[old_id].ps == PS_OCCUPIED && players[new_id].ps == PS_KI) )
		return;

	players[old_id].ps = PS_KI;
	players[new_id].ps = PS_OCCUPIED;
	players[new_id].so = players[old_id].so;

	// Alte KI löschen
	delete ai_players[new_id];
	ai_players[new_id] = 0;

	ai_players[old_id] = GameClient::inst().CreateAIPlayer(old_id);
}



bool GameServer::TogglePause()
{
	framesinfo.pause = !framesinfo.pause;
	SendToAll(GameMessage_Pause(framesinfo.pause));

	return framesinfo.pause;
}


void GameServer::SwapPlayer(const unsigned char player1, const unsigned char player2)
{
	// Message verschicken
	SendToAll(GameMessage_Player_Swap(player1, player2));
	// Spieler vertauschen
	players[player1].SwapPlayer(players[player2]);
}

void GameServer::SetAIName(const unsigned player_id)
{
	// Baby mit einem Namen Taufen ("Name (KI)")
	char str[128];
	if (players[player_id].aiType == AI_DUMMY)
		sprintf(str,_("Dummy %u"),unsigned(player_id));
	else
		sprintf(str,_("Computer %u"),unsigned(player_id));

	players[player_id].name = str;
	players[player_id].name += _(" (AI)");
}


void GameServer::SendAIEvent(AIEvent::Base *ev, unsigned receiver)
{
	if (ai_players[receiver] != NULL)
		ai_players[receiver]->SendAIEvent(ev);
	else
		delete ev;
}
