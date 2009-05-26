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
// aint with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.

#ifndef ENHOPTION_H_INCLUDED
#define ENHOPTION_H_INCLUDED

#include <string>
#include <map>
#include <vector>
#include "main.h" // TODO: Work out what is needed from main.h
#include "Serializer.h"

enum OptType
{
	OPT_BOOL,
	OPT_SELECT,
	OPT_INT
};

enum OptGroup
{
	OPT_NONE,
	OPT_MILITARY,
	OPT_ECONOMY,
	OPT_CONSTRUCTION
};

class enhOption;
class enhOptions;

// class for an enhancement option
class enhOption
{
	friend class enhOptions;
public:
	/// Constructors
	enhOption(void) : id(0), type(OPT_BOOL), def(0), state(0), min(0), max(0), name(""), description(""), ingameChangeable(false), group(OPT_NONE) {}
	enhOption(unsigned int id, OptType type, int def, int min, int max, std::string name, bool isIngameChangeable, OptGroup group);

	/// Destructor
	~enhOption(void) {}
	
	// id
	inline const unsigned int GetId() const {return id; } 
	// return type of option (bool, selections, integer)
	inline const OptType GetType() const { return type; }
	// return def value
	inline const int GetDef() const { return def; } 
	// get maximum value (useful for OPT_SELECT only, where min is 0)
	inline const int GetState() const { return state; }
	// set current state, tell about success
	inline const int GetMin() const { return min; }
	// return current state
	inline const int GetMax() const { return max; }
	// get maximum value (useful for OPT_SELECT only, where min is 0)

	// return name/description
	inline const std::string GetName() const { return name; }
	// return tooltip/description
	inline const std::string GetDescription() const {return description; }
	// whether may be changed while the game runs
	inline const bool isIngameChangeable() const { return ingameChangeable; }
	// group
	// inline const OptGroup GetGroup() const { return group; }

	// return selections (in case of OPT_SELECT)
	inline const std::string GetSelections(unsigned int i) const { return selections[i]; }

	// set the state and tell about success
	const bool SetState(int s) 
	{ 
		if(s >= min && s <= max) 
		{
			state = s;
			return true;
		}
		return false; 
	}

private:
	unsigned int id;
	OptType type;
	int def, state, min, max;
	std::string name, description;
	bool ingameChangeable;
	OptGroup group;
	std::vector<std::string> selections;
};

 
// container class
class enhOptions
{
public:
	// Constructor
	enhOptions(void);
	enhOptions(const enhOptions& other);
	// Destructor
	~enhOptions(void);

	void Serialize(Serializer * serializer) const;
	void Deserialize(Serializer * serializer);

	enhOptions& operator=(enhOptions const& other);

	// get Option by ID
	enhOption* by_id(const unsigned int id) const { return (enh_by_id.find(id))->second; }
	// get Option by Number (for getting all of them in a predefined order)
	enhOption* by_order(const unsigned int i) const { return enh_by_order.at(i); }
	inline const unsigned char size() const { return enh_by_order.size(); }

	unsigned GetLength() const { return 1 + 8 * size(); }
	unsigned char * Serialize() const;
	void Deserialize(const unsigned char * buffer);

protected:
	std::map<unsigned int, enhOption*> enh_by_id;
	std::vector<enhOption*> enh_by_order;
};
#endif // !ENHOPTION_H_INCLUDED
