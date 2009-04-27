// $Id: GameProtocol.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef GAMEPROTOCOL_H_INCLUDED
#define GAMEPROTOCOL_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Netzwerk Messages                       // client> | <server
enum
{
	NMS_NULL_MSG = 0x0000, // 0

	NMS_PING, // 0
	NMS_PONG, // 0

	NMS_SERVER_TYP = 0x0101, // 1 servertyp, x server-version
	NMS_SERVER_PASSWORD, // x serverpassword | 1 serverpasswordok
	NMS_SERVER_NAME, // x servername
	NMS_SERVER_START, // 
	NMS_SERVER_CHAT, // 1 destination, x text | 1 source, x text
	NMS_SERVER_ASYNC, // playercount*4 int (Checksummen)

	NMS_PLAYER_ID = 0x0201, // 1 playerid
	NMS_PLAYER_NAME, // x playername
	NMS_PLAYER_LIST, // 1 playercount | x GamePlayerInfo
	NMS_PLAYER_TOGGLESTATE, // 1 playerid
	NMS_PLAYER_TOGGLENATION, // 0 | 1 playerid
	NMS_PLAYER_TOGGLETEAM, // 0 | 1 playerid
	NMS_PLAYER_TOGGLECOLOR, // 0 | 1 playerid
	NMS_PLAYER_KICKED, // 12 npk
	NMS_PLAYER_PING, // 1 playerid, 2 playerping
	NMS_PLAYER_NEW, // 1 playerid, x playername
	NMS_PLAYER_READY, // 1 status | 1 playerid, 1 status
	NMS_PLAYER_SWAP, // 1 playerid1, 1 playerid2

	NMS_MAP_NAME = 0x0301, // x mapname
	NMS_MAP_INFO, // 0 | 4 parts, 4 ziplength, 4 length
	NMS_MAP_DATA, // 0 | x mappartdata
	NMS_MAP_CHECKSUM, // 4 checksum | 1 checksumok

	NMS_NFC_DONE = 0x0401, // 0
	NMS_NFC_COMMANDS, // 1 nc-count, (2 nc_type, x nc_data)*nc-count
	NMS_NFC_ANSWER, // 1 playerid, 2 nc_type, x nc_data
	NMS_NFC_PAUSE, // 1 pause (yes/no)

	NMS_GGS_CHANGE = 0x0501, // 

	NMS_DEAD_MSG = 0xFFFF // 0
};

///////////////////////////////////////////////////////////////////////////////
/* Hinweise:

bc = broadcast
ec = broadcast to enemy
tc = broadcast to team

*/
///////////////////////////////////////////////////////////////////////////////
/* Reihenfolge bei Client:

NMS_NULL_MSG			--> ignore

NMS_PING				--> NMS_PONG

NMS_PLAYER_ID			--> ok ? NMS_SERVER_TYP : disconnect
NMS_PLAYER_TOGGLESTATE	-->
NMS_PLAYER_TOGGLENATION --> NMS_PLAYER_TOGGLENATION
NMS_PLAYER_TOGGLETEAM	--> NMS_PLAYER_TOGGLETEAM
NMS_PLAYER_TOGGLECOLOR	--> NMS_PLAYER_TOGGLECOLOR
NMS_PLAYER_KICKED		-->

NMS_SERVER_TYP			--> ok ? NMS_SERVER_PASSWORD : disconnect
NMS_SERVER_PASSWORD		--> ok ? NMS_PLAYER_NAME : disconnect

NMS_MAP_NAME			--> NMS_MAP_INFO
NMS_MAP_INFO			--> parts*NMS_MAP_DATA
NMS_MAP_DATA			--> part >= parts ? NMS_MAP_CHECKSUM
NMS_MAP_CHECKSUM		--> ( !ok ) ? disconnect

NMS_GGS_CHANGE			--> 

NMS_NFC_ANSWER			-->

NMS_DEAD_MSG			--> disconnect

*/
///////////////////////////////////////////////////////////////////////////////
/* Reihenfolge bei Server:

NMS_NULL_MSG			--> ignore

NMS_PONG				--> bc(NMS_PLAYER_PING)

connect					--> NMS_PLAYER_ID

NMS_SERVER_TYP			--> NMS_SERVER_TYP, ( !ok ) ? kick
NMS_SERVER_PASSWORD		--> NMS_SERVER_PASSWORD, ( !ok ) ? kick
NMS_SERVER_CHAT			--> toteam ? tc(NMS_SERVER_CHAT) : toenemy ? ec(NMS_SERVER_CHAT) : bc(NMS_SERVER_CHAT)

NMS_PLAYER_NAME			--> NMS_MAP_NAME
NMS_PLAYER_TOGGLESTATE	--> (playerstate != KI) ? kick(playerid) : bc(NMS_PLAYER_TOGGLESTATE)
NMS_PLAYER_TOGGLENATION --> bc(NMS_PLAYER_TOGGLENATION)
NMS_PLAYER_TOGGLETEAM	--> bc(NMS_PLAYER_TOGGLETEAM)
NMS_PLAYER_TOGGLECOLOR	--> bc(NMS_PLAYER_TOGGLECOLOR)

NMS_MAP_INFO			--> NMS_MAP_INFO
NMS_MAP_DATA			--> NMS_MAP_DATA
NMS_MAP_CHECKSUM		--> NMS_MAP_CHECKSUM, ( !ok ) ? kick : ( bc(NMS_PLAYER_NEW), NMS_SERVER_NAME, NMS_PLAYER_LIST, NMS_GGS_CHANGE )

NMS_NFC_COMMAND			--> NMS_NFC_ANSWER

NMS_DEAD_MSG			--> kick

kick					--> bc(NMS_PLAYER_KICK), NMS_DEAD_MSG

*/
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////
// Strukturen fr einzelne Messages
struct NS_PlayerKicked
{
	unsigned char playerid;
	unsigned char cause;
	unsigned short param;
};

