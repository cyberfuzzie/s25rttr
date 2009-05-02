// $Id: GameClientCommands.cpp 4784 2009-05-02 20:43:44Z OLiver $
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

#include "VideoDriverWrapper.h"
#include "noFlag.h"
#include "GameWorld.h"
#include "GameClientPlayer.h"

#include "GameServer.h"
#include "nobUsual.h"
#include "nobMilitary.h"
#include "nobBaseWarehouse.h"
#include "dskGameInterface.h"
#include "ClientInterface.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

void GameClient::ExecuteNC(const unsigned short nfc_type,const unsigned char player, const void * const data)
{
	// NC-Type rausfischen und dann je nach Message entscheiden, was zu tun ist
	// aufpassen, immer -2 Bytes bei den indizes, da hier nicht der typ mitgezählt wird!

	switch(nfc_type)
	{
	case NC_SETFLAG:
		{
			gw->SetFlag(static_cast<const unsigned short*>(data)[0],static_cast<const unsigned short*>(data)[1],player);
		} break;
	case NC_DESTROYFLAG:
		{
			gw->DestroyFlag(static_cast<const unsigned short*>(data)[0],static_cast<const unsigned short*>(data)[1]);
			// dem GUI Bescheid sagen, um evtl den Straßenbau abzubrechen
			ci->CI_FlagDestroyed(static_cast<const unsigned short*>(data)[0],static_cast<const unsigned short*>(data)[1]);
		} break;
	case NC_BUILDROAD:
		{
			gw->BuildRoad(player,static_cast<const unsigned short*>(data)[0] == 1,static_cast<const unsigned short*>(data)[1],
				static_cast<const unsigned short*>(data)[2],&static_cast<const unsigned char*>(data)[8],static_cast<const unsigned short*>(data)[3]);
		} break;
	case NC_DESTROYROAD:
		{
			gw->DestroyRoad(static_cast<const unsigned short*>(data)[0],static_cast<const unsigned short*>(data)[1],
				static_cast<const unsigned char*>(data)[4]);
		} break;
	case NC_CHANGEDISTRIBUTION:
		{
			GAMECLIENT.GetPlayer(player)->ChangeDistribution(static_cast<const unsigned char*>(data));
		} break;
	case NC_CHANGEBUILDORDER:
		{
			GAMECLIENT.GetPlayer(player)->ChangeBuildOrder(static_cast<const unsigned char*>(data)[0],
				&static_cast<const unsigned char*>(data)[1]);
		} break;
	case NC_SETBUILDINGSITE:
		{
			gw->SetBuildingSite(static_cast<BuildingType>(static_cast<const unsigned char*>(data)[4]),static_cast<const unsigned short*>(data)[0],static_cast<const unsigned short*>(data)[1],player);
		} break;
	case NC_DESTROYBUILDING:
		{
			gw->DestroyBuilding(static_cast<const unsigned short*>(data)[0],static_cast<const unsigned short*>(data)[1],player);
		} break;
	case NC_CHANGETRANSPORT:
		{
			GAMECLIENT.GetPlayer(player)->ConvertTransportData(static_cast<const unsigned char*>(data));
		} break;
	case NC_CHANGEMILITARY:
		{
			GAMECLIENT.GetPlayer(player)->ChangeMilitarySettings(static_cast<const unsigned char *>(data));
		} break;
	case NC_CHANGETOOLS:
		{
			GAMECLIENT.GetPlayer(player)->ChangeToolsSettings(static_cast<const unsigned char *>(data));
		} break;
	case NC_CALLGEOLOGIST:
		{
			// TODO: Verzögerungsbug abgefangen.
			unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetGOT() == GOT_FLAG)
				GetPlayer(player)->CallFlagWorker(x,y,JOB_GEOLOGIST);
		} break;
	case NC_CALLSCOUT:
		{
			// TODO: Verzögerungsbug abgefangen.
			unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetGOT() == GOT_FLAG)
				GetPlayer(player)->CallFlagWorker(x,y,JOB_SCOUT);
		} break;
	case NC_ATTACK:
		{
			gw->Attack(player,static_cast<const unsigned short*>(data)[0],static_cast<const unsigned short*>(data)[1],static_cast<const unsigned short*>(data)[2],(static_cast<const unsigned char*>(data)[6]==1)?true:false);
		} break;
	case NC_STOPPRODUCTION:
		{
			// Verzögerungsbug abfangen, erst nachsehen, ob das Gebäude noch existert
			unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetGOT() == GOT_NOB_USUAL)
				gw->GetSpecObj<nobUsual>(x,y)->StopProduction();
		} break;
	case NC_STOPGOLD:
		{
			// Verzögerungsbug abfangen, erst nachsehen, ob das Gebäude noch existert und dem Spieler noch gehört, der die Gold-
			// zufuhr umstellen will
			unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetGOT() == GOT_NOB_MILITARY)
			{
				if(gw->GetSpecObj<nobMilitary>(x,y)->GetPlayer() == player)
					gw->GetSpecObj<nobMilitary>(x,y)->StopGold();
			}
		} break;
	case NC_CHANGEINVENTORYSETTING:
		{
			// TODO: Verzögerungsbug abgefangen.
			unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetType() == NOP_BUILDING)
				gw->GetSpecObj<nobBaseWarehouse>(x,y)->ChangeRealInventorySetting(
				static_cast<const unsigned char*>(data)[4],static_cast<const unsigned char*>(data)[5],
				static_cast<const unsigned char*>(data)[6]);
		} break;

	case NC_CHANGEALLINVENTORYSETTINGS:
		{
			unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetType() == NOP_BUILDING)
				gw->GetSpecObj<nobBaseWarehouse>(x,y)->ChangeAllRealInventorySettings(
				static_cast<const unsigned char*>(data)[4],static_cast<const unsigned char*>(data)[5]);
		} break;
	case NC_INCREASERESERVE:
		{
			unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetType() == NOP_BUILDING)
				gw->GetSpecObj<nobBaseWarehouse>(x,y)->IncreaseReserveReal(
				static_cast<const unsigned char*>(data)[4]);
		} break;
	case NC_DECREASERESERVE:
		{
		unsigned short x = static_cast<const unsigned short*>(data)[0], y = static_cast<const unsigned short*>(data)[1];
			if(gw->GetNO(x,y)->GetType() == NOP_BUILDING)
				gw->GetSpecObj<nobBaseWarehouse>(x,y)->DecreaseReserveReal(
				static_cast<const unsigned char*>(data)[4]);
		} break;
	case NC_CHEAT_ARMAGEDDON:
		{
			gw->Armageddon();
		} break;
	case NC_SURRENDER:
		{
			GameClient::inst().GetPlayer(player)->Surrender();
		} break;
	default: return;
	}

	//printf("nc: %u\n",nfc_type);

}


