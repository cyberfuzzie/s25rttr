// $Id: GameServer.cpp 4870 2009-05-15 21:27:56Z OLiver $
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
#include "GameServer.h"

#include "SocketSet.h"

#include "Loader.h"
#include "Random.h"
#include "VideoDriverWrapper.h"
#include "GameMessage.h"
#include "GameClient.h"

#include "FileChecksum.h"
#include "GlobalGameSettings.h"
#include "LobbyClient.h"
#include "iwDirectIPCreate.h"

#include "GameServerPlayer.h"

#include "GameFiles.h"

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
	map_type = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
GameServer::GameServer(void)
{
	status = SS_STOPPED;

	framesinfo.Clear();
	serverconfig.Clear();
	mapinfo.Clear();

	players = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
GameServer::~GameServer(void)
{
	Stop();
}

///////////////////////////////////////////////////////////////////////////////
// Spiel hosten
bool GameServer::TryToStart(const CreateServerInfo& csi, const std::string& map_path, const unsigned map_type)
{
	Stop();

	// Name, Password und Kartenname kopieren
	serverconfig.gamename = csi.gamename;
	serverconfig.password = csi.password;
	serverconfig.mapname = map_path;
	serverconfig.servertype = csi.type;
	serverconfig.port = csi.port;
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
	if(serverconfig.servertype == NP_DEPENDENT)
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

	fread(map_data, 1, mapinfo.length, map_f);
	fclose(map_f);

	// map mit bzip2 komprimieren
	int err = BZ_OK;
	if( (err = BZ2_bzBuffToBuffCompress( (char*)mapinfo.zipdata, (unsigned int*)&mapinfo.ziplength, map_data, mapinfo.length, 9, 0, 250)) != BZ_OK)
	{
		LOG.lprintf("FATAL ERROR: BZ2_bzBuffToBuffCompress failed with error: %d\n", err);
		return false;
	}

	mapinfo.checksum = CalcChecksumOfBuffer((unsigned char*)map_data, mapinfo.length);

	delete[] map_data;

	mapinfo.partcount = mapinfo.ziplength / 4096 +  ( (mapinfo.ziplength%4096) ? 1 : 0);

	// Speicher für Spieler anlegen
	players = new GameServerPlayer*[serverconfig.playercount];
	for(unsigned char i = 0; i < serverconfig.playercount; ++i)
		players[i] = new GameServerPlayer(i);


	bool host_found = false;

	//// Spieler 0 erstmal der Hos
	switch(mapinfo.map_type)
	{
	case MAPTYPE_OLDMAP:
		{
			// Host bei normalen Spieler der erste Spieler
			players[0]->is_host = true;
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
				players[i]->ps = PlayerState(save.players[i].ps);

				if(players[i]->ps != PS_LOCKED)
				{
					// (ehemaliger) Spielername
					players[i]->origin_name = save.players[i].name;

					// Volk, Team und Farbe
					players[i]->nation = save.players[i].nation;
					players[i]->color = save.players[i].color;
					players[i]->team = save.players[i].team;
					
				}

				// Besetzt --> freigeben, damit auch jemand reinkann
				if(players[i]->ps == PS_OCCUPIED)
				{
					players[i]->ps = PS_FREE;
					// Erster richtiger Spieler? Dann ist das der Host später
					if(!host_found)
					{
						players[i]->is_host = true;
						host_found = true;
					}
				}
				// KI-Spieler? Namen erzeugen
				else if(players[i]->ps == PS_KI)
					SetKIName(i);
			}

			// Und die GGS
			ggs = save.ggs;
		} break;
	}
	

	if(players == NULL)
		return false;

	// ab in die Konfiguration
	status = SS_CONFIG;

	// und das socket in listen-modus schicken
	if(!serversocket.Listen(serverconfig.port))
	{
		LOG.lprintf("GameServer::Start: ERROR: Listening on port %d failed!\n", serverconfig.port);
		LOG.getlasterror("Fehler");
		return false;
	}

	// Zu sich selbst connecten als Host
	GAMECLIENT.Connect("localhost", serverconfig.password, serverconfig.servertype, serverconfig.port, true);

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

	// queues abarbeiten
	for(unsigned int client = 0; client < serverconfig.playercount; ++client)
	{
		GameServerPlayer *player = players[client];
		unsigned char count = 0;

		// send-queue abarbeiten
		while(player->send_queue.count() > 0)
		{
			// maximal 10 Pakete verschicken
			if(count > 10)
				break;

			GameMessage *m = player->send_queue.front();

			// maximal 1 großes Paket verschicken
			if(count > 0 && m->m_uiLength > 512)
				break;

			if(m->m_usID > 0)
			{
				if(!m->send(&player->so))
				{
					LOG.lprintf("SERVER: Sending Message for player %d failed, kick it like Beckham!\n", client);
					KickPlayer(client, NP_CONNECTIONLOST, 0);
				}
				else
				{
					player->send_queue.pop();
					++count;
				}
			}
		}

		// recv-queue abarbeiten
		while(player->recv_queue.count() > 0)
		{
			GameMessage *m = player->recv_queue.front();
			player->recv_queue.pop();

			if(m->m_usID > 0)
			{
				if(status == SS_CONFIG)
					HandleMessage(m, client);
				if(status == SS_GAME)
					HandleGameMessage(m, client);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// stoppt den server
void GameServer::Stop(void)
{
	// player verabschieden
	if(players)
	{
		for(unsigned char client = 0; players && client < serverconfig.playercount; ++client)
		{
			if(players[client]->isValid())
				KickPlayer(client, NP_CONNECTIONLOST, 0);
			delete players[client];
		}

		// und beerdigen
		delete[] players;
		players = NULL;
	}

	// aufräumen
	framesinfo.Clear();
	serverconfig.Clear();
	mapinfo.Clear();

	// laden dicht machen
	serversocket.Close();

	if(status != SS_STOPPED)
		LOG.lprintf("server state changed to stop\n");

	// status
	status = SS_STOPPED;
}


///////////////////////////////////////////////////////////////////////////////
// startet das Spiel
bool GameServer::StartGame()
{
	GameServerPlayer *player = NULL;
	unsigned char client = 0xFF;

	// Alle Spieler da?
	for(client = 0; client < serverconfig.playercount; ++client)
	{
		player = players[client];

		// noch nicht alle spieler da -> feierabend!
		if( (player->ps == PS_FREE) || (player->ps == PS_RESERVED) )
			return false;
		else if(player->ps != PS_LOCKED && player->ps != PS_KI && !player->ready)
			return false;
	}

	bool reserved_colors[GAME_COLORS_COUNT];
	memset(reserved_colors, 0, sizeof(bool) * GAME_COLORS_COUNT);

	// Alle ne andere Farbe?
	for(client = 0; client < serverconfig.playercount; ++client)
	{
		player = players[client];

		if( (player->ps == PS_OCCUPIED) || (player->ps == PS_KI) )
		{
			// farbe schon belegt -> und tschüss
			if(reserved_colors[player->color])
				return false;

			reserved_colors[player->color] = true;
		}
	}

	NS_StartGameInfo nsgi;
	// Bei Savegames wird der Startwert von den Clients aus der Datei gelesen!
	nsgi.random_init = (mapinfo.map_type==MAPTYPE_SAVEGAME)?0xFFFFFFFF:VideoDriverWrapper::inst().GetTickCount();

	

	// Höchsten Ping ermitteln
	unsigned highest_ping = 0;

	for(client = 0; client < serverconfig.playercount; ++client)
	{
		player = players[client];

		if(player->ps == PS_OCCUPIED)
		{
			if(player->ping > highest_ping)
				highest_ping = player->ping;
		}
	}

	unsigned gf_length = SPEED_GF_LENGTHS[ggs.game_speed];

	// NetworkFrame-Länge bestimmen, je schlechter (also höher) die Pings, desto länger auch die Framelänge
	unsigned i = 1;
	for(;i<20;++i)
	{
		if(i*gf_length > highest_ping+200)
			break;
	}

	framesinfo.nwf_length = i*gf_length;

	nsgi.nwf_length = i;
		

	LOG.lprintf("SERVER: Using gameframe length of %dms\n", gf_length);
	LOG.lprintf("SERVER: Using networkframe length of %ums\n", framesinfo.nwf_length);

	GameMessage startmessage(NMS_SERVER_START);
	startmessage.m_uiLength = sizeof(NS_StartGameInfo);
	startmessage.alloc(&nsgi);

	// Spielstart allen mitteilen
	SendToAll(startmessage);
	LOG.write("SERVER >>> BROADCAST: NMS_SERVER_START(%d)\n", nsgi.random_init);

	// ab ins game wechseln
	status = SS_GAME;

	framesinfo.lasttime = VideoDriverWrapper::inst().GetTickCount();

	// Spielstart allen mitteilen
	GameMessage firstframe(NMS_NFC_DONE);
	SendToAll(firstframe);

	// Erste KI-Nachrichten schicken
	for(unsigned char i = 0;i<this->serverconfig.playercount;++i)
	{
		if(players[i]->ps == PS_KI)
		{
			SendNothingNC(i);
			//GameMessage nfc(NMS_NFC_ANSWER, 2);
			//unsigned char * data = static_cast<unsigned char*>(nfc.m_pData);
			//*data = i;
			//*(data+1) = 0;
			////*((unsigned short*)(&data[1])) = NC_NOTHING;
			//SendToAll(nfc);
		}
	}

	LOG.write("SERVER >>> BROADCAST: NMS_NFC_DONE\n");

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// wechselt Spielerstatus durch
void GameServer::TogglePlayerState(unsigned char client)
{
	GameServerPlayer *player = players[client];

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
			// Baby mit einem Namen Taufen ("Name (KI)")
			SetKIName(client);
		} break;
	case PS_KI:     player->ps = PS_LOCKED; break;
	case PS_LOCKED: player->ps = PS_FREE;   break;
	}
	player->ready = (player->ps == PS_KI);

	GameMessage msg(NMS_PLAYER_TOGGLESTATE);
	msg.m_uiLength = 1;
	msg.alloc(&client);

	// tat verkünden!
	SendToAll(msg);

	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_TOGGLESTATE(%d)\n", client);

	// freie farbe suchen lassen
	bool reserved_colors[GAME_COLORS_COUNT];
	memset(reserved_colors, 0, sizeof(bool) * GAME_COLORS_COUNT);

	unsigned char rc = 0, cc = 0;
	for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
	{
		GameServerPlayer *ki = players[cl];

		if( (player != ki) && ( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) ) )
			reserved_colors[ki->color] = true;
		if( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) )
			++rc;
		if( ki->ps == PS_LOCKED )
			++cc;
	}
	if(serverconfig.servertype == NP_DEPENDENT)
		LOBBYCLIENT.UpdateServerPlayerCount(rc, serverconfig.playercount-cc);

	// bis wir eine freie farbe gefunden haben!
	// (while-schleife obwohl OnNMSPlayerToggleColor selbst schon eine freie sucht)
	while(reserved_colors[player->color])
	{
		GameMessage msg(NMS_PLAYER_TOGGLECOLOR);
		OnNMSPlayerToggleColor(&msg, client);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Team der KI ändern
void GameServer::TogglePlayerTeam(unsigned char client)
{
	GameServerPlayer *player = players[client];

	// nur KI
	if(player->ps != PS_KI)
		return;

	// team wechseln
	GameMessage msg(NMS_PLAYER_TOGGLETEAM);
	OnNMSPlayerToggleTeam(&msg, client);
}

///////////////////////////////////////////////////////////////////////////////
// Farbe der KI ändern
void GameServer::TogglePlayerColor(unsigned char client)
{
	GameServerPlayer *player = players[client];

	// nur KI
	if(player->ps != PS_KI)
		return;

	// Farbe suchen lassen
	GameMessage msg(NMS_PLAYER_TOGGLECOLOR);
	OnNMSPlayerToggleColor(&msg, client);
}

///////////////////////////////////////////////////////////////////////////////
// Nation der KI ändern
void GameServer::TogglePlayerNation(unsigned char client)
{
	GameServerPlayer *player = players[client];

	// nur KI
	if(player->ps != PS_KI)
		return;

	// nation wechseln
	GameMessage msg(NMS_PLAYER_TOGGLENATION);
	OnNMSPlayerToggleNation(&msg, client);
}

///////////////////////////////////////////////////////////////////////////////
// Spieleinstellungen verschicken
void GameServer::ChangeGlobalGameSettings(const GlobalGameSettings& ggs)
{
	this->ggs = ggs;

	GameMessage msg(NMS_GGS_CHANGE);

	// Einstellungen serialisieren
	unsigned char *buffer = ggs.Serialize();

	msg.m_uiLength = GlobalGameSettings::GGS_BUFFER_SIZE;
	msg.alloc(buffer);

	delete[] buffer;

	// und verschicken
	SendToAll(msg);

	LOG.write("SERVER >>> BROADCAST: NMS_GGS_CHANGE\n");
}

///////////////////////////////////////////////////////////////////////////////
// Nachricht an Alle
void GameServer::SendToAll(const GameMessage& msg)
{
	unsigned char client = 0xFF;
	GameServerPlayer *player = NULL;

	for(client = 0; client < serverconfig.playercount; ++client)
	{
		player = players[client];

		// ist der slot belegt, dann nachricht senden
		if(player->ps == PS_OCCUPIED)
		{
			/*if(!msg->send(&player->so))
			{
				LOG.lprintf("SERVER: Failed to send broadcast message to player %d, cya next life!\n", client);
				KickPlayer(client, NP_CONNECTIONLOST, 0);
			}*/
			player->send_queue.push(msg);
		}
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
	GameServerPlayer *player = players[npk.playerid];

	// send-queue flushen
	player->send_queue.flush(&player->so);

	// deadmessage schicken
	GameMessage deadmessage(NMS_DEAD_MSG);
	deadmessage.send(&player->so);

	// töten, falls außerhalb
	if(status == SS_GAME)
	{
		player->ps = PS_KI;
		player->so.Close();
	}
	else
		player->clear();

	// trauern
	GameMessage msg(NMS_PLAYER_KICKED);
	msg.m_uiLength = sizeof(NS_PlayerKicked);
	msg.alloc(&npk);

	if(serverconfig.servertype == NP_DEPENDENT)
	{
		unsigned char rc = 0;
		for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
		{
			GameServerPlayer *ki = players[cl];
			if( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) )
				++rc;
		}
		LOBBYCLIENT.UpdateServerPlayerCount(rc, serverconfig.playercount);
	}

	// beleidskarte verschicken
	SendToAll(msg);
	LOG.write("SERVER >>> BROADCAST: NMS_PLAYERKICKED(%d,%d,%d)\n", npk.playerid, npk.cause, npk.param);

	if(status == SS_GAME)
	{
		SendNothingNC(npk.playerid);
		/*GameMessage nfc(NMS_NFC_ANSWER, 2);
		unsigned char * data = static_cast<unsigned char*>(nfc.m_pData);
		*data = npk.playerid;
		*(data+1) = 0;
		SendToAll(nfc);*/
	}
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
		if( players[client]->isValid() )
		{
			// zum set hinzufügen
			set.Add(players[client]->so);
		}
	}

	// auf fehler prüfen
	if(set.Select(0, 2) > 0)
	{
		for(client = 0; client < serverconfig.playercount; ++client)
		{
			if(set.InSet(players[client]->so))
			{
				LOG.lprintf("SERVER: Error on socket of player %d, bye bye!\n", client);
				KickPlayer(client, NP_CONNECTIONLOST, 0);
			}
		}
	}

	for(client = 0; client < serverconfig.playercount; ++client)
	{
		 player = players[client];

		 // player anpingen
		 player->doPing();

		 // auf timeout prüfen
		 player->doTimeout();
	}

	// prüfen ob Netwerkframe vergangen
	if(status == SS_GAME)
	{
		unsigned int currenttime = VideoDriverWrapper::inst().GetTickCount();

		if(!framesinfo.pause)
		{
			// network frame durchführen
			if(currenttime - framesinfo.lasttime > framesinfo.nwf_length)
			{
				// auf laggende spieler prüfen und evtl Kommandos der KI-Spieler senden
				unsigned char lagging_player = 0xFF;

				for(client = 0; client < serverconfig.playercount; ++client)
				{
					player = players[client];

					if(player->ps == PS_OCCUPIED)
					{
						if(player->nfc_queue.size() == 0)
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
					framesinfo.lasttime = currenttime - ( currenttime - framesinfo.lasttime - framesinfo.nwf_length);

					// Bei evtl. Spielerwechsel die IDs speichern, die "gewechselt" werden sollen
					unsigned char player_switch_old_id = 255,player_switch_new_id= 255;

					// Checksumme des ersten Spielers als Richtwert
					bool took_checksum = false;
					int checksum = 0;

					for(client = 0; client < serverconfig.playercount; ++client)
					{
						player = players[client];
					/*	player->timeout = 30;
						player->lasttimeout = 0;*/

						if(player->nfc_queue.size() > 0)
						{
							// Spieler laggt nicht (mehr ggf)
							player->NotLagging();

							// Checksumme des ersten Spielers als Richtwert
							if(!took_checksum)
							{
								checksum = *static_cast<int*>((*player->nfc_queue.begin())->m_pData);
								took_checksum = true;
							}

							GameMessage * gw = (*player->nfc_queue.begin());
							// Checksumme merken, da die Nachricht dann wieder entfernt wird
							player->checksum = *static_cast<int*>((*players[client]->nfc_queue.begin())->m_pData);

							if(gw->m_uiLength > 0)
							{
								// Spielerwechsel (alle NCs durchgehen und nach Spielerwechselcommand suchen)
								unsigned char * p = static_cast<unsigned char*>(gw->m_pData);
								// Checksumme überspringen
								p+=4;
								// NC-Anzahl auslesen
								unsigned char nc_count = *p;

								++p;

								for(unsigned char i = 0;i<nc_count;++i)
								{
									// Länge des Commands

									unsigned short length = *((unsigned short*)p);
									p+=2;
									if(length > 0)
									{
										// Daten
										if(*((unsigned short*)p) == NC_SWITCHPLAYER)
										{
											// Dann merken und NACH der Schleife erst wechseln!
											player_switch_old_id = client;
											player_switch_new_id = p[2];
										}
										p+=length;
									}
								}
							}

							delete gw;
							player->nfc_queue.pop_front();
							
							//LOG.lprintf("%d = %d - %d\n", framesinfo.nr, checksum, Random::inst().GetCurrentRandomValue());

							// Checksummen nicht gleich?
							if(player->checksum != checksum)
							{
								// Das muss den Clients mitgeteilt werden! --> Nachricht erzeugen
								GameMessage msg(NMS_SERVER_ASYNC,serverconfig.playercount*4);
								for(unsigned i = 0; i < serverconfig.playercount; ++i)
									static_cast<int*>(msg.m_pData)[i] = players[i]->checksum;

								SendToAll(msg);

								// Spiel pausieren
								TogglePause();

								// diesen Spieler kicken
								KickPlayer(client,NP_ASYNC,0);

								// rausgehen
								continue;
							}

							
						}

						// Befehle der KI senden
						if(player->ps == PS_KI)
						{
							SendNothingNC(client);
							//GameMessage nfc(NMS_NFC_ANSWER, 2);
							//unsigned char * data = static_cast<unsigned char*>(nfc.m_pData);
							//*data = client;
							//*(data+1) = 0;
							////*((unsigned short*)(&data[1])) = NC_NOTHING;
							//SendToAll(nfc);
						}

					}

					// Evtl den Spieler wechseln?
					if(player_switch_old_id != 255)
						ChangePlayer(player_switch_old_id,player_switch_new_id);

					GameMessage msg(NMS_NFC_DONE);

					// Done Message senden
					SendToAll(msg);
					//LOG.write("SERVER >>> BROADCAST: NMS_NFC_DONE\n");

					// Framecounter erhöhen
					++framesinfo.nr;
				}
				else
				{
					if((currenttime - framesinfo.lasttime) > framesinfo.nwf_length)
					{
						//// ein spieler laggt, spiel pausieren
						//LOG.lprintf("SERVER: %03lu Player %d lags (delay %lums)\n", framesinfo.nr, lagging_player, (currenttime - framesinfo.lasttime) - framesinfo.nwf_length);

						//networkframe.paused = true;
						//networkframe.pausetime = currenttime;

						GameServerPlayer *player = players[lagging_player];
						player->Lagging();
						if(player->GetTimeOut() == 0)
						{
							// ???
							//player->timeout = 30;

							KickPlayer(lagging_player, NP_PINGTIMEOUT, 0);
						}
			/*			if( (currenttime - player->lasttimeout) > 1000)
						{*/
							if(!(player->GetTimeOut() % 5))
								LOG.lprintf("SERVER: Kicke Spieler %d in %u Sekunden\n", lagging_player, player->GetTimeOut());
			
						//	player->lasttimeout = currenttime;
						//}
					}
				}
			}
		}
	}
}

void GameServer::SendNothingNC(const unsigned char player)
{
	GameMessage nfc(NMS_NFC_ANSWER, 1 + 5);
	unsigned char *data = (unsigned char*)nfc.m_pData;
	data[0] = player;
	/*int *rest = (int*)&data[1];
	rest[0] = RANDOM.GetCurrentRandomValue();*/

	SendToAll(nfc);
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
				if(players[client]->ps == PS_FREE)
				{
					// platz reservieren
					players[client]->reserve(&socket, client);
					playerid = client;

					// schleife beenden
					break;
				}
			}

			GameMessage msg(NMS_PLAYER_ID);

			char *data = (char*)msg.alloc(1);
			data[0] = playerid;

			// Serverstatus senden
			msg.send(&socket);
			LOG.write("SERVER >>> CLIENT%d: NMS_PLAYER_ID(%d)\n", playerid, playerid);

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
			if( players[client]->isValid() )
			{
				// zum set hinzufügen
				set.Add(players[client]->so);
			}
		}

		not_empty = false;

		// ist eines der Sockets im Set lesbar?
		if(set.Select(0, 0) > 0)
		{
			for(client = 0; client < serverconfig.playercount; ++client)
			{
				if( players[client]->isValid() && set.InSet(players[client]->so) )
				{
					// nachricht empfangen
					if(players[client]->recv_queue.recv(&players[client]->so) == -1)
					{
						LOG.lprintf("SERVER: Receiving Message for player %d failed, kick it like Beckham!\n", client);
						KickPlayer(client, NP_CONNECTIONLOST, 0);
					}

					if(players[client]->ps == PS_OCCUPIED)
						not_empty = true;
				}
			}
		}
	} while(not_empty);
}

