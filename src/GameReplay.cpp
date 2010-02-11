// $Id: GameReplay.cpp 5999 2010-02-11 09:53:02Z FloSoft $
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
#include "GameReplay.h"

#include "GameSavegame.h"

/// Kleine Signatur am Anfang "RTTRRP", die ein g�ltiges S25 RTTR Replay kennzeichnet
const char Replay::REPLAY_SIGNATURE[6] = {'R','T','T','R','R','P'};
/// Version des Replay-Formates
const unsigned short Replay::REPLAY_VERSION = 20;

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
Replay::Replay() : nwf_length(0), random_init(0), map_length(0), map_zip_length(0), map_data(0), 
	savegame(0), last_gf(0), last_gf_file_pos(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
Replay::~Replay()
{
	StopRecording();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
void Replay::StopRecording()
{
	file.Close();

	delete [] players;
	players = 0;
	delete [] map_data;
	map_data = 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
bool Replay::WriteHeader(const std::string& filename)
{
	// Datei �ffnen
	if(!file.Open(filename.c_str(),OFM_WRITE))
		return false;

	// Versionszeug schreiben
	WriteVersion(file,6,REPLAY_SIGNATURE,REPLAY_VERSION);
	// Timestamp der Aufzeichnung (TODO: Little/Big Endian unterscheidung)
	file.WriteRawData(&save_time,8);
	/// NWF-L�nge
	file.WriteUnsignedShort(nwf_length);
	/// Zufallsgeneratorinitialisierung
	file.WriteUnsignedInt(random_init);

	// Position merken f�r End-GF
	last_gf_file_pos = file.Tell();

	/// End-GF (erstmal nur 0, wird dann im Spiel immer geupdatet)
	file.WriteUnsignedInt(last_gf);
	// Anzahl Spieler
	file.WriteUnsignedChar(player_count);
	// Spielerdaten
	WritePlayerData(file);
	// GGS
	WriteGGS(file);
	// Addoneinstellungen schreiben
	WriteAddonSettings(file);

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

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
bool Replay::LoadHeader(const std::string& filename, const bool load_extended_header)
{
	// Datei �ffnen
	if(!file.Open(filename.c_str(),OFM_READ))
		return false;

	// Version �berpr�fen
	// Signatur und Version einlesen
	if(!ValidateFile(file,6,REPLAY_SIGNATURE,REPLAY_VERSION))
	{
		LOG.lprintf("Replay::Load: ERROR: File \"%s\" is not a valid RTTR replay!\n", filename.c_str());
		return false;
	}

	// Zeitstempel
	file.ReadRawData(&save_time,8);
	// NWF-L�nge
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
	// Addoneinstellungen laden
	ReadAddonSettings(file);

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

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
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

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
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

	// L�nge der Daten
	file.WriteUnsignedShort(length);
	// Daten
	file.WriteRawData(data,length);

	// bei ungerader 4er position aufrunden
	while(file.Tell() % 4)
		file.WriteSignedChar(0);

	// Sofort rein damit
	file.Flush();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
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

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
Replay::ReplayCommand Replay::ReadRCType()
{
	// Type auslesen
	return ReplayCommand(file.ReadUnsignedChar());
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
void Replay::ReadChatCommand(unsigned char * player, unsigned char  * dest, std::string& str)
{
	*player = file.ReadUnsignedChar();
	*dest = file.ReadUnsignedChar();
	file.ReadLongString(str);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
void Replay::ReadGameCommand(unsigned short *length, unsigned char ** data)
{
	*length = file.ReadUnsignedShort();
	*data = new unsigned char[*length];
	file.ReadRawData(*data,*length);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
void Replay::UpdateLastGF(const unsigned last_gf)
{
	// An die Stelle springen
	file.Seek(last_gf_file_pos,SEEK_SET);
	// Dorthin schreiben
	file.WriteUnsignedInt(last_gf);
	// Wieder ans Ende springen
	file.Seek(0,SEEK_END);
}
