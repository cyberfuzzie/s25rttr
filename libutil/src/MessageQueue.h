// $Id: MessageQueue.h 4984 2009-06-01 10:37:37Z OLiver $
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
#ifndef MESSAGEQUEUE_H_INCLUDED
#define MESSAGEQUEUE_H_INCLUDED

//class Socket;
class MessageQueue
{
public:
	MessageQueue(Message *(*createfunction)(unsigned short)) : createfunction(createfunction) {}
	MessageQueue(const MessageQueue& mq);
	~MessageQueue(void);

	MessageQueue& operator=(const MessageQueue& mq);

private:
	typedef std::vector<Message *> Queue;
	typedef std::vector<Message *>::iterator QueueIt;
	Queue messages;
	Message *(*createfunction)(unsigned short);

public:
	void clear(void);

	/// flusht die Queue, verschickt alle Elemente.
	bool flush(Socket *sock) { return send(sock, messages.size(), 0xFFFFFFFF); }

	/// liefert die Größe der Queue
	unsigned int count() { return messages.size(); }

	/// verschickt Pakete der Queue, maximal @p max, mit einem maximal @p sizelimit groß (aber beliebig viele kleine)
	bool send(Socket *sock, int max, unsigned int sizelimit = 512);
	bool recv(Socket *sock, bool wait = false);

public:
	/// hängt ein Element hinten an.
	void push(Message *message) { messages.push_back(message); }
	/// liefert das vorderste Element der Queue.
	Message *front() { return (messages.size() > 0 ? (*messages.begin()) : NULL); }
	/// entfernt das vorderste Element aus der Queue.
	void pop(void);
	
};

#endif // LOBBYMESSAGEQUEUE_H_INCLUDED
