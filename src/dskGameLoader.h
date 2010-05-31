// $Id: dskGameLoader.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef dskGAMELOADER_H_INCLUDED
#define dskGAMELOADER_H_INCLUDED

#pragma once

#include "Desktop.h"

#include "LobbyInterface.h"
#include "ClientInterface.h"

class GameWorldViewer;

class dskGameLoader : 
	public Desktop, 
	public ClientInterface,
	public LobbyInterface
{
public:
	/// Konstruktor von @p dskGameLoader.
	dskGameLoader(GameWorldViewer * gwv);
	/// Destruktor von @p dskGameLoader.
	~dskGameLoader();

	void LC_Status_Error(const std::string &error);

private:
	void Msg_MsgBoxResult(const unsigned int msgbox_id, const MsgboxResult mbr);
	void Msg_Timer(const unsigned int ctrl_id);

	unsigned int position;
	/// Falls ein Savegame geladen wird --> Pointer darauf
	GameWorldViewer *gwv;
};

#endif // !dskGAMELOADER_H_INCLUDED
