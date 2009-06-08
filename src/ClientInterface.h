// $Id: ClientInterface.h 5018 2009-06-08 18:24:25Z OLiver $
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
#ifndef CLIENTGUIINTERFACE_H_
#define CLIENTGUIINTERFACE_H_

#pragma once

#include "GameProtocol.h"
#include "GamePlayerInfo.h"

class GameWorldViewer;
class GlobalGameSettings;

/// Verbindungsstatus beim Verbinden zum Server
enum ConnectState
{
	CS_WAITFORANSWER = 0,
	CS_QUERYPW,
	CS_QUERYMAPNAME,
	CS_QUERYPLAYERLIST,
	CS_FINISHED,
	CS_REGISTERED,
	CS_LOGGEDIN,
	CS_SERVERCREATED
};

/// Fehler, die vom Client gemeldet werden
enum ClientError
{
	CE_INCOMPLETEMESSAGE,
	CE_SERVERFULL,
	CE_WRONGPW,
	CE_CONNECTIONLOST,
	CE_INVALIDSERVERTYPE,
	CE_WRONGMAP,
	CE_WRONGVERSION,
	CE_LOBBYFULL
};



class ClientInterface
{
public:

	virtual ~ClientInterface() { }

	virtual void CI_NextConnectState(const ConnectState cs);
	virtual void CI_Error(const ClientError ce);

	virtual void CI_NewPlayer(const unsigned player_id);
	virtual void CI_PlayerLeft(const unsigned player_id);

	virtual void CI_GameStarted(GameWorldViewer *gwv);

	virtual void CI_PSChanged(const unsigned player_id, const PlayerState ps);
	virtual void CI_NationChanged(const unsigned player_id, const Nation nation);
	virtual void CI_ColorChanged(const unsigned player_id, const unsigned char color);
	virtual void CI_TeamChanged(const unsigned player_id, const unsigned char team);
	virtual void CI_PingChanged(const unsigned player_id, const unsigned short ping);
	virtual void CI_ReadyChanged(const unsigned player_id, const bool ready);
	virtual void CI_PlayersSwapped(const unsigned player1, const unsigned player2);
	virtual void CI_GGSChanged(const GlobalGameSettings& ggs);

	virtual void CI_Chat(const unsigned player_id, const ChatDestination cd, const std::string& msg);

	virtual void CI_NewPostMessage(const unsigned postmessages_count);
	virtual void CI_PostMessageDeleted(const unsigned postmessages_count);

	virtual void CI_Async(const std::string& checksums_list);
	virtual void CI_ReplayAsync(const std::string& msg);
	virtual void CI_ReplayEndReached(const std::string& msg);
	virtual void CI_GamePaused();
	virtual void CI_GameResumed();
	virtual void CI_FlagDestroyed(const unsigned short x, const unsigned short y);



	///*CNM_NEWPLAYER,
	//CNM_PLAYERLEAVE,
	//CNM_PSCHANGE,
	//CNM_NATIONCHANGE,
	//CNM_COLORCHANGE,
	//CNM_TEAMCHANGE,
	//CNM_PINGCHANGE,
	//CNM_READYCHANGE,
	//CNM_SWAPPLAYER,
	//CNM_CHAT,
	//CNM_GGSCHANGE,
	//CNM_COS_WAITFORANSWER,
	//CNM_COS_QUERYPW,
	//CNM_COS_QUERYMAPNAME,
	//CNM_COS_QUERYPLAYERLIST,
	//CNM_COS_FINISHED,
	//CNM_COS_REGISTERED,
	//CNM_COS_LOGGEDIN,
	//CNM_COS_SERVERCREATED,
	//CNM_ERR_INCOMPLETEMESSAGE,
	//CNM_ERR_SERVERFULL,
	//CNM_ERR_WRONGPW,
	//CNM_ERR_CONNECTIONLOST,
	//CNM_ERR_INVALIDSERVERTYPE,
	//CNM_ERR_WRONGMAP,
	//CNM_ERR_WRONGVERSION,
	//CNM_ERR_LOBBYFULL,
	//CNM_ERROR*/
};


#endif
