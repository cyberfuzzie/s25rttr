// $Id: MessageQueue.h 4910 2009-05-21 19:38:04Z OLiver $
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
#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED

#pragma once

#include <memory.h>
#include <string>
#include <assert.h>

/// Klasse die einen Buffer zum Serialisieren verwaltet und entsprechende Methoden zum Lesen/Schreiben bereitstellt.
class Serializer
{
public:
	Serializer();
	/// Fill with initial data
	// Pushes will push to the end, 
	// while Pops will pop from the beginning
	Serializer(const void * const data, const unsigned initial_size);
	Serializer(const Serializer& two);
	virtual ~Serializer();

	/// Aufräumen
	void Clear();

	/// Getter
	unsigned GetLength() const { return length; }
	/// Zugriff auf die Rohdaten
	const unsigned char * GetData() const { return data; }

	/// Kopiermethoden

	/// Rohdaten kopieren
	inline void PushRawData(const void * const data, const unsigned length)
	{
		ExtendMemory(length);
		memcpy(&this->data[pos],data,length);
		pos = this->length +=length; 
	}

	/// Sämtliche Integer
	inline void PushSignedInt(signed int i)
	{
		ExtendMemory(4);
		*((signed int*)&data[length]) = htonl(i);
		pos = this->length +=4;
	}
	inline void PushUnsignedInt(unsigned int i)
	{
		ExtendMemory(4);
		*((unsigned int*)&data[length]) = htonl(i);
		pos = this->length +=4;
	}

	inline void PushSignedShort(signed short int i)
	{
		ExtendMemory(2);
		*((signed short*)&data[length]) = htons(i);
		pos = this->length +=2;
	}
	inline void PushUnsignedShort(unsigned short int i)
	{
		ExtendMemory(2);
		*((unsigned short*)&data[length]) = htons(i);
		pos = this->length +=2;
	}

	inline void PushSignedChar(signed char i)
	{
		ExtendMemory(1);
		*((signed char*)&data[length]) = i;
		pos = this->length +=1;
	}
	inline void PushUnsignedChar(unsigned char i)
	{
		ExtendMemory(1);
		*((signed char*)&data[length]) = i;
		pos = this->length +=1;
	}

	inline void PushBool(bool b)
	{
		PushUnsignedChar(b ? 1 : 0);
	}

	inline void PushString(const std::string& str)
	{
		PushUnsignedInt(str.length());
		for(unsigned i = 0;i<str.length();++i)
			PushSignedChar(str[i]);
	}

	// Lesemethoden

	/// Copy all data
	inline void ToBuffer(unsigned char * const buffer) 
	{ 
		memcpy(buffer, data, length);
	}

	/// Rohdaten kopieren
	inline void PopRawData(void * const data, const unsigned length)
	{
		assert(pos<length);
		memcpy(data,&this->data[pos],length);
		pos +=length; 
	}

	/// Sämtliche Integer
	inline signed int PopSignedInt()
	{
		assert(pos<length);
		signed int i = htonl(*((signed int*)&data[pos]));
		pos +=4;
		return i;
	}
	inline unsigned int PopUnsignedInt()
	{
		assert(pos<length);
		unsigned int i = htonl(*((unsigned int*)&data[pos]));
		pos +=4;
		return i;
	}

	inline signed short PopSignedShort()
	{
		assert(pos<length);
		signed short i = htons(*((signed short*)&data[pos]));
		pos +=2;
		return i;
	}
	inline unsigned short PopUnsignedShort()
	{
		unsigned short i = htons(*((unsigned short*)&data[pos]));
		pos +=2;
		return i;
	}

	inline signed char PopSignedChar()
	{
		assert(pos<length);
		signed char i = *((signed char*)&data[pos]);
		pos +=1;
		return i;
	}
	inline unsigned char PopUnsignedChar()
	{
		assert(pos<length);
		unsigned char i = *((unsigned char*)&data[pos]);
		pos +=1;
		return i;
	}

	inline bool PopBool()
	{
		assert(pos<length);
		return ((PopUnsignedChar() == 1) ? true : false);
	}

	inline std::string PopString() 
	{
		assert(pos<length);
		std::string str;
		str.resize(PopUnsignedInt());
		for(unsigned i = 0;i<str.length();++i)
			str[i] = PopSignedChar();
		return str;
	}

protected:
	/// data mit den Daten
	unsigned char * data;
	/// Länge des datas
	unsigned buffer_length;
	/// Logische Länge
	unsigned length;
	/// Schreib/Leseposition
	unsigned pos;

protected:
	/// vergrößert den Speicher auf die nächst höhere 2er potenz zur Länge @p length.
	void Realloc(const unsigned int length);
	/// Erweitert ggf. Speicher um add_length
	void ExtendMemory(const unsigned add_length);

};

#endif // !SERIALIZER_H_INCLUDED
