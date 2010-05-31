// $Id: GameSavegame.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef GAMESAVEGAME_H_INCLUDED
#define GAMESAVEGAME_H_INCLUDED

#pragma once

#include "GameSavedFile.h"
#include "SerializedGameData.h"

class Savegame : public SavedFile
{
public:
	Savegame();
	~Savegame();

	/// Schreibst Savegame oder Teile davon
	bool Save(const std::string& filename);
	bool Save(BinaryFile& file);

	/// Lädt Savegame oder Teile davon
	bool Load(const std::string&  filename, const bool load_players, const bool load_sgd);
	bool Load(BinaryFile& file, const bool load_players, const bool load_sgd);

public:
	/// Start-GF
	unsigned int start_gf;
	/// Serialisierte Spieldaten
	SerializedGameData sgd;

private:
	static const unsigned short SAVE_VERSION;
	static const char SAVE_SIGNATURE[8];
};

#endif //!GAMESAVEGAME_H_INCLUDED
