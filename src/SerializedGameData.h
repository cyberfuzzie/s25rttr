// $Id: SerializedGameData.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef SERIALIZED_GAME_DATA_H_
#define SERIALIZED_GAME_DATA_H_

#pragma once

#include <memory.h>
#include "list.h"
#include "BinaryFile.h"
#include "GameObject.h"
#include "FOWObjects.h"

class noBase;
class GameObject;
class GameWorld;


/// Kümmert sich um das Serialisieren der GameDaten fürs Speichern und Resynchronisieren
class SerializedGameData
{
	/// Großer Buffer aller Daten
	unsigned char * buffer;
	/// Größe des Buffers
	unsigned buffer_size;
	/// Aktuelle Position im Buffer
	unsigned pos;
	/// Objektreferenzen
	union
	{
		const GameObject ** objects_write;
		GameObject ** objects_read;
	};
	/// Voraussichtliche Gesamtanzahl an Objekten (nur beim Laden)
	unsigned total_objects_count;
	/// Aktuelle Anzahl an Objekten
	unsigned objects_count;

	EventManager *em;

private:

	/// Objekt(referenzen) lesen
	GameObject * PopObject_(GO_Type got); 

public:

	SerializedGameData();
	~SerializedGameData();

	/// Nimmt das gesamte Spiel auf und speichert es im Buffer
	void MakeSnapshot(GameWorld *const gw, EventManager *const em);
	/// Schreibt den Buffer in eine Datei
	void WriteToFile(BinaryFile& file);
	/// Liest den Buffer aus einer Datei
	void ReadFromFile(BinaryFile& file);

	void PrepareDeserialization(EventManager * const em) { this->em = em; }

	/// Erzeugt GameObject
	GameObject * Create_GameObject(const GO_Type got, const unsigned obj_id);
	/// Erzeugt FOWObject
	FOWObject * Create_FOWObject(const FOW_Type fowtype);

	/// Kopiermethoden

	/// Rohdaten kopieren
	void PushRawData(const void * const data, const unsigned length)
	{ memcpy(buffer+pos,data,length); pos += length; }

	/// Sämtliche Integer
	void PushSignedInt(signed int i)
	{ memcpy(buffer+pos,&i,4); pos+=4; }
	void PushUnsignedInt(unsigned i)
	{ memcpy(buffer+pos,&i,4); pos+=4; }

	void PushSignedShort(signed short i)
	{ memcpy(buffer+pos,&i,2); pos+=2; }
	void PushUnsignedShort(unsigned short i)
	{ memcpy(buffer+pos,&i,2); pos+=2; }

	void PushSignedChar(signed char i)
	{ buffer[pos] = i; ++pos; }
	void PushUnsignedChar(unsigned char i)
	{ buffer[pos] = i; ++pos; }

	/// Booleans
	void PushBool(const bool b)
	{ buffer[pos] = b?1:0; ++pos; }

	/// Objekt(referenzen) kopieren
	void PushObject(const GameObject * go,const bool known); 

	/// Kopiert eine Liste von GameObjects
	template <typename T>
	void PushObjectList(const list<T*>& gos,const bool known)
	{
		// Anzahl
		PushUnsignedInt(gos.size());
		// einzelne Objekte
		for(typename list<T*>::const_iterator it = gos.begin(); it.valid(); ++it)
			PushObject(*it,known);
	}

	/// FoW-Objekt
	void PushFOWObject(const FOWObject * fowobj);


	// Lesemethoden

	/// Rohdaten lesen
	void PopRawData(void * const data, const unsigned length)
	{ memcpy(data,buffer+pos,length); pos += length; }

	/// Sämtliche Integer
	signed int PopSignedInt()
	{ signed int i; memcpy(&i,buffer+pos,4); pos+=4; return i; }
	unsigned PopUnsignedInt()
	{ unsigned i; memcpy(&i,buffer+pos,4); pos+=4; return i; }

	signed short PopSignedShort()
	{ signed short i; memcpy(&i,buffer+pos,2); pos+=2; return i;  }
	unsigned short PopUnsignedShort()
	{ unsigned short i; memcpy(&i,buffer+pos,2); pos+=2; return i;  }

	signed char PopSignedChar()
	{ return buffer[pos++]; }
	unsigned char PopUnsignedChar()
	{ return buffer[pos++]; }

	/// Booleans
	bool PopBool()
	{ return (buffer[pos++]?true:false); }

	/// Objekt(referenzen) lesen
	template <typename T>
	T * PopObject(GO_Type got) { return static_cast<T*>(PopObject_(got)); } 

	/// FoW-Objekt
	FOWObject * PopFOWObject();
	
	/// Liest eine Liste von GameObjects
	template <typename T>
	void PopObjectList(list<T*>& gos,GO_Type got)
	{
		// Anzahl
		unsigned size = PopUnsignedInt();
		// einzelne Objekte
		for(unsigned i = 0;i<size;++i)
			gos.push_back(PopObject<T>(got));
	}

	/// Fügt ein gelesenes Objekt zur globalen Objektliste dazu
	void AddObject(GameObject * go);


	/// Sucht ein Objekt, falls vorhanden
	const GameObject * GetConstGameObject(const unsigned obj_id) const;
	GameObject * GetGameObject(const unsigned obj_id) const;
};


#endif // !SERIALIZED_GAME_DATA_H_
