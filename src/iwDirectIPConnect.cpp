// $Id: iwDirectIPConnect.cpp 4933 2009-05-24 12:29:23Z OLiver $
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
#include "iwDirectIPConnect.h"

#include "Loader.h"
#include "GameClient.h"
#include "controls.h"
#include "WindowManager.h"

#include "dskHostGame.h"
#include "Settings.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwDirectIPConnect.
 *
 *  @author OLiver
 */
iwDirectIPConnect::iwDirectIPConnect(unsigned int server_type)
	: IngameWindow(CGI_DIRECTIPCONNECT, 0xFFFF, 0xFFFF, 300, 250, _("Join Game"), GetImage(resource_dat, 41), true),
	server_type(server_type)
{
	ctrlEdit *host, *port;

	// "IP - Adresse vom Host"
	AddText(0, 20, 30, _("IP Address of Host:"), COLOR_YELLOW, 0, NormalFont);
	host = AddEdit(1, 20, 45, 260, 22, TC_GREEN2, NormalFont, 0, false, (server_type == NP_LOBBY),  true);

	// "Server-Port"
	AddText(2, 20, 80, _("Server-Port:"), COLOR_YELLOW, 0, NormalFont);
	port = AddEdit(3, 20, 95, 260, 22, TC_GREEN2, NormalFont, 0, false, (server_type == NP_LOBBY),  true);

	// "Passwort (falls vorhanden)"
	AddText(4, 20, 130, _("Password (if needed):"), COLOR_YELLOW, 0, NormalFont);
	AddEdit(5, 20, 145, 260, 22, TC_GREEN2, NormalFont, 0, false, false,  true);

	// Status
	AddText(6, 150, 180, EMPTY_STRING, COLOR_RED, glArchivItem_Font::DF_CENTER, NormalFont);

	// "Verbinden"
	AddTextButton(7, 20, 205, 125, 22, TC_GREEN2, _("Connect"),NormalFont);

	// "Zurück"
	AddTextButton(8, 155, 205, 125, 22, TC_RED1, _("Back"),NormalFont);

	host->SetFocus();
	host->SetText(Settings::inst().last_ip);
	port->SetText(GetTxt(client_txt, 3));

	// Client unser Window geben, damit er uns benachrichtigen kann
	GAMECLIENT.SetInterface(this);
}


void iwDirectIPConnect::Msg_EditChange(const unsigned int ctrl_id)
{
	// Statustext resetten
	SetText(EMPTY_STRING, COLOR_RED, true);
}

void iwDirectIPConnect::Msg_EditEnter(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 1:
		{
			ctrlEdit *host = GetCtrl<ctrlEdit>(1);
			ctrlEdit *port = GetCtrl<ctrlEdit>(3);
			ctrlEdit *pass = GetCtrl<ctrlEdit>(5);
			host->SetFocus(false);
			port->SetFocus(true);
			pass->SetFocus(false);
		} break;
	case 3:
		{
			ctrlEdit *host = GetCtrl<ctrlEdit>(1);
			ctrlEdit *port = GetCtrl<ctrlEdit>(3);
			ctrlEdit *pass = GetCtrl<ctrlEdit>(5);
			host->SetFocus(false);
			port->SetFocus(false);
			pass->SetFocus(true);
		} break;
	case 5:
		{
			Msg_ButtonClick(7);
		} break;
	}
}




void iwDirectIPConnect::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 7: // "Verbinden"
		{
			ctrlEdit *host = GetCtrl<ctrlEdit>(1);
			ctrlEdit *port = GetCtrl<ctrlEdit>(3);
			ctrlEdit *pass = GetCtrl<ctrlEdit>(5);

			Settings::inst().last_ip = host->GetText();

			if(atoi(port->GetText().c_str()) <= 0 || atoi(port->GetText().c_str()) >= 65535 || atoi(port->GetText().c_str()) == 3664)
			{
				SetText(_("Invalid port. The valid port-range is 1 to 65535!"), COLOR_RED, false);
				host->SetFocus(false);
				port->SetFocus(true);
				pass->SetFocus(false);
				break;
			}

			// Text auf "Verbinde mit Host..." setzen und Button deaktivieren
			SetText( _("Connecting with Host..."), COLOR_RED, false);

			GAMECLIENT.Stop();
			if(!GAMECLIENT.Connect(host->GetText(), pass->GetText(), server_type, (unsigned short)atoi(port->GetText().c_str()), false))
			{
				// Text auf "Verbindung fehlgeschlagen" setzen und Button aktivieren
				SetText( _("Connection failed!"), COLOR_RED, true);
			}

		} break;
	case 8:
		{
			Close();
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt den Text und Schriftfarbe vom Textfeld und den Status des
 *  Buttons.
 *
 *  @author FloSoft
 */
void iwDirectIPConnect::SetText(const std::string& text, unsigned int color, bool button)
{
	// Text setzen
	GetCtrl<ctrlText>(6)->SetColor(color);
	GetCtrl<ctrlText>(6)->SetText(text);

	// Button (de)aktivieren
	GetCtrl<ctrlButton>(7)->Enable(button);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt den Hostnamen im Editfeld.
 *
 *  @author FloSoft
 */
void iwDirectIPConnect::SetHost(const char *text)
{
	static char h[256];
	snprintf(h, 256, "%s", text);

	ctrlEdit *host = GetCtrl<ctrlEdit>(1);
	host->SetText(h);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt den Port im Editfeld.
 *
 *  @author FloSoft
 */
void iwDirectIPConnect::SetPort(unsigned short port)
{
	static char p[256];
	snprintf(p, 256, "%d", port);

	ctrlEdit *pp = GetCtrl<ctrlEdit>(3);
	pp->SetText(p);
}

void iwDirectIPConnect::CI_Error(const ClientError ce)
{
	switch(ce)
	{
	case CE_SERVERFULL:        SetText(_("This Server is full!"), COLOR_RED, true); break;
	case CE_WRONGPW:           SetText(_("Wrong Password!"), COLOR_RED, true); break;
	case CE_WRONGVERSION:      SetText(_("Wrong client version"), COLOR_RED, true); break;
	case CE_CONNECTIONLOST:    SetText(_("Connection to Host closed!"), COLOR_RED, true); break;
	case CE_INCOMPLETEMESSAGE: SetText(_("Too short Message received!"), COLOR_RED, true); break;
	case CE_INVALIDSERVERTYPE: SetText(_("Wrong Server Type!"), COLOR_RED, true); break;
	case CE_WRONGMAP:          SetText("", COLOR_RED, true); break;
	default:                   break;
	}
}

void iwDirectIPConnect::CI_NextConnectState(const ConnectState cs)
{
	switch(cs)
	{
	case CS_WAITFORANSWER:     SetText(_("Waiting for Reply..."), COLOR_YELLOW, true); break;
	case CS_QUERYPW:           SetText(_("Checking Password..."), COLOR_YELLOW, true); break;
	case CS_QUERYMAPNAME:      SetText(_("Checking Map..."), COLOR_YELLOW, true); break;
	case CS_QUERYPLAYERLIST:   SetText(_("Waiting for Playerinfo..."), COLOR_YELLOW, true); break;

	case CS_FINISHED: // Wir wurden verbunden
		{
			WindowManager::inst().Switch(new dskHostGame);
		} break;
	default: break;
	}
}
