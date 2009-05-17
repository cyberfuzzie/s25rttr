// $Id: SerializableArray.h 4878 2009-05-17 11:40:50Z OLiver $
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
#ifndef SERIALIZABLEARRAY_H_INCLUDED
#define SERIALIZABLEARRAY_H_INCLUDED

#pragma once

#include "Message.h"

template <class Type, class Allocator = Type>
class SerializableArray
{
public:
	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  Konstruktor von @p SerializableArray.
	 *
	 *  @author FloSoft
	 */
	SerializableArray(void) : count(0), elements(NULL)
	{
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  Destruktor von @p SerializableArray.
	 *
	 *  @author FloSoft
	 */
	~SerializableArray(void)
	{
		delete[] elements;
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  Zuweisungsoperator.
	 *
	 *  @author FloSoft
	 */
	template<class T, class A>
	SerializableArray& operator= (const SerializableArray<T, A> &other)
	{
		return copy(other);
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  Zuweisungsoperator, spezifisch
	 *
	 *  @author FloSoft
	 */
	SerializableArray& operator= (const SerializableArray &other)
	{
		return copy(other);
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  Kopierfunktion
	 *
	 *  @author FloSoft
	 */
	template<class T, class A>
	SerializableArray& copy(const SerializableArray<T, A> &other)
	{
		alloc(other.getCount());

		for(unsigned int i = 0; i < other.getCount(); ++i)
			elements[i] = other.elements[i];

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  Array-Operatoren
	 *
	 *  @author FloSoft
	 */
	Type&       operator[](unsigned int i)       { return getElement(i); }
	const Type& operator[](unsigned int i) const { return getElement(i); }

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  Alloziert eine Spielerliste.
	 *
	 *  @param[in] count Anzahl der Spieler in der Liste.
	 *
	 *  @author FloSoft
	 */
	Type *alloc(unsigned int count)
	{
		clear();

		if(count <= 0)
			return NULL;

		this->count = count;
		this->elements = new Allocator[count];

		return elements;
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  räumt die Liste auf.
	 *
	 *  @author FloSoft
	 */
	void clear(void)
	{
		delete[] elements;

		count	= 0;
		elements = NULL;
	}

	///////////////////////////////////////////////////////////////////////////////
	/*
	 *  serialisiert die Daten.
	 *
	 *  @param[in,out] data Datensatz, muss groß genug sein
	 *
	 *  @return liefert die Größe der Daten zurück.
	 *
	 *  @author FloSoft
	 */
	void serialize(Message * msg) const
	{
		if(msg)
			msg->pushUI(count);

		for(unsigned int i = 0; i < count; ++i)
			elements[i].serialize(msg);
	}

	///////////////////////////////////////////////////////////////////////////////
	/*
	 *  deserialisiert die Daten.
	 *
	 *  @param[in] data Datensatz, muss groß genug sein
	 *
	 *  @return liefert die Größe der gelesenen Daten zurück.
	 *
	 *  @author FloSoft
	 */
	void deserialize(Message * msg)
	{
		if(!msg)
			return;

		count = msg->popUI();

		for(unsigned int i = 0; i < count; ++i)
			elements[i].deserialize(msg);
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  liefert ein Element der Liste.
	 *
	 *  @author FloSoft
	 */
	const Type *getElement(unsigned int i) const
	{
		if(i < count)
			return &elements[i];

		return NULL;
	}

	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  liefert ein Element der Liste.
	 *
	 *  @author FloSoft
	 */
	Type *getElement(unsigned int i)
	{
		if(i < count)
			return &elements[i];

		return NULL;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	/**
	 *  liefert die Anzahl der Elemente.
	 *
	 *  @author FloSoft
	 */
	unsigned int getCount(void) const
	{
		return count; 
	}

private:
	unsigned int count;	///< Anzahl der Elemente in der Liste
	Type *elements;		///< Die Elemente
};

#endif // SERIALIZABLEARRAY_H_INCLUDED
