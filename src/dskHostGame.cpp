// $Id: dskHostGame.cpp 5853 2010-01-04 16:14:16Z FloSoft $
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
#include "dskHostGame.h"
#include "dskGameLoader.h"

#include "WindowManager.h"
#include "Loader.h"
#include "GameClient.h"
#include "GameServer.h"
#include "controls.h"
#include "LobbyClient.h"

#include "dskDirectIP.h"
#include "dskLobby.h"
#include "iwMsgbox.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p dskHostGame.
 *
 *  @author Devil
 *  @author FloSoft
 */
dskHostGame::dskHostGame() :
	Desktop(LOADER.GetImageN("setup015", 0)), temppunkte(1000)
{
	// Kartenname
	AddText(0, 400, 5,GAMECLIENT.GetGameName().c_str(), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, LargeFont);

	// "Spielername"
	AddText(10, 95, 40, _("Player Name"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Einstufung"
	AddText(11, 205, 40, _("Classification"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Volk"
	AddText(12, 285, 40, _("Race"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Farbe"
	AddText(13, 355, 40, _("Color"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Team"
	AddText(14, 405, 40, _("Team"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Bereit"
	AddText(15, 465, 40, _("Ready?"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Ping"
	AddText(16, 515, 40, _("Ping"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	// "Verschieben" (nur bei Savegames!)
	if(GAMECLIENT.IsSavegame())
		AddText(17, 645, 40, _("Past player"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);

	// Chatfenster
	AddChatCtrl(1, 20, 310, 360, 218, TC_GREY, NormalFont);
	// Edit für Chatfenster
	AddEdit(4, 20, 530, 360, 22, TC_GREY, NormalFont);

	// "Spiel starten"
	ctrlButton *start = AddTextButton(2, 600, (GAMECLIENT.IsHost() ? 560 : 530), 180, 22, TC_GREEN2, _("Start game"),NormalFont);
	if(!GAMECLIENT.IsHost())
		start->SetVisible(false);

	// "Zurück"
	AddTextButton(3, (GAMECLIENT.IsHost() ? 400 : 600), 560, 180, 22, TC_RED1, _("Return"),NormalFont);

	// "Teams sperren"
	AddCheckBox(20, 400, 460, 180, 26, TC_GREY, _("Lock teams:"), NormalFont, !GAMECLIENT.IsHost()||GAMECLIENT.IsSavegame());
	// "Gemeinsame Team-Sicht"
	AddCheckBox(19, 600, 460, 180, 26, TC_GREY, _("Shared team view"), NormalFont, !GAMECLIENT.IsHost()||GAMECLIENT.IsSavegame());

	// "Enhancements"
	AddText(21, 400, 499, _("Enhancements:"), COLOR_YELLOW, 0, NormalFont);
	AddTextButton(22, 600, 495, 180, 22, TC_GREEN1, (GAMECLIENT.IsHost() ? _("Change Settings...") : _("View Settings...")),NormalFont);

	ctrlComboBox *combo;

	// umgedrehte Reihenfolge, damit die Listen nicht dahinter sind
	// "Abriss-Verbot"
	AddText(29, 400, 435, _("Demolition prohibition:"), COLOR_YELLOW, 0, NormalFont);
	combo = AddComboBox(39, 600, 430, 180, 20, TC_GREY, NormalFont, 100, !GAMECLIENT.IsHost()||GAMECLIENT.IsSavegame());
	combo->AddString(_("Off"));
	combo->AddString(_("When attack"));
	combo->AddString(_("Near frontiers"));

	// "Aufklärung"
	AddText(30, 400, 405, _("Exploration:"), COLOR_YELLOW, 0, NormalFont);
	combo = AddComboBox(40, 600, 400, 180, 20, TC_GREY, NormalFont, 100, !GAMECLIENT.IsHost()||GAMECLIENT.IsSavegame());
	combo->AddString(_("Off (all visible)"));
	combo->AddString(_("Classic (Settlers 2)"));
	combo->AddString(_("Fog of War"));
	combo->AddString(_("FoW - all explored"));

	// "Waren zu Beginn"
	AddText(31, 400, 375, _("Goods at start:"), COLOR_YELLOW, 0, NormalFont);
	combo = AddComboBox(41, 600, 370, 180, 20, TC_GREY, NormalFont, 100, !GAMECLIENT.IsHost()||GAMECLIENT.IsSavegame());
	combo->AddString(_("Low"));
	combo->AddString(_("Normal"));
	combo->AddString(_("A lot"));

	// "Spielziel"
	AddText(32, 400, 345, _("Goals:"), COLOR_YELLOW, 0, NormalFont);
	combo = AddComboBox(42, 600, 340, 180, 20, TC_GREY, NormalFont, 100, !GAMECLIENT.IsHost()||GAMECLIENT.IsSavegame());
	combo->AddString(_("None")); // Kein Spielziel
	combo->AddString(_("Conquer 3/4 of map")); // Besitz 3/4 des Landes
	combo->AddString(_("Total domination")); // Alleinherrschaft

	// "Geschwindigkeit"
	AddText(33, 400, 315, _("Speed:"), COLOR_YELLOW, 0, NormalFont);
	combo = AddComboBox(43, 600, 310, 180, 20, TC_GREY, NormalFont, 100, !GAMECLIENT.IsHost());
	combo->AddString(_("Very slow")); // Sehr Langsam
	combo->AddString(_("Slow")); // Langsam
	combo->AddString(_("Normal")); // Normal
	combo->AddString(_("Fast")); // Schnell
	combo->AddString(_("Very fast")); // Sehr Schnell

	// Karte laden, um Kartenvorschau anzuzeigen
	if(GameClient::inst().GetMapType() == MAPTYPE_OLDMAP)
	{
		// Map laden
		libsiedler2::ArchivInfo ai;
		// Karteninformationen laden
		if(libsiedler2::loader::LoadMAP(GameClient::inst().GetMapPath().c_str(), &ai) == 0)
		{
			glArchivItem_Map *map = static_cast<glArchivItem_Map*>(ai.get(0));
			ctrlPreviewMinimap * preview = AddPreviewMinimap(70,560,40,220,220,map);

			// Titel der Karte, Y-Position relativ je nach Höhe der Minimap festlegen, daher nochmals danach
			// verschieben, da diese Position sonst skaliert wird!
			ctrlText * text = AddText(71,670,0,_("Map: ") +  GameClient::inst().GetMapTitle(),COLOR_YELLOW,glArchivItem_Font::DF_CENTER,NormalFont);
			text->Move(text->GetX(false),preview->GetY(false)+preview->GetBottom()+10);
		}
	}

	// Alle Spielercontrols erstellen
	for(unsigned char i = GAMECLIENT.GetPlayerCount(); i; --i)
		UpdatePlayerRow(i-1);

	//// GGS aktualisieren, zum ersten Mal
	this->CI_GGSChanged(GameClient::inst().GetGGS());

	LOBBYCLIENT.SetInterface(this);
	if(LOBBYCLIENT.LoggedIn())
		LOBBYCLIENT.SendServerJoinRequest();
	GAMECLIENT.SetInterface(this);
}

void dskHostGame::UpdatePlayerRow(const unsigned row)
{
	unsigned cy = 80 + row * 30;
	TextureColor tc = (row&1 ? TC_GREY : TC_GREEN2);

	// Alle Controls erstmal zerstören (die ganze Gruppe)
	DeleteCtrl(58-row);
	// und neu erzeugen
	ctrlGroup *group = AddGroup(58-row, scale);

	std::string name;
	// Name
	switch(GAMECLIENT.GetPlayer(row)->ps)
	{
	default:
		name = "";
		break;
	case PS_OCCUPIED:
	case PS_KI:
		{
			name = GAMECLIENT.GetPlayer(row)->name;
		} break;
	case PS_FREE:
	case PS_RESERVED:
		{
			// Offen
			name = _("Open");
		} break;
	case PS_LOCKED:
		{
			// Geschlossen
			name = _("Closed");
		} break;
	//case PS_KI:
	//	{
	//		// KI-Spieler
	//		name = _("AI Player");
	//	} break;
	}

	
	if(GetCtrl<ctrlPreviewMinimap>(70))
	{
		if(GAMECLIENT.GetPlayer(row)->ps == PS_OCCUPIED ||
		   GAMECLIENT.GetPlayer(row)->ps == PS_KI)
		   // Nur KIs und richtige Spieler haben eine Farbe auf der Karte
			GetCtrl<ctrlPreviewMinimap>(70)->SetPlayerColor(row,COLORS[GAMECLIENT.GetPlayer(row)->color]);
		else
			// Keine richtigen Spieler --> Startposition auf der Karte ausblenden
			GetCtrl<ctrlPreviewMinimap>(70)->SetPlayerColor(row,0);
	}


	// Spielername, beim Hosts Spielerbuttons, aber nich beim ihm selber, er kann sich ja nich selber kicken!
	ctrlBaseText * text;
	if(GAMECLIENT.IsHost() && !GAMECLIENT.GetPlayer(row)->is_host)
		text = group->AddTextButton(1, 20, cy, 150, 22, tc, name.c_str(), NormalFont);
	else
		text = group->AddDeepening(1, 20, cy, 150, 22, tc, name.c_str(), NormalFont, COLOR_YELLOW);


	// Is das der Host? Dann farblich markieren
		if(GAMECLIENT.GetPlayer(row)->is_host == true)
			text->SetColor(0xFF00FF00);



	// Bei geschlossenem nicht sichtbar
	if(GAMECLIENT.GetPlayer(row)->ps == PS_OCCUPIED || GAMECLIENT.GetPlayer(row)->ps == PS_KI)
	{
		/// @todo Einstufung ( "%d" )
		group->AddVarDeepening(2, 180, cy, 50, 22, tc, _("%d"), NormalFont, COLOR_YELLOW, 1, &temppunkte);

		// Host kann nur das Zeug von der KI noch mit einstellen
		if(((GAMECLIENT.IsHost() && GAMECLIENT.GetPlayer(row)->ps == PS_KI) || GAMECLIENT.GetPlayerID() == row) && !GAMECLIENT.IsSavegame())
		{
			// Volk
			group->AddTextButton( 3, 240, cy, 90, 22, tc, _("Africans"),NormalFont);
			// Farbe
			//group->AddTextButton( 4, 340, cy, 30, 22, tc, _("T"),NormalFont);
			group->AddColorButton( 4, 340, cy, 30, 22, tc, 0 );
			// Team
			group->AddTextButton( 5, 380, cy, 50, 22, tc, _("-"),NormalFont);
		}
		else
		{
			// Volk
			group->AddDeepening( 3, 240, cy, 90, 22, tc, _("Africans"), NormalFont, COLOR_YELLOW);
			// Farbe
			//group->AddDeepening( 4, 340, cy, 30, 22, tc, _("T"), NormalFont, COLOR_YELLOW);
			group->AddColorDeepening( 4, 340, cy, 30, 22, tc, 0);
			// Team
			group->AddDeepening( 5, 380, cy, 50, 22, tc, _("-"), NormalFont, COLOR_YELLOW);
		}

		// Bereit (nicht bei KIs und Host)
		if(GAMECLIENT.GetPlayer(row)->ps == PS_OCCUPIED && !GAMECLIENT.GetPlayer(row)->is_host)
			group->AddCheckBox(6, 450, cy, 22, 22, tc, EMPTY_STRING, NULL, (GAMECLIENT.GetPlayerID() != row) );

		// Ping ( "%d" )
		ctrlVarDeepening *ping = group->AddVarDeepening(7, 490, cy, 50, 22, tc, _("%d"), NormalFont, COLOR_YELLOW, 1, &GAMECLIENT.GetPlayer(row)->ping);

		// Verschieben (nur bei Savegames und beim Host!)
		if(GAMECLIENT.IsSavegame() && GAMECLIENT.GetPlayer(row)->ps == PS_OCCUPIED)
		{
			ctrlComboBox * combo = group->AddComboBox(8,570,cy,150,22,tc,NormalFont,150,!GAMECLIENT.IsHost());

			// Mit den alten Namen füllen
			for(unsigned i = 0;i<GAMECLIENT.GetPlayerCount();++i)
			{
				if(GAMECLIENT.GetPlayer(i)->origin_name.length())
				{
					combo->AddString(GAMECLIENT.GetPlayer(i)->origin_name.c_str());
					if(i == row)
						combo->SetSelection(combo->GetCount()-1);
				}
			}
		}


		// Ping bei KI und Host ausblenden
		if(GAMECLIENT.GetPlayer(row)->ps == PS_KI || GAMECLIENT.GetPlayer(row)->is_host)
			ping->SetVisible(false);


		// Felder ausfüllen
		ChangeNation(row,GAMECLIENT.GetPlayer(row)->nation);
		ChangeTeam(row,GAMECLIENT.GetPlayer(row)->team);
		ChangePing(row);
		ChangeReady(row,GAMECLIENT.GetPlayer(row)->ready);
		ChangeColor(row,GAMECLIENT.GetPlayer(row)->color);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Methode vor dem Zeichnen
 *
 *  @author OLiver
 */
void dskHostGame::Msg_PaintBefore()
{
	GetCtrl<ctrlEdit>(4)->SetFocus();
}

void dskHostGame::Msg_Group_ButtonClick(const unsigned int group_id, const unsigned int ctrl_id)
{
	unsigned player_id = 8 - (group_id - 50);

	switch(ctrl_id)
	{
	// Klick auf Spielername
	case 1:
		{
			if(GAMECLIENT.IsHost())
				GAMESERVER.TogglePlayerState(player_id);
		} break;
	// Volk
	case 3:
		{
			TogglePlayerReady(player_id, false);

			if(GAMECLIENT.IsHost())
				GAMESERVER.TogglePlayerNation(player_id);
			if(player_id == GAMECLIENT.GetPlayerID())
			{
					GAMECLIENT.Command_ToggleNation();
					GAMECLIENT.GetLocalPlayer()->nation = Nation((unsigned(GAMECLIENT.GetLocalPlayer()->nation) + 1) % 4);
					ChangeNation(GAMECLIENT.GetPlayerID(),GAMECLIENT.GetLocalPlayer()->nation);
				}
			} break;

	// Farbe
	case 4:
	{
		TogglePlayerReady(player_id, false);

		if(GAMECLIENT.IsHost())
			GAMESERVER.TogglePlayerColor(player_id);
		if(player_id == GAMECLIENT.GetPlayerID())
		{
			GAMECLIENT.Command_ToggleColor();

			GameClientPlayer *player = GAMECLIENT.GetLocalPlayer();
			bool reserved_colors[PLAYER_COLORS_COUNT];
			memset(reserved_colors, 0, sizeof(bool) * PLAYER_COLORS_COUNT);

			for(unsigned char cl = 0; cl < GAMECLIENT.GetPlayerCount(); ++cl)
			{
				GameClientPlayer *others = GAMECLIENT.GetPlayer(cl);

				if( (player != others) && ( (others->ps == PS_OCCUPIED) || (others->ps == PS_KI) ) )
					reserved_colors[others->color] = true;
			}
			do {
				player->color = (player->color + 1) % PLAYER_COLORS_COUNT;
			} while(reserved_colors[player->color]);
			ChangeColor(GAMECLIENT.GetPlayerID(), player->color);
		}

		// Start-Farbe der Minimap ändern
	} break;

	// Team
	case 5:
		{
			TogglePlayerReady(player_id, false);

			if(GAMECLIENT.IsHost())
				GAMESERVER.TogglePlayerTeam(player_id);
			if(player_id == GAMECLIENT.GetPlayerID())
			{
				GAMECLIENT.Command_ToggleTeam();
				GAMECLIENT.GetLocalPlayer()->team = Team((GAMECLIENT.GetLocalPlayer()->team + 1) % TEAM_COUNT);
				ChangeTeam(GAMECLIENT.GetPlayerID(),GAMECLIENT.GetLocalPlayer()->team);
			}
		} break;
	}
}

void dskHostGame::Msg_Group_CheckboxChange(const unsigned int group_id, const unsigned int ctrl_id, const bool checked)
{
	unsigned player_id = 8 - (group_id - 50);

	// Bereit
	if(player_id < 8)
		TogglePlayerReady(player_id, checked);
}

void dskHostGame::Msg_Group_ComboSelectItem(const unsigned int group_id, const unsigned int ctrl_id, const unsigned short selection)
{
	unsigned player_id = 8 - (group_id - 50);

	// Spieler wurden vertauscht

	// 2. Player herausfinden (Strings vergleichen
	unsigned player2;
	for(player2 = 0;player2<GAMECLIENT.GetPlayerCount();++player2)
	{
		if(GAMECLIENT.GetPlayer(player2)->origin_name == GetCtrl<ctrlGroup>(group_id)->
			GetCtrl<ctrlComboBox>(8)->GetText(selection))
			break;
	}

	// Keinen Namen gefunden?
	if(player2 == GAMECLIENT.GetPlayerCount())
	{
		LOG.lprintf("dskHostGame: ERROR: No Origin Name found, stop swapping!\n");
		return;
	}

	GameServer::inst().SwapPlayer(player_id,player2);
}

void dskHostGame::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	// Zurück
	case 3:
	{
		if(GAMECLIENT.IsHost())
			GAMESERVER.Stop();

		GAMECLIENT.Stop();

		if(LOBBYCLIENT.LoggedIn())
			WindowManager::inst().Switch(new dskLobby);
		else
			// Hauptmenü zeigen
			WindowManager::inst().Switch(new dskDirectIP);

	} break;
	// Starten
	case 2:
	{
		if(GAMECLIENT.IsHost())
		{
			if(!GAMESERVER.StartGame())
				WindowManager::inst().Show(new iwMsgbox(_("Error"), _("Game can only be started as soon as everybody has a unique color,everyone is ready and all free slots are closed."), this, MSB_OK, MSB_EXCLAMATIONRED, 10));
		}
	} break;
	case 102:
	{
		CreateMapPreview();
	} break;
	}
}

void dskHostGame::Msg_EditEnter(const unsigned int ctrl_id)
{
	GAMECLIENT.Command_Chat(GetCtrl<ctrlEdit>(4)->GetText(),CD_ALL);
	GetCtrl<ctrlEdit>(4)->SetText("");
}

void dskHostGame::Msg_MsgBoxResult(const unsigned msgbox_id, const MsgboxResult mbr)
{
	if(msgbox_id == 0) // Verbindung zu Server verloren?
	{
		GAMECLIENT.Stop();

		if(LOBBYCLIENT.LoggedIn()) // steht die Lobbyverbindung noch?
			WindowManager::inst().Switch(new dskLobby);
		else
			WindowManager::inst().Switch(new dskDirectIP);
	}
}

void dskHostGame::Msg_ComboSelectItem(const unsigned int ctrl_id, const unsigned short selection)
{
	switch(ctrl_id)
	{
	default:
		break;

	case 43: // Geschwindigkeit
	case 42: // Ziel
	case 41: // Waren
	case 40: // Aufklärung
	case 39: // Abriss-Verbot
		{
			// GameSettings wurden verändert, resetten
			UpdateGGS();
		} break;
	}
}

void dskHostGame::Msg_CheckboxChange(const unsigned int ctrl_id, const bool checked)
{

	switch(ctrl_id)
	{
	default:
		break;
	case 19: // Team-Sicht
	case 20: // Teams
		{
			// GameSettings wurden verändert, resetten
			UpdateGGS();
		} break;
	}
}


void dskHostGame::UpdateGGS()
{
	// Geschwindigkeit
	ggs.game_speed = static_cast<GlobalGameSettings::GameSpeed>(GetCtrl<ctrlComboBox>(43)->GetSelection());
	// Spielziel
	ggs.game_objective = static_cast<GlobalGameSettings::GameObjective>(GetCtrl<ctrlComboBox>(42)->GetSelection());
	// Waren zu Beginn
	ggs.start_wares = static_cast<GlobalGameSettings::StartWares>(GetCtrl<ctrlComboBox>(41)->GetSelection());
	// Aufklärung
	ggs.exploration = static_cast<GlobalGameSettings::Exploration>(GetCtrl<ctrlComboBox>(40)->GetSelection());
	// Abriss-Verbot
	ggs.demolition_prohibition = static_cast<GlobalGameSettings::DemolitionProhibition>(GetCtrl<ctrlComboBox>(39)->GetSelection());
	// Teams gesperrt
	ggs.lock_teams = GetCtrl<ctrlCheck>(20)->GetCheck();
	// Team sicht
	ggs.team_view = GetCtrl<ctrlCheck>(19)->GetCheck();

	// An Server übermitteln
	GameServer::inst().ChangeGlobalGameSettings(ggs);
}

void dskHostGame::ChangeTeam(const unsigned i, const unsigned char nr)
{
	const std::string teams[6] =
	{ "-","?", "1","2","3","4", };

	GetCtrl<ctrlGroup>(58-i)->GetCtrl<ctrlBaseText>(5)->SetText(teams[nr]);
}

void dskHostGame::ChangeReady(const unsigned i, const bool ready)
{
	if(ctrlCheck * check = GetCtrl<ctrlGroup>(58-i)->GetCtrl<ctrlCheck>(6))
		check->SetCheck(ready);
}

void dskHostGame::ChangeNation(const unsigned i, const Nation nation)
{
	const std::string nations[4] =
	{ _("Africans"), _("Japaneses"), _("Romans"), _("Vikings") };
	GetCtrl<ctrlGroup>(58-i)->GetCtrl<ctrlBaseText>(3)->SetText(nations[nation]);
}

void dskHostGame::ChangePing(const unsigned i)
{
	unsigned int color = COLOR_RED;

	// Farbe bestimmen
	if(GAMECLIENT.GetPlayer(id)->ping < 300)
		color = COLOR_GREEN;
	else if(GAMECLIENT.GetPlayer(id)->ping < 800 )
		color = COLOR_YELLOW;

	// und setzen
	GetCtrl<ctrlGroup>(58-i)->GetCtrl<ctrlVarDeepening>(7)->SetColor(color);
}

void dskHostGame::ChangeColor(const unsigned i, const unsigned char color)
{
	GetCtrl<ctrlGroup>(58-i)->GetCtrl<ColorControlInterface>(4)->SetColor(COLORS[color]);

	// Minimap-Startfarbe ändern
	if(GetCtrl<ctrlPreviewMinimap>(70))
		GetCtrl<ctrlPreviewMinimap>(70)->SetPlayerColor(i,COLORS[color]);
}

void dskHostGame::TogglePlayerReady(unsigned char player, bool ready)
{
	/*if(GAMECLIENT.IsHost())
		GAMESERVER.TogglePlayerReady(player);*/
	if(player == GAMECLIENT.GetPlayerID())
	{
		GAMECLIENT.GetLocalPlayer()->ready = (GAMECLIENT.IsHost() ? true : ready);
		GAMECLIENT.Command_ToggleReady();
		ChangeReady(GAMECLIENT.GetPlayerID(), GAMECLIENT.GetLocalPlayer()->ready);
	}
}

void dskHostGame::CreateMapPreview()
{
	//unsigned short w = gwg->GetWidth();
	//unsigned short h = gwg->GetHeight();

	//unsigned char *buffer = new unsigned char[w*2*h*2];
	//memset(buffer, libsiedler2::TRANSPARENT_INDEX, w*2*h*2);

	//const unsigned char *t1 = GetMap()->GetLayer(MAP_TERRAIN1);
	//const unsigned char *t2 = GetMap()->GetLayer(MAP_TERRAIN2);

	//const unsigned char *colors = NULL;

	//switch(GetMap()->getHeader().getGfxSet())
	//{
	//case 0: // Grünland
	//	{
	//		const unsigned char c[256] = {
	//			45, // steppe
	//			77, // berg
	//			156, // schnee
	//			0,
	//			53, // wüste
	//			62, // wasser
	//			0,
	//			43, // blumenwiese
	//			42, // gras
	//			43, // gras 2
	//			44, // gras 3
	//			79, // berg 2
	//			0,
	//			0,
	//			47, // strand
	//			42, // langes gras
	//			0,
	//			0,
	//			78 // bergland
	//		};
	//		colors = c;
	//	} break;
	//case 1: // Ödland
	//case 2: // Winterwelt
	//	// todo
	//	break;
	//}

	//if(!colors)
	//	return;

	//for(unsigned short x = 0; x < w; ++x)
	//{
	//	for(unsigned short y = 0; y < h; ++y)
	//	{
	//		buffer[y*2*w*2 + x*2 + 0] = colors[t1[y*w + x]];
	//		buffer[y*2*w*2 + x*2 + 1] = colors[t1[y*w + x]];
	//		buffer[(y*2+1)*w*2 + x*2 + 0] = colors[t1[y*w + x]];
	//		buffer[(y*2+1)*w*2 + x*2 + 1] = colors[t2[y*w + x]];
	//	}
	//}

	//preview.setFilter(4444);
	//preview.setFilter(GL_LINEAR);
	//preview.create(w*2, h*2, buffer, w*2, h*2, libsiedler2::FORMAT_PALETTED, LOADER.GetPaletteN("pal5"));

	//delete[] buffer;

	//// "Preview"
	//AddImage(100, 500, 40, &preview);
	//AddButton(102, 400, 560, 180, 22, TC_RED1, 0, "Preview", NULL, NormalFont);
}

void dskHostGame::CI_NewPlayer(const unsigned player_id)
{
	// Spielername setzen
	UpdatePlayerRow(player_id);
}

void dskHostGame::CI_PlayerLeft(const unsigned player_id)
{
	UpdatePlayerRow(player_id);
}

void dskHostGame::CI_GameStarted(GameWorldViewer *gwv)
{
	// Desktop wechseln
	WindowManager::inst().Switch(new dskGameLoader(gwv));
}

void dskHostGame::CI_PSChanged(const unsigned player_id, const PlayerState ps)
{
	UpdatePlayerRow(player_id);
}

void dskHostGame::CI_NationChanged(const unsigned player_id, const Nation nation)
{
	ChangeNation(player_id,nation);
}

void dskHostGame::CI_TeamChanged(const unsigned player_id, const unsigned char team)
{
	ChangeTeam(player_id,team);
}

void dskHostGame::CI_ColorChanged(const unsigned player_id, const unsigned char color)
{
	ChangeColor(player_id, color);
}

void dskHostGame::CI_PingChanged(const unsigned player_id, const unsigned short ping)
{
	ChangePing(player_id);
}

void dskHostGame::CI_ReadyChanged(const unsigned player_id, const bool ready)
{
	ChangeReady(player_id,ready);
}

void dskHostGame::CI_PlayersSwapped(const unsigned player1, const unsigned player2)
{
	// Spieler wurden vertauscht, beide Reihen updaten
	UpdatePlayerRow(player1);
	UpdatePlayerRow(player2);
}

void dskHostGame::CI_GGSChanged(const GlobalGameSettings& ggs)
{
	this->ggs = ggs;
	
	// Geschwindigkeit
	GetCtrl<ctrlComboBox>(43)->SetSelection(static_cast<unsigned short>(ggs.game_speed));

	// Ziel
	GetCtrl<ctrlComboBox>(42)->SetSelection(static_cast<unsigned short>(ggs.game_objective));

	// Waren
	GetCtrl<ctrlComboBox>(41)->SetSelection(static_cast<unsigned short>(ggs.start_wares));

	// Aufklärung
	GetCtrl<ctrlComboBox>(40)->SetSelection(static_cast<unsigned short>(ggs.exploration));

	// Abriss-Verbot
	GetCtrl<ctrlComboBox>(39)->SetSelection(static_cast<unsigned short>(ggs.demolition_prohibition));

	// Teams
	GetCtrl<ctrlCheck>(20)->SetCheck(ggs.lock_teams);

	// Team-Sicht
	GetCtrl<ctrlCheck>(19)->SetCheck(ggs.team_view);


	TogglePlayerReady(GAMECLIENT.GetPlayerID(), false);
}


void dskHostGame::CI_Chat(const unsigned player_id, const ChatDestination cd, const std::string& msg)
{
	if(player_id != 0xFFFFFFFF) // Keine Lobby-Nachrichten anzeigen
	{
		// Zeit holen
		char time_string[64];
		TIME.FormatTime(time_string, "(%H:%i:%s)", NULL);

		GetCtrl<ctrlChat>(1)->AddMessage(time_string,GAMECLIENT.GetPlayer(player_id)->name.c_str(),
			COLORS[GAMECLIENT.GetPlayer(player_id)->color],msg.c_str(),0xFFFFFF00);
	}
}

void dskHostGame::CI_Error(const ClientError ce)
{
	switch(ce)
	{
	case CE_INCOMPLETEMESSAGE:
	case CE_CONNECTIONLOST:
		{
			// Verbindung zu Server abgebrochen
			WindowManager::inst().Show(new iwMsgbox(_("Error"), _("Lost connection to server!"), this, MSB_OK, MSB_EXCLAMATIONRED, 0));
		} break;
	default: break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  (Lobby-)Status: Benutzerdefinierter Fehler (kann auch Conn-Loss o.ä sein)
 *
 *  @author FloSoft
 */
void dskHostGame::LC_Status_Error(const std::string &error)
{
	WindowManager::inst().Show(new iwMsgbox(_("Error"), error, this, MSB_OK, MSB_EXCLAMATIONRED, 0));
}

