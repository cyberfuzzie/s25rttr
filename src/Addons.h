// $Id$
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
#ifndef ADDONS_H_INCLUDED
#define ADDONS_H_INCLUDED

#pragma once

#include "const_addons.h"
#include "ctrlComboBox.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon baseclass
 *
 *  @author FloSoft
 */
class Addon
{
public:
	Addon(const AddonId id, const unsigned int groups, const std::string &name, const std::string &description, const unsigned int default_status)
		: id(id), groups(groups), name(name), description(description), default_status(default_status)	{	}
	virtual ~Addon() {	}

	AddonId getId() const { return id; }
	unsigned int getGroups() const { return (ADDONGROUP_ALL|groups); }
	std::string getName() const { return name; }
	std::string getDescription() const { return description; }
	unsigned int getDefaultStatus() const { return default_status; }

private:
	AddonId id;
	unsigned int groups;
	std::string name;
	std::string description;
	unsigned int default_status;
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon baseclass for option-list addons
 *  first option added will be the default one
 *
 *  @author FloSoft
 */
class AddonList : public Addon
{
public:
	AddonList(const AddonId id, const unsigned int groups, const std::string &name, const std::string &description, const unsigned int default_status)
		: Addon(id, groups, name, description, default_status) { }

	void fillComboBox(ctrlComboBox *c) const
	{
		for(std::vector<std::string>::const_iterator it = options.begin(); it != options.end(); ++it)
			c->AddString(*it);
	}

protected:
	void removeOptions()
	{
		options.clear();
	}

	void addOption(const std::string &name)
	{
		if(std::find(options.begin(), options.end(), name) == options.end())
			options.push_back(name);
	}

private:
	std::vector<std::string> options;
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon baseclass for boolean addons
 *
 *  @author FloSoft
 */
class AddonBool : public Addon
{
public:
	AddonBool(const AddonId id, const unsigned int groups, const std::string &name, const std::string &description, const unsigned int default_status)
		: Addon(id, groups, name, description, default_status) { }
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for changing gold deposits to other resources or
 *  to remove them completely
 *
 *  @author Divan
 */
class AddonChangeGoldDeposits : public AddonList
{
public:
	AddonChangeGoldDeposits() : AddonList(ADDON_CHANGE_GOLD_DEPOSITS, 
										  ADDONGROUP_MILITARY|ADDONGROUP_ECONOMY,
											gettext_noop("Change gold deposits"), 
											gettext_noop("Allows to play games without gold.\n\n"
														 "You can choose to remove gold resources completely,\n"
														 "to convert them into iron ore, coal or granite.\n\n"
														 "You'll probably want to convert gold to iron ore, as this (on most maps)\n"
														 "allows you to utilize the additional coal not needed for minting anymore."),
											0
										 )
	{
		addOption(gettext_noop("No change"));
		addOption(gettext_noop("Remove gold completely"));
		addOption(gettext_noop("Convert to iron ore"));
		addOption(gettext_noop("Convert to coal"));
		addOption(gettext_noop("Convert to granite"));	
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for changing the maximum length of waterways.
 *
 *  @author Divan
 */
class AddonMaxWaterwayLength : public AddonList
{
public:
	AddonMaxWaterwayLength() : AddonList(ADDON_MAX_WATERWAY_LENGTH, 
										 ADDONGROUP_GAMEPLAY,
											gettext_noop("Set maximum waterway length"), 
											gettext_noop("Limits the distance settlers may travel per boat.\n\n"
														 "Possible values are:\nShort: 3 tiles\n"
														 "Default: 5 tiles\nLong: 9 tiles\nLonger: 13 tiles\n"
														 "Very long: 21 tiles\nand Unlimited"),
											1
										 )
	{
		addOption(gettext_noop("Short"));
		addOption(gettext_noop("Default"));
		addOption(gettext_noop("Long"));
		addOption(gettext_noop("Longer"));
		addOption(gettext_noop("Very long"));	
		addOption(gettext_noop("Unlimited"));	
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for allowing a custom build sequence
 *
 *  @author Divan
 */
class AddonCustomBuildSequence : public AddonBool
{
public:
	AddonCustomBuildSequence() : AddonBool(ADDON_CUSTOM_BUILD_SEQUENCE,
										   ADDONGROUP_ECONOMY|ADDONGROUP_GAMEPLAY,
											gettext_noop("Custom build sequence"), 
											gettext_noop("Allows every player to control whether building sites\n"
														 "should be supplied in sequence of given order or in a definable\n"
														 "sequence based on the building type."),
											0
										   )
	{
	}
};


///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for changing the maximum length of waterways.
 *
 *  @author Divan
 */
class AddonStatisticsVisibility : public AddonList
{
public:
	AddonStatisticsVisibility() : AddonList(ADDON_STATISTICS_VISIBILITY, 
											ADDONGROUP_OTHER,
											gettext_noop("Change the visibility of your ingame statistics"), 
											gettext_noop("Decides to whom your statistics is visible.\n\n"
												   "\"Allies\" applies to team members as well as "
												   "to allies by treaty."),
											0
										 )
	{
		addOption(gettext_noop("Everyone"));
		addOption(gettext_noop("Allies"));
		addOption(gettext_noop("Nobody else but you"));	
	}
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Addon for limiting the count of catapults
 *
 *  @author FloSoft
 */
class AddonLimitCatapults : public AddonList
{
public:
	AddonLimitCatapults() : AddonList(ADDON_LIMIT_CATAPULTS, 
									  ADDONGROUP_MILITARY,
									  gettext_noop("Limit count of catapults"), 
									  gettext_noop("Limits the count of catapults per player.\n\n"
												   "Proportional uses 0.125 catapult per barrack (you need 8 baracks for one catapult)\n"
												   "                  0.25           per guardhouse (4 guardhouses per catapult)\n"
												   "                  0.5            per watchtower (2 watchtower per catapult)\n"
												   "                  1              per fortress (1 fortress per catapult)\n"),
									  0
									 )
	{
		addOption(gettext_noop("Unlimited"));
		addOption(gettext_noop("Proportional"));
		addOption(gettext_noop("No catapults"));
		addOption(gettext_noop("10 catapults"));
		addOption(gettext_noop("20 catapults"));
		addOption(gettext_noop("30 catapults"));
		addOption(gettext_noop("40 catapults"));
		addOption(gettext_noop("50 catapults"));
	}
};

#endif // !ADDONS_H_INCLUDED
