// $Id: LobbyProtocol.h 4936 2009-05-24 15:17:07Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "Serializer.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Serializer::Serializer() : data(0), buffer_length(0), length(0), pos(0)
{
}

Serializer::Serializer(const void * const data, const unsigned initial_size)
	: data(new unsigned char[initial_size]), buffer_length(initial_size), length(initial_size), pos(0)
{
	memcpy(this->data, data, initial_size);
}

Serializer::Serializer(const Serializer& two) 
	: data(new unsigned char[two.buffer_length]), buffer_length(two.buffer_length), length(two.length), pos(two.pos)
{
	memcpy(data,two.data,length);
}

Serializer::~Serializer()
{
	delete[] data;
}

/// Aufräumen
void Serializer::Clear()
{
	delete[] data;
	buffer_length = 0;
	length = 0;
	pos = 0;
}

void Serializer::Realloc(const unsigned int length)
{
	if(this->buffer_length == 0)
			this->buffer_length = 64;

	// speicher vergrößern
	while(this->buffer_length < length)
		this->buffer_length *= 2;

	if(data == 0)
	{
		// neu anlegen
		data = new unsigned char[this->buffer_length];
		memset(data, 0, sizeof(unsigned char)*this->buffer_length);
	}
	else
	{
		// umkopieren (vergrößern)
		unsigned char *ndata = new unsigned char[this->buffer_length];
		memcpy(ndata, data, this->length);

		delete[] data;
		data = ndata;
	}
}


/// Erweitert ggf. Speicher um add_length
void Serializer::ExtendMemory(const unsigned add_length)
{
	if(length + add_length > this->buffer_length)
		Realloc(length+add_length);
}
