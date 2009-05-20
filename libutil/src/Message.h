// $Id: Message.h 4898 2009-05-20 13:44:45Z OLiver $
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
	Message(unsigned short id) : id(id), length(0), data(NULL), index(0) { }
	virtual ~Message();

	virtual unsigned short getId() const { return id; }
	virtual unsigned int getSize() const { return length; }
	bool send(Socket *sock);

	static Message *recv(Socket *sock, int &error, bool wait, Message *(*createfunction)(unsigned short));
	static Message *create_base(unsigned short id);

	virtual Message *create(unsigned short id) const { return create_base(id); }
	virtual Message *duplicate() const;

	virtual void run(MessageInterface *callback, unsigned int id) { throw std::logic_error("pure virtual call: Message::run(callback, id)"); };

private:
	/// vergrößert den Speicher auf die nächst höhere 2er potenz zur Länge @p length.
	inline void ralloc(const unsigned int length)
	{
		if(this->length == 0)
			this->length = 64;

		// speicher vergrößern
		while(this->length < length)
			this->length *= 2;

		if(data == NULL)
		{
			// neu anlegen
			data = new unsigned char[this->length];
			memset(data, 0, sizeof(unsigned char)*this->length);
		}
		else
		{
			// umkopieren (vergrößern)
			unsigned char *ndata = new unsigned char[this->length];
			memcpy(ndata, data, index);

			delete[] data;
			data = ndata;
		}
	}

public:
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

	/// unsigned char in Message aufnehmen
	inline void pushUC(const unsigned char value)
	{
		pushUC(&value,1);
	}

	/// bool in Message aufnehmen
	inline void pushB(const bool &value)
	{
		pushUC(value ? 1 : 0);
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

		strncpy(&((char*)data)[index], value.c_str(), length);
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

	/// unsigned char in Message aufnehmen
	inline unsigned char popUC()
	{
		unsigned char value;
		popUC(&value, 1);
		return value;
	}

	/// bool in Message aufnehmen
	inline bool popB()
	{
		return ((popUC() == 1) ? true : false);
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

		value = (char*)&data[index];
		index += value.length() + 1;

		return value;
	}

private:
	Message(void) {}
	Message(const Message &) {}
	Message& operator = (const Message&) { return *this; }

	int recv(Socket *sock, unsigned int length);

protected:
	unsigned short id;
	unsigned int length;
	unsigned char *data;
	unsigned int index;
};

#endif //!MESSAGE_H_INCLUDED
