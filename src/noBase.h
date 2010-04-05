// $Id: noBase.h 6267 2010-04-05 09:16:14Z OLiver $
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
#ifndef NOBASE_H_INCLUDED
#define NOBASE_H_INCLUDED

#pragma once

#include "GameObject.h"
#include "NodalObjectTypes.h"

class FOWObject;

class noBase : public GameObject
{
public:
	/// Konstruktor von @p noBase.
	noBase(const NodalObjectType nop) : nop(nop) {}
	/// Deserialisierungskonstruktor
	noBase(SerializedGameData * sgd,const unsigned obj_id);

	/// An x,y zeichnen.
	virtual void Draw(int x, int y) = 0;

	/// Type zur�ckgeben.
	NodalObjectType GetType(void) const { return nop; }
	/// Serialisierungsfunktion.
	void Serialize(SerializedGameData *sgd) const { Serialize_noBase(sgd); }

	/// Erzeugt von ihnen selbst ein FOW Objekt als visuelle "Erinnerung" f�r den Fog of War
	virtual FOWObject * CreateFOWObject() const;

	/// Gibt an, inwieweit ein Objekt auf der Karte die BQ beeinflusst
	enum BlockingManner
	{
		BM_NOTBLOCKING, // blockiert gar nicht (z.B. Zierobjekte)
		BM_HUT,
		BM_HOUSE,
		BM_CASTLE,
		BM_MINE,
		BM_SINGLEBLOCKING, // Blockiert nur einzelnen Punkt, hat aber sonst keinen weiteren Einfluss auf Umgebung
		BM_GRANITE,
		BM_TREE,
		BM_FLAG
		};

	virtual BlockingManner GetBM() const;

protected:
	/// R�umt das Basisobjekt auf.
	void Destroy_noBase(void) {}
	/// serialisiert das Basisobjekt.
	void Serialize_noBase(SerializedGameData * sgd) const;

protected:
	NodalObjectType nop; ///< Typ des NodeObjekt ( @see NodalObjectTypes.h )
};

#endif // !NOBASE_H_INCLUDED
