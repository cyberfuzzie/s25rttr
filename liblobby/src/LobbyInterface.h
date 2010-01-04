// $Id: LobbyInterface.h 5853 2010-01-04 16:14:16Z FloSoft $
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
#ifndef LOBBYINTERFACE_H_INCLUDED
#define LOBBYINTERFACE_H_INCLUDED

#pragma once

#include "LobbyProtocol.h"

class LobbyInterface
{
public:
	virtual ~LobbyInterface() {}

	virtual void LC_LoggedIn(const std::string &email);
	virtual void LC_Registered(void);
	virtual void LC_Connected(void);
	virtual void LC_Created(void);

	virtual void LC_Status_ConnectionLost(void);
	virtual void LC_Status_IncompleteMessage(void);
	virtual void LC_Status_Waiting(void);
	virtual void LC_Status_Error(const std::string &error);

	virtual void LC_Chat(const std::string &player, const std::string &text);
};

#endif //!LOBBYINTERFACE_H_INCLUDED
