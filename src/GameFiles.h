// $Id: GameFiles.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef GAMEFILES_H_
#define GAMEFILES_H_

#include "BinaryFile.h"
#include "SerializedGameData.h"
#include "GameProtocol.h"
#include "GameConsts.h"
#include "GlobalGameSettings.h"

class BinaryFile;
class EventManager;

/// Basisklasse für Replays und Savegames
class SavedGameFile
{
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
	} * players;

	/// GGS
	GlobalGameSettings ggs;

protected:

	/// Schreibt Signatur und Version der Datei
	void WriteVersion(BinaryFile& file,unsigned signature_length,const char * signature,unsigned short version);
	/// Schreibt Spielerdaten
	void WritePlayerData(BinaryFile& file);
	/// Schreibt GGS
	void WriteGGS(BinaryFile& file);

	/// Überprüft Signatur und Version der Datei
	bool ValidateFile(BinaryFile& file,unsigned signature_length,const char * signature,unsigned short version);
	/// Liest Spielerdaten aus
	void ReadPlayerData(BinaryFile& file);
	/// Liest GGS
	void ReadGGS(BinaryFile& file);

public:

	SavedGameFile();
	virtual ~SavedGameFile();
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

/// Klasse für geladene bzw. zu speichernde Savegames
class Savegame : public SavedGameFile
{
	static const unsigned short SAVE_VERSION;
	static const char SAVE_SIGNATURE[8];

public:

	/// Start-GF
	unsigned start_gf;
	/// Serialisierte Spieldaten
	SerializedGameData sgd;

public:

	Savegame();
	~Savegame();

	/// Schreibst Savegame oder Teile davon
	bool Save(const std::string& filename);
	bool Save(BinaryFile& file);
	/// Lädt Savegame oder Teile davon
	bool Load(const std::string&  filename, const bool load_players,const bool load_sgd);
	bool Load(BinaryFile& file,const bool load_players,const bool load_sgd);
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

/// Replay-Command-Art
enum ReplayCommand
{
	RC_REPLAYEND = 0,
	RC_CHAT,
	RC_GAME

};

/// Klasse für geladene bzw. zu speichernde Replays
class Replay : public SavedGameFile
{
private:

	/// Replayformat-Version und Signaturen
	static const unsigned short REPLAY_VERSION;
	static const char REPLAY_SIGNATURE[6];

	/// Dateihandle
	BinaryFile file;

public:

	/// NWF-Länge
	unsigned short nwf_length;
	/// Zufallsgeneratorinitialisierung
	unsigned random_init;

	/// Gespeichertes Spiel, Zufallskarte, normale Karte...?
	MapType map_type;
	/// Gepackte Map - Daten (für alte Karte)
	unsigned map_length, map_zip_length;
	unsigned char * map_data;
	/// Savegame (für gespeichertes Spiel)
	Savegame * savegame;

	/// End-GF
	unsigned last_gf;
	/// Position des End-GF in der Datei
	unsigned last_gf_file_pos;

public:

	Replay();
	~Replay();

	/// Räumt auf, schließt datei
	void StopRecording();

	/// Replaydatei gültig?
	bool IsValid() const { return file.IsValid(); }

	/// Beginnt die Save-Datei und schreibt den Header
	bool WriteHeader(const std::string& filename);
	/// Lädt den Header
	bool LoadHeader(const std::string& filename,const bool load_extended_header);

	/// Fügt ein Chat-Kommando hinzu (schreibt)
	void AddChatCommand(const unsigned gf,const unsigned char player, const unsigned char dest, const std::string& str);
	/// Fügt ein Spiel-Kommando hinzu (schreibt)
	void AddGameCommand(const unsigned gf,const unsigned short length, const unsigned char * const data);

	/// Liest RC-Type aus, liefert false, wenn das Replay zu Ende ist
	bool ReadGF(unsigned * gf);
	/// RC-Type aus, liefert false
	ReplayCommand ReadRCType();
	/// Liest ein Chat-Command aus
	void ReadChatCommand(unsigned char * player, unsigned char  * dest, std::string& str);
	void ReadGameCommand(unsigned short *length, unsigned char ** data);

	/// Aktualisiert den End-GF, schreibt ihn in die Replaydatei (nur beim Spielen bzw. Schreiben verwenden!)
	void UpdateLastGF(const unsigned last_gf);
};


#endif // !GAMEFILES_H_
