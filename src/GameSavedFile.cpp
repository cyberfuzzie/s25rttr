// $Id: GameSavedFile.cpp 5999 2010-02-11 09:53:02Z FloSoft $
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
#include "GameSavedFile.h"

#include "AddonManager.h"
#include "GamePlayerInfo.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
SavedFile::SavedFile() : save_time(0), player_count(0), players(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
SavedFile::~SavedFile()
{
	delete[] players;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
void SavedFile::WriteVersion(BinaryFile& file, unsigned int signature_length, const char* signature, unsigned short version)
{
	// Signatur schreiben
	file.WriteRawData(signature,signature_length);

	// Version vom Programm reinschreiben (mal 0 mit reinschreiben, damits ne runde 8 ergibt!)
	file.WriteRawData(GetWindowRevision(), 8);

	// Version des Save-Formats
	file.WriteUnsignedShort(version);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
bool SavedFile::ValidateFile(BinaryFile& file, unsigned int signature_length, const char* signature, unsigned short version)
{
	char read_signature[32];

	file.ReadRawData(read_signature,signature_length);

	// Signatur �berpr�fen
	if(memcmp(read_signature, signature, signature_length))
	{
		// unterscheiden sich! --> raus
		LOG.lprintf("SavedFile::Load: ERROR: Not a valid file!\n");
		return false;
	}

	// Programmversion �berspringen
	file.Seek(8, SEEK_CUR);

	// Version �berpr�fen
	unsigned short read_version = file.ReadUnsignedShort();
	if(read_version != version)
	{
		// anderes Dateiformat --> raus
		LOG.lprintf("SavedFile::Load: ERROR: Old file version (version: %u; expected: %u)!\n",read_version,version);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
void SavedFile::WritePlayerData(BinaryFile& file)
{
	// Spielerdaten
	for(unsigned char i = 0; i < player_count; ++i)
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

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *  @author OLiver
 */
void SavedFile::ReadPlayerData(BinaryFile& file)
{
	// ggf. wieder l�schen
	delete[] players;

	players = new SavedFile::Player[player_count];
	for(unsigned char i = 0; i < player_count; ++i)
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

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die GlobalGameSettings in die Datei.
 *
 *  @author OLiver
 */
void SavedFile::WriteGGS(BinaryFile& file)
{
	Serializer ser;
	ggs.Serialize(&ser);

	file.WriteUnsignedInt(ser.GetLength());
	file.WriteRawData(ser.GetData(),ser.GetLength());
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liest die GlobalGameSettings aus der Datei.
 *
 *  @author OLiver
 */
void SavedFile::ReadGGS(BinaryFile& file)
{
	unsigned length = file.ReadUnsignedInt();
	unsigned char *buffer = new unsigned char[length];

	file.ReadRawData(buffer, length);
	Serializer ser(buffer, length);
	
	ggs.Deserialize(&ser);
	
	delete[] buffer;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Addoneinstellungen in die Datei.
 *
 *  @author FloSoft
 */
void SavedFile::WriteAddonSettings(BinaryFile& file)
{
	Serializer ser;
	ADDONMANAGER.Serialize(&ser);

	file.WriteUnsignedInt(ser.GetLength());
	file.WriteRawData(ser.GetData(), ser.GetLength());
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liest die Addoneinstellungen aus der Datei.
 *
 *  @author FloSoft
 */
void SavedFile::ReadAddonSettings(BinaryFile& file)
{
	unsigned length = file.ReadUnsignedInt();
	unsigned char *buffer = new unsigned char[length];

	file.ReadRawData(buffer, length);
	Serializer ser(buffer, length);
	
	ADDONMANAGER.Deserialize(&ser);
	
	delete[] buffer;
}