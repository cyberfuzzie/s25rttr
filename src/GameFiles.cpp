// $Id: GameFiles.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "GameFiles.h"
#include "BinaryFile.h"
#include "../version.h"
#include "Log.h"
#include "GamePlayerInfo.h"
#include "SerializedGameData.h"

SavedGameFile::SavedGameFile() : save_time(0), player_count(0), players(0)
{}

SavedGameFile::~SavedGameFile()
{
	delete [] players;
}


void SavedGameFile::WriteVersion(BinaryFile& file,unsigned signature_length,const char * signature,unsigned short version)
{
	// Signatur schreiben
	file.WriteRawData(signature,signature_length);

	// Version vom Programm reinschreiben (mal 0 mit reinschreiben, damits ne runde 8 ergibt!)
	file.WriteRawData(GetWindowRevision(), 8);

	// Version des Save-Formats
	file.WriteUnsignedShort(version);
}


bool SavedGameFile::ValidateFile(BinaryFile& file,unsigned signature_length,const char * signature,unsigned short version)
{
	char read_signature[32];

	file.ReadRawData(read_signature,signature_length);

	// Signatur überprüfen
	if(memcmp(read_signature,signature,signature_length))
	{
		// unterscheiden sich! --> raus
		LOG.lprintf("SavedGameFile::Load: ERROR: Not a valid file!\n");
		return false;
	}

	// Programmversion überspringen
	file.Seek(8,SEEK_CUR);

	// Version überprüfen
	unsigned short read_version = file.ReadUnsignedShort();
	if(read_version != version)
	{
		// anderes Dateiformat --> raus
		LOG.lprintf("SavedGameFile::Load: ERROR: Old file version (version: %u; expected: %u)!\n",read_version,version);
		return false;
	}

	return true;

}


void SavedGameFile::WritePlayerData(BinaryFile& file)
{
	// Spielerdaten
	for(unsigned char i = 0;i<player_count;++i)
	{
		file.WriteUnsignedInt(players[i].ps);

		if(players[i].ps != PS_LOCKED)
		{
			file.WriteShortString(players[i].name);

			file.WriteUnsignedChar(players[i].nation);
			file.WriteUnsignedChar(players[i].color);
			file.WriteUnsignedChar(players[i].team);
		}
	}
}

void SavedGameFile::ReadPlayerData(BinaryFile& file)
{
	// ggf. wieder löschen
	delete [] players;

	players = new SavedGameFile::Player[player_count];
	for(unsigned char i = 0;i<player_count;++i)
	{
		players[i].ps = file.ReadUnsignedInt();

		if(players[i].ps != PS_LOCKED)
		{
			file.ReadShortString(players[i].name);
			
			players[i].nation = Nation(file.ReadUnsignedChar());
			players[i].color = file.ReadUnsignedChar();
			players[i].team = file.ReadUnsignedChar();
		}
	}
}


void SavedGameFile::WriteGGS(BinaryFile& file)
{
	// GGS
	unsigned char * buffer = ggs.Serialize();
	file.WriteRawData(buffer,GlobalGameSettings::GGS_BUFFER_SIZE);
	delete [] buffer;
}

void SavedGameFile::ReadGGS(BinaryFile& file)
{
	// GGS
	// Buffer erzeugen
	unsigned char * buffer = new unsigned char[GlobalGameSettings::GGS_BUFFER_SIZE];
	file.ReadRawData(buffer,GlobalGameSettings::GGS_BUFFER_SIZE);
	ggs.Deserialize(buffer);
	delete [] buffer;
}
	
// Kleine Signatur am Anfang "RTTRSAVE", die ein gültiges S25 RTTR Savegame kennzeichnet
const char Savegame::SAVE_SIGNATURE[8] = {'R','T','T','R','S','A','V','E'};
/// Version des Save-Formates
const unsigned short Savegame::SAVE_VERSION = 5;

Savegame::Savegame() : SavedGameFile(), start_gf(0)
{
}

Savegame::~Savegame()
{
}

bool Savegame::Save(const std::string& filename)
{
	BinaryFile file;
	
	if(!file.Open(filename.c_str(),OFM_WRITE))
		return false;

	bool ret = Save(file);

	file.Close();

	return ret;
}