void GameClient::NC_Nothing()
{
	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,2);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_NOTHING;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_SetFlag(const unsigned short x,const unsigned short y)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,6);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_SETFLAG;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_DestroyFlag(const unsigned short x,const unsigned short y)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,6);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_DESTROYFLAG;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_Road_Build(const bool boat_road,const unsigned short road_start_x, const unsigned short road_start_y, const unsigned char * route, const unsigned short road_length)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,10+road_length);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_BUILDROAD;
	static_cast<unsigned short*>(nfc->m_pData)[1] = boat_road ? 1 : 0;
	static_cast<unsigned short*>(nfc->m_pData)[2] = road_start_x;
	static_cast<unsigned short*>(nfc->m_pData)[3] = road_start_y;
	static_cast<unsigned short*>(nfc->m_pData)[4] = road_length;
	memcpy(&static_cast<unsigned short*>(nfc->m_pData)[5],route,road_length);


	nfc_queue.push_back(nfc);
}

void GameClient::NC_Road_Destroy(const unsigned short x, const unsigned short y, const unsigned char dir)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,7);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_DESTROYROAD;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	static_cast<unsigned char*>(nfc->m_pData)[6] = dir;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_ChangeDistribution(unsigned char * const distri_data)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,22);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGEDISTRIBUTION;
	memcpy(&static_cast<unsigned short*>(nfc->m_pData)[1],distri_data,20);
	nfc_queue.push_back(nfc);
}

void GameClient::NC_ChangeBuildOrder(const unsigned char order_type,const unsigned char * const order)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,34);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGEBUILDORDER;
	static_cast<unsigned char*>(nfc->m_pData)[2] = order_type;
	memcpy(&static_cast<unsigned char*>(nfc->m_pData)[3],order,31);
	nfc_queue.push_back(nfc);
}

void GameClient::NC_SetBuildingSite(const unsigned short x, const unsigned short y, const unsigned char type)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,7);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_SETBUILDINGSITE;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	static_cast<unsigned char*>(nfc->m_pData)[6] = type;
	nfc_queue.push_back(nfc);
}