///////////////////////////////////////////////////////////////////////////////
// führt eine nachricht aus
void GameServer::HandleMessage(GameMessage *message, unsigned char client)
{
	if(!message)
		return;

	switch(message->m_usID)
	{
	default:			break;
	case NMS_NULL_MSG:	break;

	case NMS_PONG: OnNMSPong(message, client); break;

	case NMS_SERVER_TYP:		OnNMSServerType(message, client);		break;
	case NMS_SERVER_PASSWORD:	OnNMSServerPassword(message, client);	break;
	case NMS_SERVER_CHAT:		OnNMSServerChat(message, client);		break;

	case NMS_PLAYER_NAME:			OnNMSPlayerName(message, client);			break;
	case NMS_PLAYER_TOGGLENATION:	OnNMSPlayerToggleNation(message, client);	break;
	case NMS_PLAYER_TOGGLETEAM:		OnNMSPlayerToggleTeam(message, client);		break;
	case NMS_PLAYER_TOGGLECOLOR:	OnNMSPlayerToggleColor(message, client);	break;
	case NMS_PLAYER_READY:			OnNMSPlayerReady(message, client);			break;

	case NMS_MAP_INFO:		OnNMSMapInfo(message, client);		break;
	case NMS_MAP_DATA:		OnNMSMapData(message, client);		break;
	case NMS_MAP_CHECKSUM:	OnNMSMapChecksum(message, client);	break;

	case NMS_DEAD_MSG:	break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// führt eine ingame-nachricht aus
void GameServer::HandleGameMessage(GameMessage *message, unsigned char client)
{
	if(!message)
		return;

	switch(message->m_usID)
	{
	default:			break;
	case NMS_NULL_MSG:	break;

	case NMS_SERVER_CHAT:	OnNMSServerChat(message, client);	break;

	case NMS_PONG: OnNMSPong(message, client); break;

	case NMS_NFC_COMMANDS:	OnNMSNfcCommand(message, client);	break;

	case NMS_DEAD_MSG:	break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// pongnachricht
inline void GameServer::OnNMSPong(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != 0)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	unsigned int currenttime = VideoDriverWrapper::inst().GetTickCount();

	player->ping = (unsigned short)(currenttime - player->lastping);
	player->pinging = false;
	player->lastping = currenttime;

	GameMessage msg(NMS_PLAYER_PING);
	msg.m_uiLength = 3;
	msg.alloc();

	char *data = (char*)msg.m_pData;
	data[0] = (unsigned char)client;
	unsigned short *ping = (unsigned short*)&data[1];

	*ping = player->ping;

	SendToAll(msg);
	//LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_PING(%d,%u)\n", client, *ping);
}

///////////////////////////////////////////////////////////////////////////////
// servertype
inline void GameServer::OnNMSServerType(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength <= 1)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	unsigned char typ = *(unsigned char *)message->m_pData;
	char *version = &((char *)message->m_pData)[1];

	int typok = 0;
	if(typ != serverconfig.servertype)
		typok = 1;
	else if(strcmp(GetWindowVersion(), version) != 0)
		typok = 2;

	LOG.write("CLIENT%d >>> SERVER: NMS_SERVER_TYP(%d, %s) expected: %d, %s, ok: %s\n", client, typ, GetWindowVersion(), serverconfig.servertype, version, (typok == 0) ? "yes" : "no");

	// Antwort senden
	GameMessage *msg = player->send_queue.push(NMS_SERVER_TYP);
	msg->m_uiLength = 4;
	msg->alloc(&typok);

	LOG.write("SERVER >>> CLIENT%d: NMS_SERVER_TYP(%d)\n", client, typok);

	if(typok != 0)
		KickPlayer(client, NP_CONNECTIONLOST, 0);
}

///////////////////////////////////////////////////////////////////////////////
// Serverpasswort
inline void GameServer::OnNMSServerPassword(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;

	bool pwok = false;

	if( (message->m_uiLength == 0) && (serverconfig.password.length() == 0) )
		pwok = true;

	if( (pwok == false) && (message->m_uiLength > 0) )
	{
		char *pw = (char *)message->m_pData;

		if(pw == serverconfig.password)
			pwok = true;
	}

	LOG.write("CLIENT%d >>> SERVER: NMS_SERVER_PASSWORD(%s) expected: %s, ok: %s\n", client, (char*)message->m_pData, serverconfig.password.c_str(), pwok ? "yes" : "no");

	// Antwort senden
	GameMessage *msg = player->send_queue.push(NMS_SERVER_PASSWORD);
	msg->m_uiLength = 1;
	msg->alloc(&pwok);

	LOG.write("SERVER >>> CLIENT%d: NMS_SERVER_PASSWORD(%d)\n", client, pwok);

	if(!pwok)
		KickPlayer(client, NP_WRONGPASSWORD, 0);
}

///////////////////////////////////////////////////////////////////////////////
// Chat
inline void GameServer::OnNMSServerChat(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength < 2)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	unsigned char *destination = (unsigned char*)message->m_pData;
	char *text = &((char*)message->m_pData)[1];

	LOG.write("CLIENT%d >>> SERVER: NMS_SERVER_CHAT(%u, %s)\n", client, *destination, text);

	// +1 für Player-ID!
	GameMessage answer(NMS_SERVER_CHAT,message->m_uiLength+1);
	static_cast<unsigned char*>(answer.m_pData)[0] = client;
	memcpy(static_cast<unsigned char*>(answer.m_pData)+1,message->m_pData,message->m_uiLength);

	switch(*destination)
	{
	default:
	case CD_ALL: // Alle
		{
			SendToAll(answer);

			LOG.write("SERVER >>> BROADCAST: NMS_SERVER_CHAT(%u, %s)\n", *destination, text);
		} break;
	case CD_ALLIES: // Verbündete
		{
			// Besiegte dürfen nicht mehr heimlich mit Verbündeten reden
			if(!players[client]->IsDefeated())
			{
				for(unsigned char c = 0; c < serverconfig.playercount; ++c)
				{
					GameServerPlayer *p = players[c];
					if(p->ps == PS_OCCUPIED && p->team == player->team)
						p->send_queue.push(answer);
				}
			}

			LOG.write("SERVER >>> TEAMCAST: NMS_SERVER_CHAT(%u, %s)\n", *destination, text);
		} break;
	case CD_ENEMIES: // Feinde
		{
			// Besiegte dürfen nicht mehr heimlich mit Feinden reden
			if(!players[client]->IsDefeated())
			{
				for(unsigned char c = 0; c < serverconfig.playercount; ++c)
				{
					GameServerPlayer *p = players[c];
					// Nur an Feinde aber auch an sich selber senden, damit man sieht, was man geschrieben hat
					if( p->ps == PS_OCCUPIED && ( p->team != player->team || c == client ) )
						p->send_queue.push(answer);
				}
			}

			LOG.write("SERVER >>> ENEMYCAST: NMS_SERVER_CHAT(%u, %s)\n", *destination, text);
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Spielername
inline void GameServer::OnNMSPlayerName(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength == 0)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	char *name = (char *)message->m_pData;

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_NAME(%s)\n", client, name);

	player->name = name;

	// Als Antwort Kartenname übertragen
	GameMessage *msg = player->send_queue.push(NMS_MAP_NAME);
	msg->m_uiLength = unsigned(mapinfo.name.length()) + 1;
	msg->alloc(mapinfo.name.c_str());

	LOG.write("SERVER >>> CLIENT%d: NMS_MAP_NAME(%s)\n", client, mapinfo.name.c_str());
}


///////////////////////////////////////////////////////////////////////////////
// Nation weiterwechseln
inline void GameServer::OnNMSPlayerToggleNation(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != 0)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	player->nation = Nation((unsigned(player->nation) + 1) % 4);

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_TOGGLENATION\n", client);

	// Nation-Change senden
	char data[2] = { client, player->nation };
	GameMessage togglenation(NMS_PLAYER_TOGGLENATION);
	togglenation.m_uiLength = 2;
	togglenation.alloc(data);

	SendToAll(togglenation);

	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_TOGGLENATION(%d, %d)\n", client, player->nation);
}

///////////////////////////////////////////////////////////////////////////////
// Team weiterwechseln
inline void GameServer::OnNMSPlayerToggleTeam(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != 0)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	player->team = (player->team + 1) % 6;

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_TOGGLETEAM\n", client);

	// Nation-Change senden
	char data[2] = { client, player->team };
	GameMessage toggleteam(NMS_PLAYER_TOGGLETEAM);
	toggleteam.m_uiLength = 2;
	toggleteam.alloc(data);

	SendToAll(toggleteam);

	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_TOGGLETEAM(%d, %d)\n", client, player->team);
}

///////////////////////////////////////////////////////////////////////////////
// Farbe weiterwechseln
inline void GameServer::OnNMSPlayerToggleColor(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != 0)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	// ist die farbe auch frei, wenn nicht, "überspringen"?
	bool reserved_colors[GAME_COLORS_COUNT];
	memset(reserved_colors, 0, sizeof(bool) * GAME_COLORS_COUNT);

	for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
	{
		GameServerPlayer *ki = players[cl];

		if( (player != ki) && ( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) ) )
			reserved_colors[ki->color] = true;
	}
	do {
		player->color = (player->color + 1) % GAME_COLORS_COUNT;
	} while(reserved_colors[player->color]);

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_TOGGLECOLOR\n", client);

	// Nation-Change senden
	char data[2] = { client, player->color };
	GameMessage togglecolor(NMS_PLAYER_TOGGLECOLOR);
	togglecolor.m_uiLength = 2;
	togglecolor.alloc(data);

	SendToAll(togglecolor);

	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_TOGGLECOLOR(%d, %d)\n", client, player->color);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Spielerstatus wechseln
 *
 *  @author FloSoft
 */