bool Savegame::Save(BinaryFile& file)
{
	// Versionszeug schreiben
	WriteVersion(file,8,SAVE_SIGNATURE,SAVE_VERSION);

	// Timestamp der Aufzeichnung (TODO: Little/Big Endian unterscheidung)
	file.WriteRawData(&save_time,8);

	// Mapname
	file.WriteShortString(map_name);

	// Anzahl Spieler
	file.WriteUnsignedChar(player_count);

	// Größe der Spielerdaten (später ausfüllen)
	unsigned players_size = 0;
	unsigned players_pos = file.Tell();
	file.WriteUnsignedInt(players_size);

	// Spielerdaten
	WritePlayerData(file);

	// Wieder zurückspringen und Größe des Spielerblocks eintragen
	unsigned new_pos = file.Tell();
	file.Seek(players_pos,SEEK_SET);
	file.WriteUnsignedInt(new_pos-players_pos-4);
	file.Seek(new_pos,SEEK_SET);

	// GGS
	WriteGGS(file);

	// Start-GF
	file.WriteUnsignedInt(start_gf);

	// Serialisiertes Spielzeug reinschreiben
	sgd.WriteToFile(file);

	return true;
}


bool Savegame::Load(const std::string&  filename,const bool load_players,const bool load_sgd)
{
	BinaryFile file;
	
	if(!file.Open(filename.c_str(),OFM_READ))
		return false;

	bool ret = Load(file,load_players,load_sgd);

	file.Close();

	return ret;
}

