// $Id: Message.h 4851 2009-05-10 12:21:04Z FloSoft $
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
#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#pragma once

#include <stdexcept>

class MessageInterface;
class Socket;

class Message
{
public:
	/// Konstruktor von @p Message.
	Message(unsigned short id) : data(NULL), id(id), length(0), index(0) { }
	virtual ~Message();

	virtual unsigned short getId() { return id; }
	virtual unsigned int getSize() { return length; }
	bool send(Socket *sock);

	static Message *recv(Socket *sock, int &error, bool wait, Message *(*createfunction)(unsigned short));
	static Message *create_base(unsigned short id);

	virtual Message *create(unsigned short id) const { return create_base(id); }
	virtual Message *duplicate() const;

	virtual void run(MessageInterface *callback, unsigned int id) { throw std::logic_error("pure virtual call: Message::run(callback, id)"); };

protected:
	/// Erzeugt Speicher mit der Länge @p length.
	void alloc(unsigned int length)
	{
		delete[] data;

		this->length = length;

		data = new char[length];
		memset(data, 0, sizeof(char)*length);
	}

	/// vergrößert/verkleinert den Speicher auf die Länge @p length.
	void ralloc(unsigned int length)
	{
		unsigned int olength = this->length;

		this->length = length;

		// verkleinern?
		if(olength > length)
			olength = length;

		char *ndata = new char[length];
		memcpy(ndata, data, olength);

		delete[] data;
		data = ndata;
	}

	/// int in Message aufnehmen
	inline void pushI(const int &value)
	{
		pushUI((const unsigned int)value);
	}

	/// unsigned int in Message aufnehmen
	inline void pushUI(const unsigned int &value)
	{
		if(index + sizeof(unsigned int) > this->length)
			ralloc(this->length + sizeof(unsigned int));

		*((unsigned int*)&data[index]) = htonl(value);
		index += sizeof(unsigned int);
	}
	
	/// short in Message aufnehmen
	inline void pushS(const short &value)
	{
		pushUS((const unsigned short)value);
	}
	
	/// unsigned short in Message aufnehmen
	inline void pushUS(const unsigned short &value)
	{
		if(index + sizeof(unsigned short) > this->length)
			ralloc(this->length + sizeof(unsigned short));

		*((unsigned short*)&data[index]) = htons(value);
		index += sizeof(unsigned short);
	}

	/// char in Message aufnehmen
	inline void pushC(const char *value, const unsigned int &length)
	{
		if(index + length * sizeof(unsigned char) > this->length)
			ralloc(this->length + length * sizeof(unsigned char));

		memcpy(&data[index], value, length * sizeof(unsigned char));
		index += length * sizeof(unsigned char);
	}

	/// unsigned char in Message aufnehmen
	inline void pushUC(const unsigned char *value, const unsigned int &length)
	{
		pushC((const char*)value, length);
	}

	/// std::string in Message aufnehmen
	inline void pushStr(const std::string &value)
	{
		unsigned int length = value.length();

		if(index + length + 1 > this->length)
			ralloc(this->length + length + 1);

		strncpy(&data[index], value.c_str(), length);
		index += length + 1;
	}

	/// int aus Message lesen
	inline int popI(void)
	{
		return popUI();
	}

	/// unsigned int aus Message lesen
	inline unsigned int popUI(void)
	{
		unsigned int value;

		value = htonl(*((unsigned int*)&data[index]));
		index += sizeof(unsigned int);

		return value;
	}

	/// short aus Message lesen
	inline short popS(void)
	{
		return popUS();
	}

	/// unsigned short aus Message lesen
	inline unsigned short popUS(void)
	{
		unsigned short value;

		value = htons(*((unsigned short*)&data[index]));
		index += sizeof(unsigned short);

		return value;
	}

	/// char* aus Message lesen
	inline const char *popC(char *value, const unsigned int &length)
	{
		memcpy(value, &data[index], length * sizeof(unsigned char));
		index += length * sizeof(unsigned char);

		return value;
	}

	/// unsigned char* aus Message aufnehmen
	inline const unsigned char *popUC(unsigned char *value, const unsigned int &length)
	{
		return (const unsigned char*)popC((char*)value, length);
	}

	/// std::string aus Message lesen
	inline std::string popStr(void)
	{
		std::string value;

		value = &data[index];
		index += value.length() + 1;

		return value;
	}

private:
	Message(void) {}
	Message(const Message &) {}
	Message& operator = (const Message&) { return *this; }

	int recv(Socket *sock, unsigned int length);

protected:
	char *data;

private:
	unsigned short id;
	unsigned int length;
	unsigned int index;
};

#endif //!MESSAGE_H_INCLUDED
