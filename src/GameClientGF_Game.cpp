// $Id: GameClientGF_Game.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "GameClient.h"

#include "Random.h"

void GameClient::ExecuteGameFrame_Game()
{
	// Geschickte Network Commands der Spieler ausführen und ggf. im Replay aufzeichnen

	// Bei evtl. Spielerwechsel die IDs speichern, die "gewechselt" werden sollen
	unsigned char player_switch_old_id = 255, player_switch_new_id= 255;

	int checksum = Random::inst().GetCurrentRandomValue();

	for(unsigned char i = 0; i < player_count; ++i)
	{
		if(players[i]->ps == PS_OCCUPIED || players[i]->ps == PS_KI)
		{
			GameMessage *nfc = *players[i]->nfc_queue.begin();

			// Nachricht abwerfen :)
			players[i]->nfc_queue.pop_front();

			// Command im Replay aufzeichnen (wenn nicht gerade eins schon läuft xD)
			// Nur Commands reinschreiben, KEINE PLATZHALTER (nc_count = 0)
			if(static_cast<unsigned char*>(nfc->m_pData)[5] > 0 && !replay_mode)
			{
				// Aktuelle Checksumme reinschreiben
				*(int*)(static_cast<unsigned char*>(nfc->m_pData)+1) = checksum;
				replayinfo.replay.AddGameCommand(framesinfo.nr,nfc->m_uiLength,static_cast<unsigned char*>(nfc->m_pData));
			}

			// Das ganze Zeug soll die andere Funktion ausführen
			ExecuteAllNCs(static_cast<unsigned char*>(nfc->m_pData)+5,
				*static_cast<unsigned char*>(nfc->m_pData),
				&player_switch_old_id,&player_switch_new_id);

			delete nfc;
		}
	}

	// Frame ausführen
	em->NextGF();

	//LOG.lprintf("%d = %d - %d\n", framesinfo.nr / framesinfo.nwf_length, checksum, Random::inst().GetCurrentRandomValue());

	// Stehen eigene Commands an, die gesendet werden müssen?
	if(nfc_queue.size())
	{
		GameMessage *nfc_commands = send_queue.push(NMS_NFC_COMMANDS);

		// Größe des Buffers berechnen (+1 für Anzahl, jeweils +2 für Länge)
		for(list<GameMessage*>::iterator it = nfc_queue.begin();it.valid();++it)
			nfc_commands->m_uiLength+=((*it)->m_uiLength+2);

		// +4 für Checksumme, +1 für Anzahl Commands
		nfc_commands->m_uiLength+=5;

		nfc_commands->m_pData = new unsigned char[nfc_commands->m_uiLength];

		// Checksumme (4 Byte)
		static_cast<int*>(nfc_commands->m_pData)[0] = checksum;
		// Anzahl der Commands
		static_cast<unsigned char*>(nfc_commands->m_pData)[4] = static_cast<unsigned char>(nfc_queue.size());

		// Alle Commands in den Buffer kopieren
		unsigned pos = 5;
		for(list<GameMessage*>::iterator it = nfc_queue.begin();it.valid();++it)
		{
			unsigned short length = static_cast<unsigned short>((*it)->m_uiLength);
			// Länge des Commands (unsigned short)
			memcpy(&(static_cast<unsigned char*>(nfc_commands->m_pData)[pos]),&length,2);
			// Daten des Commands
			memcpy(&(static_cast<unsigned char*>(nfc_commands->m_pData)[pos+2]),(*it)->m_pData,(*it)->m_uiLength);

			pos+=((*it)->m_uiLength+2);

			// Message zerstören
			delete (*it);
		}

		// alles gesendet --> Liste löschen
		nfc_queue.clear();
	}
	else
	{
		// Ansonsten Nothing-Command senden, nur um zu sagen, dass wir den jetzigen Frame geschafft haben
		SendNothingNC(checksum);
	}

	// Evtl Spieler wechseln?
	if(player_switch_old_id != 255)
		ChangePlayer(player_switch_old_id, player_switch_new_id);
}
