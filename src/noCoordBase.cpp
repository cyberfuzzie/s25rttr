// $Id: noCoordBase.cpp 6458 2010-05-31 11:38:51Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "noCoordBase.h"
#include "SerializedGameData.h"
#include "GameWorld.h"

void noCoordBase::Serialize_noCoordBase(SerializedGameData * sgd) const 
{
	Serialize_noBase(sgd);

	sgd->PushUnsignedShort(x);
	sgd->PushUnsignedShort(y);
}

noCoordBase::noCoordBase(SerializedGameData * sgd, const unsigned obj_id) : noBase(sgd,obj_id),
x(sgd->PopUnsignedShort()),
y(sgd->PopUnsignedShort())
{
}

unsigned noCoordBase::CreateGUIID() const
{
	return gwg->CreateGUIID(x,y);
}
