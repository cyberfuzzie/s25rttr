// $Id: GameSavedFile.h 5999 2010-02-11 09:53:02Z FloSoft $
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
#ifndef GAMESAVEDFILE_H_INCLUDED
#define GAMESAVEDFILE_H_INCLUDED

#pragma once

#include "BinaryFile.h"
#include "GameConsts.h"
#include "GlobalGameSettings.h"

/// Basisklasse für Replays und Savegames
class SavedFile
{
public:
	SavedFile();
	virtual ~SavedFile();

protected:
	/// Schreibt Signatur und Version der Datei
	void WriteVersion(BinaryFile& file, unsigned int signature_length, const char* signature, unsigned short version);
	/// Überprüft Signatur und Version der Datei
	bool ValidateFile(BinaryFile& file, unsigned int signature_length, const char* signature, unsigned short version);

	/// Schreibt Spielerdaten
	void WritePlayerData(BinaryFile& file);
	/// Liest Spielerdaten aus
	void ReadPlayerData(BinaryFile& file);

	/// schreibt die GlobalGameSettings in die Datei.
	void WriteGGS(BinaryFile& file);
	/// liest die GlobalGameSettings aus der Datei.
	void ReadGGS(BinaryFile& file);

	/// schreibt die Addoneinstellungen in die Datei.
	void WriteAddonSettings(BinaryFile& file);
	/// liest die Addoneinstellungen aus der Datei.
	void ReadAddonSettings(BinaryFile& file);

public:
	/// Zeitpunkt der Aufnahme
	unser_time_t save_time;
	/// Mapname
	std::string map_name;
	/// Anzahl Spieler
	unsigned char player_count;

	/// Spieler
	struct Player
	{
		/// PlayerState
		unsigned ps;
		/// (Damaliger) Name des Spielers
		std::string name;
		/// Volk, Farbe, Team
		Nation nation;
		unsigned char color, team;
	} *players;

	/// GGS
	GlobalGameSettings ggs;
};

#endif // !GAMEFILES_H_INCLUDED