inline void GameServer::OnNMSPlayerReady(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != 1)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	player->ready = (*((char*)message->m_pData) == 1) ? true : false;

	LOG.write("CLIENT%d >>> SERVER: NMS_PLAYER_READY(%s)\n", client, (player->ready ? "true" : "false"));

	// Ready-Change senden
	char data[2] = { client, player->ready ? 1 : 0 };
	GameMessage changeready(NMS_PLAYER_READY);
	changeready.m_uiLength = 2;
	changeready.alloc(data);

	SendToAll(changeready);

	LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_READY(%d, %s)\n", client, (player->ready ? "true" : "false"));
}

///////////////////////////////////////////////////////////////////////////////
// Karteninfo
inline void GameServer::OnNMSMapInfo(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != 0)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}
	LOG.write("CLIENT%d >>> SERVER: NMS_MAP_INFO\n", client);

	GameMessage *msg = player->send_queue.push(NMS_MAP_INFO);
	msg->m_uiLength = 16;
	msg->alloc();

	unsigned int *info = (unsigned int *)msg->m_pData;
	info[0] = mapinfo.map_type;
	info[1] = mapinfo.partcount;
	info[2] = mapinfo.ziplength;
	info[3] = mapinfo.length;

	player->temp_ul = 0;

	LOG.write("SERVER >>> CLIENT%d: NMS_MAP_INFO(%u, %u, %u)\n", client, info[0], info[1], info[2]);
}