void GameClient::NC_DestroyBuilding(const unsigned short x, const unsigned short y)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,6);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_DESTROYBUILDING;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	nfc_queue.push_back(nfc);
}

void GameClient::NC_ChangeTransport(const unsigned char * const transport_data)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,16);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGETRANSPORT;
	memcpy(&static_cast<unsigned short*>(nfc->m_pData)[1],transport_data,14);
	nfc_queue.push_back(nfc);
}

void GameClient::NC_ChangeMilitary(const unsigned char * const military_data)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,9);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGEMILITARY;
	memcpy(&static_cast<unsigned short*>(nfc->m_pData)[1],military_data,7);
	nfc_queue.push_back(nfc);
}

void GameClient::NC_ChangeTools(const unsigned char * const tools_data)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,14);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGETOOLS;
	memcpy(&static_cast<unsigned short*>(nfc->m_pData)[1],tools_data,12);
	nfc_queue.push_back(nfc);
}

void GameClient::NC_CallGeologist(const unsigned short x, const unsigned short y)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,6);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CALLGEOLOGIST;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	nfc_queue.push_back(nfc);
}

void GameClient::NC_CallScout(const unsigned short x, const unsigned short y)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,6);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CALLSCOUT;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	nfc_queue.push_back(nfc);
}

void GameClient::NC_Attack(const unsigned short x, const unsigned short y, const unsigned short soldiers_count,const bool strong_soldiers)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,9);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_ATTACK;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	static_cast<unsigned short*>(nfc->m_pData)[3] = soldiers_count;
	static_cast<unsigned char*>(nfc->m_pData)[8] = strong_soldiers ? 1 : 0;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_SwitchPlayer(const unsigned char new_id)
{
	// gucken, ob das auch legitim ist
	if(player_count > new_id)
	{
		if(players[new_id]->ps == PS_KI)
		{
			GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,3);

			static_cast<unsigned short*>(nfc->m_pData)[0] = NC_SWITCHPLAYER;
			static_cast<unsigned char*>(nfc->m_pData)[2] = new_id;

			nfc_queue.push_back(nfc);
		}
	}
}

void GameClient::NC_StopGold(const unsigned short x, const unsigned short y)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,6);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_STOPGOLD;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_StopProduction(const unsigned short x, const unsigned short y)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,6);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_STOPPRODUCTION;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;

	nfc_queue.push_back(nfc);
}

//void GameClient::NC_ChangeInventorySettings(const unsigned short x, const unsigned short y,
//		const unsigned char * const wares,const unsigned char * const figures)
//{
//	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,73);
//
//	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGEINVENTORYSETTINGS;
//	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
//	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
//	memcpy(&static_cast<unsigned char*>(nfc->m_pData)[6],wares,36);
//	memcpy(&static_cast<unsigned char*>(nfc->m_pData)[42],figures,31);
//
//	nfc_queue.push_back(nfc);
//}

void GameClient::NC_ChangeInventorySetting(const unsigned short x, const unsigned short y,unsigned char category,unsigned char state,unsigned char type)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,9);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGEINVENTORYSETTING;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	static_cast<unsigned char*>(nfc->m_pData)[6] = category;
	static_cast<unsigned char*>(nfc->m_pData)[7] = state;
	static_cast<unsigned char*>(nfc->m_pData)[8] = type;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_ChangeAllInventorySettings(const unsigned short x, const unsigned short y,const unsigned char category,const unsigned char state)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,8);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHANGEALLINVENTORYSETTINGS;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	static_cast<unsigned char*>(nfc->m_pData)[6] = category;
	static_cast<unsigned char*>(nfc->m_pData)[7] = state;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_IncreaseReserve(const unsigned short x, const unsigned short y, const unsigned char rank)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,7);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_INCREASERESERVE;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	static_cast<unsigned char*>(nfc->m_pData)[6] = rank;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_DecreaseReserve(const unsigned short x, const unsigned short y, const unsigned char rank)
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,7);

	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_DECREASERESERVE;
	static_cast<unsigned short*>(nfc->m_pData)[1] = x;
	static_cast<unsigned short*>(nfc->m_pData)[2] = y;
	static_cast<unsigned char*>(nfc->m_pData)[6] = rank;

	nfc_queue.push_back(nfc);
}

void GameClient::NC_CheatArmageddon()
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,2);
	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_CHEAT_ARMAGEDDON;
	nfc_queue.push_back(nfc);
}


