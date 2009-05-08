// $Id: Messenger.h 4834 2009-05-08 19:45:33Z FloSoft $
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

#ifndef MESSENGER_H_
#define MESSENGER_H_

#include "GlobalVars.h"
#include "colors.h"
#include "GameProtocol.h"

class Messenger
{
	struct Msg
	{
		std::string author;
		unsigned color_author;
		ChatDestination cd;
		std::string msg;
		unsigned color_msg;
		unsigned starttime;
		unsigned short width;
	};
	list<Msg> messages;

private:
	void PrintTermColor(const std::string& text, const unsigned color_text);

public:

	~Messenger();

	void Draw();
	void AddMessage(const std::string& author, const unsigned color_author, const ChatDestination cd, const std::string& msg,const unsigned color_msg = COLOR_YELLOW);
};

#endif
