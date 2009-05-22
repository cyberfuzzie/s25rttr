// $Id: Serializer.h 4898 2009-05-20 13:44:45Z OLiver $
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


#ifndef SERIALIZER_H_
#define SERIALIZER_H_


#include <memory.h>
#include <string>

/// Klasse die einen data zum Serialisieren verwaltet und entsprechende Methoden zum Lesen/Schreiben
/// bereitstellt
class Serializer
{
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

	/// Kopiermethoden

	/// Rohdaten kopieren
	void PushRawData(const void * const data, const unsigned length)
	{
		ExtendMemory(length);
		memcpy(this->data+length,data,length);
		pos = this->length +=length; 
	}

	/// Sämtliche Integer
	void PushSignedInt(signed int i)
	{
		ExtendMemory(4);
		*((signed int*)&data[length]) = i;
		pos = this->length +=4;
	}
	void PushUnsignedInt(unsigned int i)
	{
		ExtendMemory(4);
		*((unsigned int*)&data[length]) = i;
		pos = this->length +=4;
	}

	void PushSignedShort(signed short int i)
	{
		ExtendMemory(2);
		*((signed short*)&data[length]) = i;
		pos = this->length +=2;
	}
	void PushUnsignedShort(unsigned short int i)
	{
		ExtendMemory(2);
		*((unsigned short*)&data[length]) = i;
		pos = this->length +=2;
	}

	void PushSignedChar(signed char i)
	{
		ExtendMemory(1);
		*((signed char*)&data[length]) = i;
		pos = this->length +=1;
	}
	void PushUnsignedChar(unsigned char i)
	{
		ExtendMemory(1);
		*((signed char*)&data[length]) = i;
		pos = this->length +=1;
	}

	void PushBool(bool b)
	{
		PushUnsignedChar(b ? 1 : 0);
	}

	void PushString(const std::string& str)
	{
		PushUnsignedInt(str.length());
		for(unsigned i = 0;i<str.length();++i)
			PushSignedChar(str[i]);
	}

	// Lesemethoden

	/// Copy all data
	void ToBuffer(unsigned char * const data);

	/// Rohdaten kopieren
	void PopRawData(void * const data, const unsigned length)
	{
		memcpy(data,&this->data[pos],length);
		pos +=length; 
	}

	/// Sämtliche Integer
	signed int PopSignedInt()
	{
		signed int i = *((signed int*)&data[pos]);
		pos +=4;
		return i;
	}
	unsigned int PopUnsignedInt()
	{
		unsigned int i = *((unsigned int*)&data[pos]);
		pos +=4;
		return i;
	}

	signed short PopSignedShort()
	{
		signed short i = *((signed short*)&data[pos]);
		pos +=2;
		return i;
	}
	unsigned short PopUnsignedShort()
	{
		unsigned short i = *((unsigned short*)&data[pos]);
		pos +=2;
		return i;
	}

	signed char PopSignedChar()
	{
		signed char i = *((signed char*)&data[pos]);
		pos +=1;
		return i;
	}
	unsigned char PopUnsignedChar()
	{
		unsigned char i = *((unsigned char*)&data[pos]);
		pos +=1;
		return i;
	}

	bool PopBool()
	{
		return ((PopUnsignedChar() == 1) ? true : false);
	}

	std::string PopString() 
	{
		std::string str;
		str.resize(PopUnsignedInt());
		for(unsigned i = 0;i<str.length();++i)
			str[i] = PopSignedChar();
		return str;
	}
};


#endif
