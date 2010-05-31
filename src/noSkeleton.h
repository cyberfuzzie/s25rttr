// $Id: noSkeleton.h 6458 2010-05-31 11:38:51Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef NOSKELETON_H_INCLUDED
#define NOSKELETON_H_INCLUDED

#pragma once

#include "noCoordBase.h"
#include "EventManager.h"

/// Menschliches Skelett (Zierobjekt, das sich automatisch umwandelt und dann verschwindet)
class noSkeleton: public noCoordBase
{
public:

	noSkeleton(const unsigned short x, const unsigned short y);
	noSkeleton(SerializedGameData * sgd, const unsigned obj_id);

	~noSkeleton();

	void Destroy() { Destroy_noSkeleton(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_noSkeleton(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_noSkeleton(sgd); }

	GO_Type GetGOT() const { return GOT_SKELETON; }

protected:	
	void Destroy_noSkeleton();

	void Draw(int x, int y);
	void HandleEvent(const unsigned int id);

private:
	/// Type des Skeletts (0 = ganz "frisch", 1 - schon etwas verdorrt)
	unsigned char type;
	/// EventPointer, damit der dann gelöscht werden kann, falls das Skelett von außerhalb gelöscht wird
	EventManager::EventPointer current_event;
};

#endif // !NOSKELETON_H_INCLUDED
