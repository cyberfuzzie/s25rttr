// $Id: dskLobby.h 6458 2010-05-31 11:38:51Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef dskLOBBY_H_INCLUDED
#define dskLOBBY_H_INCLUDED

#pragma once

#include "Desktop.h"

#include "LobbyInterface.h"
#include "ClientInterface.h"

class iwLobbyServerInfo;
class iwDirectIPCreate;
class LobbyServerList;
class LobbyPlayerList;

class dskLobby : 
	public Desktop, 
	public ClientInterface,
	public LobbyInterface
{
private:
	const LobbyServerList *serverlist;
	const LobbyPlayerList *playerlist;
	iwLobbyServerInfo *serverinfo;
	iwDirectIPCreate *servercreate;

public:
	dskLobby();

	void UpdatePlayerList(bool first = false);
	void UpdateServerList(bool first = false);

	void LC_Connected(void);

	void LC_Status_ConnectionLost(void);
	void LC_Status_IncompleteMessage(void);
	void LC_Status_Error(const std::string &error);

	void LC_Chat(const std::string &player, const std::string &text);

protected:
	void Msg_Timer(const unsigned int ctrl_id);
	void Msg_PaintBefore();
	void Msg_MsgBoxResult(const unsigned msgbox_id, const MsgboxResult mbr);
	void Msg_ButtonClick(const unsigned int ctrl_id);
	void Msg_EditEnter(const unsigned int ctrl_id);
	void Msg_TableRightButton(const unsigned int ctrl_id, const unsigned short selection);
};

#endif // dskLOBBY_H_INCLUDED