struct NS_StartGameInfo
{
	/// Initialisierungswert für den Zufallsgenerator
	unsigned int random_init;
	/// NWF - Länge in GameFrames
	unsigned nwf_length;
	// ....
};

struct NS_ChatInfo
{
	unsigned char pi;
	unsigned char dest;
};

//////////////////////////////////////////
// Sonstige Konstanten

// Gründe fürs Kicken
enum
{
	NP_NOCAUSE = 0, // Ohne Grund --> manuell vom GameServer rausgehauen, weiß der Teufel warum
	NP_CONNECTIONLOST, // Verbindung verloren/abgebrochen, wie auch immer
	NP_INVALIDMSG, // Unglütige Message, (evtl Cheater bzw. Asynchronität)
	NP_INCOMPLETEMSG, // zu wenig gesendet
	NP_PINGTIMEOUT, // Ping Timeout
	NP_WRONGPASSWORD, // falsches passwort
	NP_WRONGCHECKSUM, // falsche Checksumme
	NP_ASYNC // asynchron
};

// Servertypen
enum
{
	NP_DEPENDENT = 0,
	NP_INDEPENDENT
};

// Größe der Ping Msg ( Anzahl von 0en)
const unsigned PING_SIZE		= 10;

// Wie lange maximal warten, bis Rausschmiss des Spielers
const unsigned PING_TIMEOUT		= 30000;

// Ziele fürs Chatten (Ingame)
enum ChatDestination
{
	CD_SYSTEM = 0,
	CD_ALL	= 1,
	CD_ALLIES,
	CD_ENEMIES
};

// Netzwerk Befehle
enum
{
	 NC_NOTSEND = 0,
	 NC_NOTHING,
	 NC_SETFLAG,
	 NC_DESTROYFLAG,
	 NC_BUILDROAD,
	 NC_DESTROYROAD,
	 NC_CHANGEDISTRIBUTION,
	 NC_CHANGEBUILDORDER,
	 NC_SETBUILDINGSITE,
	 NC_DESTROYBUILDING,
	 NC_CHANGETRANSPORT,
	 NC_CHANGEMILITARY,
	 NC_CHANGETOOLS,
	 NC_CALLGEOLOGIST,
	 NC_CALLSCOUT,
	 NC_ATTACK,
	 NC_SWITCHPLAYER,
	 NC_STOPGOLD,
	 NC_STOPPRODUCTION,
	 NC_CHANGEINVENTORYSETTING,
	 NC_CHANGEALLINVENTORYSETTINGS,
	 NC_INCREASERESERVE,
	 NC_DECREASERESERVE,
	 NC_SURRENDER,
	 NC_CHEAT_ARMAGEDDON
};

// Netzwerkframelängen
const unsigned NFL_200MS = 200;
const unsigned NFL_400MS = 400;


/// Map-Typ
enum MapType
{
	MAPTYPE_OLDMAP,
	MAPTYPE_SAVEGAME,
	MAPTYPE_RTTRMAP,
	MAPTYPE_RANDOMMAP
};

#endif // !GAMEPROTOCOL_H_INCLUDED
