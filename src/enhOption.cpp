// $Id: optOption.h 4830 2009-05-07 18:59:21Z Divan $
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

#include "enhOption.h"
#include "main.h"

//////////////////////////////////////////////////
// Class enhOption

enhOption::enhOption(unsigned int id, OptType type, int def, int min, int max, std::string name, bool ingameChangeable, OptGroup group)
{
	this->id = id;
	this->type = type;
	this->def = def;
	this->state = def;
	this->min = min;
	this->max = max;
	this->name = name;
	this->description = "";
	this->ingameChangeable = ingameChangeable;
	this->group = group;
}

//////////////////////////////////////////////////
// Class enhOptions

// enhancement option Ids
#define	ENH_EXAMPLE1	10010000
#define	ENH_EXAMPLE2	10010002
#define ENH_WATERWAYLENGTH	10010001

enhOptions::enhOptions(void)
{
	// Prepare enhancement options
	enhOption *enh;
	
	// example boolean option

		enh = new enhOption(ENH_EXAMPLE1, OPT_BOOL, 0, 0, 1, "Example option bool", true, OPT_NONE);
		enh->description="Have fun using it";
		enh_by_id.insert(std::make_pair(enh->GetId(), enh));
		enh_by_order.push_back(enh);

		enh = new enhOption(ENH_EXAMPLE2, OPT_SELECT, 1, 0, 2, "Example option select", false, OPT_NONE);
		enh->description="Have fun using it";
		enh->selections.push_back("First option");
		enh->selections.push_back("Second option");
		enh->selections.push_back("Third option");
		enh_by_id.insert(std::make_pair(enh->GetId(), enh));
		enh_by_order.push_back(enh);

		// waterway length
		enh = new enhOption(ENH_WATERWAYLENGTH, OPT_INT, 4, 0, 255, _("Waterway's maximum length"), true, OPT_CONSTRUCTION);
		enh->description = _("0 means infinite");
		enh_by_id.insert(std::make_pair(enh->GetId(), enh));
		enh_by_order.push_back(enh);
}

enhOptions::~enhOptions()
{
	if (size()>0) for(unsigned int i=enh_by_order.size(); i>0;--i)
		delete enh_by_order[i-1];
}

enhOptions::enhOptions(const enhOptions& other)  
{
	operator=(other);
}

// Operator =
enhOptions& enhOptions::operator=(enhOptions const& other)
{
	if (&other != this)
	{
		enhOption *enh;
		// clear this one
		if (size()>0) for(unsigned int i=enh_by_order.size(); i>0;--i)
			delete enh_by_order[i-1];
		enh_by_order.clear();
		enh_by_id.clear();
		// copy the other one
		if (other.size()>0) for(unsigned char i=0; i < other.enh_by_order.size(); ++i)
		{
			enh = new enhOption(*(other.by_order(i)));
			enh_by_id.insert(std::make_pair(enh->GetId(), enh));
			enh_by_order.push_back(enh);
		}
	}
	return *this;
}

void enhOptions::Serialize(Serializer * ser) const
{
	// number of options
	ser->PushUnsignedChar(size());
	
	for(unsigned char i=0; i < size(); ++i)
	{
		ser->PushUnsignedInt(enh_by_order[i]->GetId());	
		ser->PushSignedInt(enh_by_order[i]->GetState());	
	}
}

void enhOptions::Deserialize(Serializer * ser)
{
	unsigned char count = ser->PopUnsignedChar();
	for(unsigned char i=0; i < count; ++i)
	{
		const unsigned int id = ser->PopUnsignedInt();
		std::map<unsigned int, enhOption*>::iterator it = enh_by_id.find(id);
		// Do we have an option with this id? Stay compatible with older savegames (and modified clients)!
		if (it->second->GetId() == id)
		{
			it->second->SetState(ser->PopSignedInt());
		}
		else
		{
			const int value = ser->PopSignedInt();
			LOG.lprintf("Warning: The game settings either loaded from Savegame or received from the server contain an unknown enhancement option with ID %d and value %d.\n", id, value);
		}
	}
}
