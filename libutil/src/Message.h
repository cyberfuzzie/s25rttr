// $Id: Message.h 4903 2009-05-21 12:04:21Z OLiver $
//
// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
//a
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
#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#pragma once

#include <stdexcept>

#include "Serializer.h"

class MessageInterface;
class Socket;

class Message : public Serializer
{
public:
	/// Konstruktor von @p Message.
	Message(unsigned short id) : id(id) {}
	virtual ~Message();

	virtual unsigned short getId() const { return id; }
	bool send(Socket *sock);

	static Message *recv(Socket *sock, int &error, bool wait, Message *(*createfunction)(unsigned short));
	static Message *create_base(unsigned short id);

	virtual Message *create(unsigned short id) const { return create_base(id); }
	virtual Message *duplicate() const;

	virtual void run(MessageInterface *callback, unsigned int id) { throw std::logic_error("pure virtual call: Message::run(callback, id)"); };

private:
	Message(void) {}
	Message(const Message &) {}
	Message& operator = (const Message&) { return *this; }

	int recv(Socket *sock, unsigned int length);

protected:
	unsigned short id;
};

#endif //!MESSAGE_H_INCLUDED