///////////////////////////////////////////////////////////////////////////////
// Kartenteile
inline void GameServer::OnNMSMapData(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != 0)
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}
	LOG.write("CLIENT%d >>> SERVER: NMS_MAP_DATA\n", client);

	GameMessage *msg = player->send_queue.push(NMS_MAP_DATA);

	msg->m_uiLength = ( ( (mapinfo.ziplength - player->temp_ul) > 4096 ) ? 4096 : (mapinfo.ziplength - player->temp_ul) );
	msg->alloc(&mapinfo.zipdata[player->temp_ul]);

	player->temp_ul += msg->m_uiLength;

	LOG.write("SERVER >>> CLIENT%d: NMS_MAP_DATA(%u)\n", client, msg->m_uiLength);
}

///////////////////////////////////////////////////////////////////////////////
// Checksumme
inline void GameServer::OnNMSMapChecksum(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength != sizeof(unsigned int))
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	unsigned int checksum = *(unsigned int*)message->m_pData;
	bool checksumok = false;

	if(checksum == mapinfo.checksum)
		checksumok = true;

	LOG.write("CLIENT%d >>> SERVER: NMS_MAP_CHECKSUM(%u) expected: %u, ok: %s\n", client, checksum, mapinfo.checksum, checksumok ? "yes" : "no");

	// Antwort senden
	GameMessage *msg = player->send_queue.push(NMS_MAP_CHECKSUM);
	msg->m_uiLength = 1;
	msg->alloc(&checksumok);

	LOG.write("SERVER >>> CLIENT%d: NMS_MAP_CHECKSUM(%d)\n", client, checksumok);

	if(!checksumok)
		KickPlayer(client, NP_WRONGCHECKSUM, 0);
	else
	{
		// den anderen Spielern mitteilen das wir einen neuen haben
		GameMessage playernew(NMS_PLAYER_NEW);
		playernew.m_uiLength = (unsigned int)1 + player->name.length() + 1;
		playernew.alloc();

		char *buffer = (char*)playernew.m_pData;
		buffer[0] = client;
		strncpy(&buffer[1], player->name.c_str(), player->name.length() + 1);

		SendToAll(playernew);
		LOG.write("SERVER >>> BROADCAST: NMS_PLAYER_NEW(%d, %s)\n", client, player->name.c_str());

		// belegt markieren
		player->ps = PS_OCCUPIED;
		player->lastping = VideoDriverWrapper::inst().GetTickCount();

		// Servername senden
		GameMessage *servername = player->send_queue.push(NMS_SERVER_NAME);
		servername->m_uiLength = unsigned(serverconfig.gamename.length()) + 1;
		servername->alloc(serverconfig.gamename.c_str());

		LOG.write("SERVER >>> CLIENT%d: NMS_SERVER_NAME(%s)\n", client, serverconfig.gamename.c_str());

		// freie farbe suchen lassen
		bool reserved_colors[GAME_COLORS_COUNT];
		memset(reserved_colors, 0, sizeof(bool) * GAME_COLORS_COUNT);

		unsigned char rc = 0;
		for(unsigned char cl = 0; cl < serverconfig.playercount; ++cl)
		{
			GameServerPlayer *ki = players[cl];

			if( (player != ki) && ( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) ) )
				reserved_colors[ki->color] = true;
			if( (ki->ps == PS_OCCUPIED) || (ki->ps == PS_KI) )
				++rc;
		}
		if(serverconfig.servertype == NP_DEPENDENT)
			LOBBYCLIENT.UpdateServerPlayerCount(rc, serverconfig.playercount);

		// Spielerliste senden
		GameMessage *playerlist = player->send_queue.push(NMS_PLAYER_LIST);
		playerlist->m_uiLength = 1 + serverconfig.playercount * 15;
		for(unsigned char i = 0; i < serverconfig.playercount; ++i)
			playerlist->m_uiLength += (unsigned(players[i]->name.length())+unsigned(players[i]->origin_name.length())+2);

		playerlist->alloc();

		char *data = (char*)playerlist->m_pData;
		unsigned char *count = (unsigned char*)&data[0];
		//GamePlayerInfo *list = (c*)&data[1];

		*count = serverconfig.playercount;

		// GamePlayerInfo als Data direkt senden, sonst kommt mist raus!
		int j = 1;
		for(unsigned char i = 0; i < *count; ++i)
		{
			assert(players[i]->name.length() < 254);
			data[j] = static_cast<unsigned char>(players[i]->name.length()+1);
			assert(players[i]->name.length() < 254);
			data[j+1] = static_cast<unsigned char>(players[i]->origin_name.length()+1);
			j+=2;

			memcpy(&data[j], players[i]->name.c_str(), players[i]->name.length()+1);
			j+=int(players[i]->name.length())+1;
			memcpy(&data[j], players[i]->origin_name.c_str(), players[i]->origin_name.length()+1);
			j+=int(players[i]->origin_name.length())+1;
			
			memcpy(&data[j], &players[i]->ps, 4);
			memcpy(&data[j+4], &players[i]->rating, 2);
			data[j+6] = players[i]->is_host?1:0;
			memcpy(&data[j+7], &players[i]->ping, 2);
			memcpy(&data[j+9], &players[i]->nation, 1);
			memcpy(&data[j+10], &players[i]->color, 1);
			memcpy(&data[j+11], &players[i]->team, 1);
			data[j+12] = players[i]->ready ? 1 : 0;

			j+=13;

			LOG.write("    %d: %s %d %d %d %d %d %d\n", i, players[i]->name.c_str(), players[i]->ps, players[i]->rating, players[i]->ping, players[i]->nation, players[i]->color, players[i]->team);
		}
		LOG.write("SERVER >>> CLIENT%d: NMS_PLAYER_LIST\n", client);

		// bis wir eine freie farbe gefunden haben!
		// (while-schleife obwohl OnNMSPlayerToggleColor selbst schon eine freie sucht)
		while(reserved_colors[player->color])
		{
			GameMessage msg(NMS_PLAYER_TOGGLECOLOR);
			OnNMSPlayerToggleColor(&msg, client);
		}

		// GGS senden
		GameMessage msg(NMS_GGS_CHANGE);
		// Einstellungen serialisieren
		unsigned char *ggs_buffer = ggs.Serialize();

		msg.m_uiLength = GlobalGameSettings::GGS_BUFFER_SIZE;
		msg.alloc(ggs_buffer);

		delete[] ggs_buffer;

		// und verschicken
		player->send_queue.push(msg);

		LOG.write("SERVER >>> BROADCAST: NMS_GGS_CHANGE\n");
	}
}