void GameClient::NC_Surrender()
{
	// Nicht in der Pause
	if(framesinfo.pause || GetLocalPlayer()->IsDefeated())
		return;

	GameMessage * nfc = new GameMessage(NMS_NFC_COMMANDS,2);
	static_cast<unsigned short*>(nfc->m_pData)[0] = NC_SURRENDER;
	nfc_queue.push_back(nfc);
}



///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void GameClient::Command_SetFlag2(int x, int y, unsigned char player)
{
	gw->SetFlag(x, y, player);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Chatbefehl, hängt eine Textnachricht in die Sende-Queue.
 *
 *  @param[in] text        Der Text
 *  @param[in] destination Ziel der Nachricht
 *
 *  @author FloSoft
 */
void GameClient::Command_Chat(const std::string& text, const ChatDestination cd)
{
	// Replaymodus oder kein Text --> nichts senden
	if(GameClient::inst().IsReplayModeOn() || text.length() == 0)
		return;

	GameMessage *msg = send_queue.push(NMS_SERVER_CHAT);

	char *buffer = (char*)msg->alloc(1 + (unsigned int)text.length()+1);
	buffer[0] = static_cast<unsigned char>(cd);
	strncpy(&buffer[1], text.c_str(), text.length()+1);

	LOG.write(">>>NMS_SERVER_CHAT(%d, %s)\n", cd, text.c_str());
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void GameClient::Command_ToggleNation()
{
	send_queue.push(NMS_PLAYER_TOGGLENATION);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void GameClient::Command_ToggleTeam()
{
	send_queue.push(NMS_PLAYER_TOGGLETEAM);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  sendet den "Bereit"-Status.
 *
 *  @author FloSoft
 */
void GameClient::Command_ToggleReady()
{
	GameMessage *m = send_queue.push(NMS_PLAYER_READY);
	char *data = (char*)m->alloc(1);
	data[0] = GetLocalPlayer()->ready ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void GameClient::Command_ToggleColor()
{
	send_queue.push(NMS_PLAYER_TOGGLECOLOR);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  wechselt einen Spieler.
 *
 *  @param[in] old_id Alte Spieler-ID
 *  @param[in] new_id Neue Spieler-ID
 *
 *  @author OLiver
 *  @author FloSoft
 */
void GameClient::ChangePlayer(const unsigned char old_id, const unsigned char new_id)
{
	// Gleiche ID - wäre unsinnig zu wechseln
	if(old_id == new_id)
		return;

	players[old_id]->ps = PS_KI;
	players[new_id]->ps = PS_OCCUPIED;

	// Wenn wir betroffen waren, unsere ID neu setzen und BQ neu berechnen
	if(playerid == old_id)
	{
		playerid = new_id;

		// BQ überall neu berechnen
		for(unsigned y = 0; y < gw->GetHeight(); ++y)
		{
			for(unsigned x = 0; x < gw->GetWidth(); ++x)
				gw->SetBQ(x, y, new_id);
		}

		// Visuelle Einstellungen vom Spieler wieder holen
		GetVisualSettings();

		//// zum HQ hinscrollen
		//gw->MoveToMapObject(player->hqx,player->hqy);
		//GameClientPlayer *player = players[playerid]; // wegen GCC-Fehlermeldung auskommentiert
	}

	// GUI Bescheid sagen (um z.B. Schatten neu zu berechnen)
	if(ci)
		ci->CI_PlayersSwapped(old_id,new_id);
}

void GameClient::ChangeReplayPlayer(const unsigned new_id)
{
	unsigned old_id = playerid;

	if(old_id == new_id)
		// Unsinn auf den selben Spieler zu wechseln
		return;
	// Auch innerhalb der gültigen Spieler?
	if(new_id >= GameClient::inst().GetPlayerCount())
		return;
	// Und ein richtiger ehemaliger Spieler?
	if(GameClient::inst().GetPlayer(new_id)->ps != PS_KI &&
		GameClient::inst().GetPlayer(new_id)->ps != PS_OCCUPIED)
		return;


	playerid = new_id;

	// BQ überall neu berechnen
	for(unsigned y = 0; y < gw->GetHeight(); ++y)
	{
		for(unsigned x = 0; x < gw->GetWidth(); ++x)
			gw->SetBQ(x, y, new_id);
	}

	// GUI Bescheid sagen (um z.B. Schatten neu zu berechnen)
	if(ci)
		ci->CI_PlayersSwapped(old_id,new_id);
}