bool Savegame::Load(BinaryFile& file,const bool load_players,const bool load_sgd)
{
	// Signatur und Version einlesen
	if(!ValidateFile(file,8,SAVE_SIGNATURE,SAVE_VERSION))
	{
		LOG.lprintf("Savegame::Load: ERROR: File is not a valid RTTR savegame!\n");
		return false;
	}

	// Zeitstempel
	file.ReadRawData(&save_time,8);

	// Map-Name
	file.ReadShortString(map_name);

	// Anzahl Spieler
	player_count = file.ReadUnsignedChar();

	// Spielerzeug
	if(load_players)
	{
		// Größe des Spielerblocks überspringen
		file.Seek(4,SEEK_CUR);

		ReadPlayerData(file);
	}
	else
	{
		// Überspringen
		players = 0;
		unsigned player_size = file.ReadUnsignedInt();
		file.Seek(player_size,SEEK_CUR);
	}

	// GGS
	ReadGGS(file);

	// Start-GF
	start_gf = file.ReadUnsignedInt();

	if(load_sgd)
		sgd.ReadFromFile(file);

	return true;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


// Kleine Signatur am Anfang "RTTRRP", die ein gültiges S25 RTTR Replay kennzeichnet
const char Replay::REPLAY_SIGNATURE[6] = {'R','T','T','R','R','P'};
/// Version des Save-Formates
const unsigned short Replay::REPLAY_VERSION = 10;

Replay::Replay() : nwf_length(0), random_init(0), map_length(0), map_zip_length(0), map_data(0), savegame(0), last_gf(0),
last_gf_file_pos(0)
{
}


Replay::~Replay()
{
	StopRecording();
}

void Replay::StopRecording()
{
	file.Close();

	delete [] players;
	players = 0;
	delete [] map_data;
	map_data = 0;
}

bool Replay::WriteHeader(const std::string& filename)
{
	// Datei öffnen
	if(!file.Open(filename.c_str(),OFM_WRITE))
		return false;

	// Versionszeug schreiben
	WriteVersion(file,6,REPLAY_SIGNATURE,REPLAY_VERSION);
	// Timestamp der Aufzeichnung (TODO: Little/Big Endian unterscheidung)
	file.WriteRawData(&save_time,8);
	/// NWF-Länge
	file.WriteUnsignedShort(nwf_length);
	/// Zufallsgeneratorinitialisierung
	file.WriteUnsignedInt(random_init);

	// Position merken für End-GF
	last_gf_file_pos = file.Tell();

	/// End-GF (erstmal nur 0, wird dann im Spiel immer geupdatet)
	file.WriteUnsignedInt(last_gf);
	// Anzahl Spieler
	file.WriteUnsignedChar(player_count);
	// Spielerdaten
	WritePlayerData(file);
	// GGS
	WriteGGS(file);

	// Map-Type
	file.WriteUnsignedShort(static_cast<unsigned short>(map_type));

	switch(map_type)
	{
	default:
		break;
	case MAPTYPE_OLDMAP:
		{
			// Map-Daten
			file.WriteUnsignedInt(map_length);
			file.WriteUnsignedInt(map_zip_length);
			file.WriteRawData(map_data,map_zip_length);
		} break;
	case MAPTYPE_SAVEGAME:
		{
			// Savegame speichern
			if(!savegame->Save(file))
				return false;
		} break;
	}

	// Mapname
	file.WriteShortString(map_name);

	// bei ungerader 4er position aufrunden
	while(file.Tell() % 4)
		file.WriteSignedChar(0);

	// Alles sofort reinschreiben
	file.Flush();

	return true;
}


bool Replay::LoadHeader(const std::string& filename,const bool load_extended_header)
{
	// Datei öffnen
	if(!file.Open(filename.c_str(),OFM_READ))
		return false;

	// Version überprüfen
	// Signatur und Version einlesen
	if(!ValidateFile(file,6,REPLAY_SIGNATURE,REPLAY_VERSION))
	{
		LOG.lprintf("Replay::Load: ERROR: File \"%s\" is not a valid RTTR replay!\n", filename.c_str());
		return false;
	}

	// Zeitstempel
	file.ReadRawData(&save_time,8);
	// NWF-Länge
	nwf_length = file.ReadUnsignedShort();
	// Zufallsgeneratorinitialisierung
	random_init = file.ReadUnsignedInt();
	/// End-GF
	last_gf = file.ReadUnsignedInt();
	// Spieleranzahl
	player_count = file.ReadUnsignedChar();

	// Spielerdaten
	ReadPlayerData(file);
	// GGS
	ReadGGS(file);

	// Map-Type
	map_type = static_cast<MapType>(file.ReadUnsignedShort());

	if(load_extended_header)
	{
		switch(map_type)
		{
		default:
			break;
		case MAPTYPE_OLDMAP:
			{
				// Map-Daten
				map_length = file.ReadUnsignedInt();
				map_zip_length = file.ReadUnsignedInt();
				map_data = new unsigned char[map_zip_length];
				file.ReadRawData(map_data,map_zip_length);
			} break;
		case MAPTYPE_SAVEGAME:
			{
				// Savegame speichern
				if(!savegame->Load(file,true,true))
					return false;
			} break;
		}

		file.ReadShortString(map_name);

		// bei ungerader 4er position aufrunden
		while(file.Tell() % 4)
			file.Seek(1, SEEK_CUR);
	}

	return true;
}


void Replay::AddChatCommand(const unsigned gf,const unsigned char player, const unsigned char dest, const std::string& str)
{
	if(!file.IsValid())
		return;

	// GF-Anzahl
	file.WriteUnsignedInt(gf);
	// Type (0)
	file.WriteUnsignedChar(RC_CHAT);
	// Spieler
	file.WriteUnsignedChar(player);
	// Ziel
	file.WriteUnsignedChar(dest);
	// Chat-Text
	file.WriteLongString(str);

	// bei ungerader 4er position aufrunden
	while(file.Tell() % 4)
		file.WriteSignedChar(0);

	// Sofort rein damit
	file.Flush();
}


void Replay::AddGameCommand(const unsigned gf,const unsigned short length, const unsigned char * const data)
{
	if(!file.IsValid())
		return;

	//// Marker schreiben
	//file.WriteRawData("GCCM", 4);

	// GF-Anzahl
	file.WriteUnsignedInt(gf);
	// Type (RC_GAME)
	file.WriteUnsignedChar(RC_GAME);

	// Länge der Daten
	file.WriteUnsignedShort(length);
	// Daten
	file.WriteRawData(data,length);

	// bei ungerader 4er position aufrunden
	while(file.Tell() % 4)
		file.WriteSignedChar(0);

	// Sofort rein damit
	file.Flush();
}

bool Replay::ReadGF(unsigned * gf)
{
	// bei ungerader 4er position aufrunden
	while(file.Tell() % 4 && !file.EndOfFile())
		file.Seek(1, SEEK_CUR);

	//// kein Marker bedeutet das Ende der Welt
	//if(memcmp(marker, "GCCM", 4) != 0)
	//	return false;

	// GF-Anzahl
	*gf = file.ReadUnsignedInt();

	// Replaydatei zu Ende?
	bool eof = file.EndOfFile();
	if(eof)
		*gf = 0xFFFFFFFF;

	return !eof;
}

ReplayCommand Replay::ReadRCType()
{
	// Type auslesen
	return ReplayCommand(file.ReadUnsignedChar());
}

void Replay::ReadChatCommand(unsigned char * player, unsigned char  * dest, std::string& str)
{
	*player = file.ReadUnsignedChar();
	*dest = file.ReadUnsignedChar();
	file.ReadLongString(str);
}

void Replay::ReadGameCommand(unsigned short *length, unsigned char ** data)
{
	*length = file.ReadUnsignedShort();
	*data = new unsigned char[*length];
	file.ReadRawData(*data,*length);
}

void Replay::UpdateLastGF(const unsigned last_gf)
{
	// An die Stelle springen
	file.Seek(last_gf_file_pos,SEEK_SET);
	// Dorthin schreiben
	file.WriteUnsignedInt(last_gf);
	// Wieder ans Ende springen
	file.Seek(0,SEEK_END);

}