//////////////////////////////////////////////////////////////////////////////
// NFC-Kommando
inline void GameServer::OnNMSNfcCommand(GameMessage *message, unsigned char client)
{
	GameServerPlayer *player = players[client];

	if(!message)
		return;
	if(message->m_uiLength < sizeof(unsigned char))
	{
		KickPlayer(client, NP_INCOMPLETEMSG, 0);
		return;
	}

	LOG.write("SERVER <<< NMS_NFC_COMMAND\n");

	// NFCs speichern
	player->nfc_queue.push_back(new GameMessage(*message));

	//// Command schließlich an alle Clients weiterleiten, aber nicht in der Pause und nicht, wenn derjenige Spieler besiegt wurde!
	if(!this->framesinfo.pause && !players[client]->IsDefeated())
	{
		GameMessage answer(NMS_NFC_ANSWER,message->m_uiLength + 1);
		static_cast<unsigned char*>(answer.m_pData)[0] = client;
		memcpy(&static_cast<unsigned char*>(answer.m_pData)[1],message->m_pData,message->m_uiLength);
		SendToAll(answer);
	}
	else
	{
		// In der Pause leeres Command senden
		GameMessage answer(NMS_NFC_ANSWER,6);
		static_cast<unsigned char*>(answer.m_pData)[0] = client;
		// Checksumme kopieren
		memcpy(&static_cast<unsigned char*>(answer.m_pData)[1],message->m_pData,4);
		// 0 NCs
		static_cast<unsigned char*>(answer.m_pData)[5] = 0;
		SendToAll(answer);
	}
}

