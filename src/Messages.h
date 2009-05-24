// $Id: Messages.h 4933 2009-05-24 12:29:23Z OLiver $
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
#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
/**
 *  Nachrichten des Fenstersystems
 *
 *  MSG_* Fenster Nachrichten
 *  CNM_* Netzwerk Nachrichten
 *  TBL_* Tabellen Nachrichten
 *
 *  @author OLiver
 */
enum Msg
{
	// Client-Netzwerk-Messages ( die kommen VOM Client und werden an einen festgelegten Messagehandler gesendet )
	CNM_NEWPLAYER,
	CNM_PLAYERLEAVE,
	CNM_PSCHANGE,
	CNM_NATIONCHANGE,
	CNM_COLORCHANGE,
	CNM_TEAMCHANGE,
	CNM_PINGCHANGE,
	CNM_READYCHANGE,
	CNM_SWAPPLAYER,
	CNM_CHAT,
	CNM_GGSCHANGE,
	CNM_COS_WAITFORANSWER,
	CNM_COS_QUERYPW,
	CNM_COS_QUERYMAPNAME,
	CNM_COS_QUERYPLAYERLIST,
	CNM_COS_FINISHED,
	CNM_COS_REGISTERED,
	CNM_COS_LOGGEDIN,
	CNM_COS_SERVERCREATED,
	CNM_ERR_INCOMPLETEMESSAGE,
	CNM_ERR_SERVERFULL,
	CNM_ERR_WRONGPW,
	CNM_ERR_CONNECTIONLOST,
	CNM_ERR_INVALIDSERVERTYPE,
	CNM_ERR_WRONGMAP,
	CNM_ERR_WRONGVERSION,
	CNM_ERR_LOBBYFULL,
	CNM_ERROR
};


 ctrl,shift,alt;
};


#endif // !MESSAGES_H_INCLUDED