void GameServer::ChangePlayer(const unsigned char old_id, const unsigned char new_id)
{
	players[old_id]->ps = PS_KI;
	players[new_id]->ps = PS_OCCUPIED;
	players[new_id]->so = players[old_id]->so;
}

bool GameServer::TogglePause()
{
	framesinfo.pause = !framesinfo.pause;

	GameMessage pause(NMS_NFC_PAUSE, 1);

	LOG.write("SERVER >>> NMS_NFC_PAUSE\n");

	static_cast<char*>(pause.m_pData)[0] = (char)framesinfo.pause;
	SendToAll(pause);

	return framesinfo.pause;
}


void GameServer::SwapPlayer(const unsigned char player1, const unsigned char player2)
{
	// Message verschicken
	GameMessage swap_player(NMS_PLAYER_SWAP,2);
	static_cast<unsigned char*>(swap_player.m_pData)[0] = player1;
	static_cast<unsigned char*>(swap_player.m_pData)[1] = player2;

	SendToAll(swap_player);

	// Spieler vertauschen
	players[player1]->SwapPlayer(*players[player2]);
}

void GameServer::SetKIName(const unsigned player_id)
{
	// Baby mit einem Namen Taufen ("Name (KI)")
	char str[128];
	sprintf(str,_("Computer %u"),unsigned(player_id));
	players[player_id]->name = str;
	players[player_id]->name += _(" (AI)");
}
