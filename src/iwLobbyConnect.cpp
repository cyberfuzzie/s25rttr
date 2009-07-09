// $Id: iwLobbyConnect.cpp 5238 2009-07-09 20:50:28Z FloSoft $
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
#include "iwLobbyConnect.h"

#include "Loader.h"
#include "GameClient.h"
#include "controls.h"
#include "WindowManager.h"
#include "Settings.h"
#include "LobbyClient.h"

#include "dskLobby.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwLobbyConnect.
 *
 *  @author FloSoft
 */
iwLobbyConnect::iwLobbyConnect(void)
	: IngameWindow(CGI_LOBBYCONNECT, 0xFFFF, 0xFFFF, 500, 230, _("Connecting to Lobby"), LOADER.GetImageN("resource", 41))
{
	// Benutzername
	AddText(0,  20, 40, _("Username:"), COLOR_YELLOW, 0, NormalFont);
	ctrlEdit *user = AddEdit(1, 260, 40, 220, 22, TC_GREEN2, NormalFont, 15);
	user->SetFocus();
	user->SetText(SETTINGS.name);
	
	// Passwort
	AddText(2,  20, 70, _("Password:"), COLOR_YELLOW, 0, NormalFont);
	ctrlEdit *pass = AddEdit(3, 260, 70, 220, 22, TC_GREEN2, NormalFont, 0, true);
	pass->SetText(SETTINGS.password);
	
	// Emailadresse
	AddText(4,  20, 100, _("Email Address:"), COLOR_YELLOW, 0, NormalFont);
	ctrlEdit *email = AddEdit(5, 260, 100, 220, 22, TC_GREEN2, NormalFont);
	email->SetText(SETTINGS.email);
	
	// Passwort speichern ja/nein
	AddText(6, 20, 130, _("Save Password?"), COLOR_YELLOW, 0, NormalFont);

	ctrlOptionGroup *group = AddOptionGroup(10, ctrlOptionGroup::CHECK);
	group->AddTextButton(0, 260, 130, 105,	22, TC_GREEN2, _("No"), NormalFont); // nein
	group->AddTextButton(1, 375, 130, 105,	22, TC_GREEN2, _("Yes"), NormalFont); // ja
	group->SetSelection( (SETTINGS.save_password ? 1 : 0) );

	// Verbinden
	AddTextButton(7, 20, 190, 220,  22, TC_RED1, _("Connect"),NormalFont);

	// Registrieren
	AddTextButton(8, 260, 190, 220,  22, TC_GREEN2, _("Register"),NormalFont);
	
	// Status
	AddText(9, 250, 165, "", COLOR_RED, glArchivItem_Font::DF_CENTER, NormalFont);

	// Lobby-Interface setzen
	LOBBYCLIENT.SetInterface(this);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
iwLobbyConnect::~iwLobbyConnect()
{
	// Form abrufen und ggf in settings speichern
	std::string user, pass, email;
	LobbyForm(user, pass, email);

	if(!LOBBYCLIENT.LoggedIn())
	{
		LOBBYCLIENT.Stop();
		LOBBYCLIENT.SetInterface(NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  speichert die eingegebenen Benutzerdaten in die Settings
 *
 *  @author FloSoft
 */
void iwLobbyConnect::LobbyForm(std::string& user, std::string& pass, std::string& email)
{
	// Dann Form abrufen
	user = GetCtrl<ctrlEdit>(1)->GetText();
	pass =  GetCtrl<ctrlEdit>(3)->GetText();
	email =  GetCtrl<ctrlEdit>(5)->GetText();

	// Name speichern
	SETTINGS.name = user;

	// Ist Passwort speichern an?
	if(SETTINGS.save_password == true)
	{
		// ja, Passwort speichern
		SETTINGS.password = pass;

		// Email speichern
		SETTINGS.email = email;
	}
	else
	{
		SETTINGS.password = "";
		SETTINGS.email = "";
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
void iwLobbyConnect::Msg_EditChange(const unsigned int ctrl_id)
{
	// Statustext resetten
	SetText(0, COLOR_RED, true);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
void iwLobbyConnect::Msg_EditEnter(const unsigned int ctrl_id)
{
	ctrlEdit *user = GetCtrl<ctrlEdit>(1);
	ctrlEdit *pass = GetCtrl<ctrlEdit>(3);
	ctrlEdit *email = GetCtrl<ctrlEdit>(5);
	
	switch(ctrl_id)
	{
	case 1:
		{
			user->SetFocus(false);
			pass->SetFocus(true);
			email->SetFocus(false);
		} break;
	case 3:
		{
			user->SetFocus(false);
			pass->SetFocus(false);
			email->SetFocus(true);
		} break;
	case 5:
		{
			Msg_ButtonClick(7);
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
void iwLobbyConnect::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 7: // Verbinden
		{
			// Text auf "Verbinde mit Host..." setzen und Button deaktivieren
			SetText( _("Connecting with Host..."), COLOR_RED, false);

			// Form abrufen und ggf in settings speichern
			std::string user, pass, email;
			LobbyForm(user, pass, email);

			// Einloggen
			if(LOBBYCLIENT.Login(GetTxt(client_txt, 0), atoi(GetTxt(client_txt, 1)), user, pass) == false)
			{
				SetText(_("Connection failed!"), COLOR_RED, true);
				break;
			}
		} break;
	case 8: // Registrieren
		{
			// Text auf "Verbinde mit Host..." setzen und Button deaktivieren
			SetText( _("Connecting with Host..."), COLOR_RED, false);

			// Form abrufen und ggf in settings speichern
			std::string user, pass, email;
			LobbyForm(user, pass, email);

			if( user == "" || pass == "" || email == "")
		{
				// Einige Felder nicht ausgefüllt
				SetText(_("Please fill out all fields!"), COLOR_RED, true);
				break; // raus
			}

			if(LOBBYCLIENT.Register(GetTxt(client_txt, 0), atoi(GetTxt(client_txt, 1)), user, pass, email) == false)
			{
				SetText(_("Connection failed!"), COLOR_RED, true);
				break;
			}
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
void iwLobbyConnect::Msg_OptionGroupChange(const unsigned int ctrl_id, const unsigned short selection)
{
	switch(ctrl_id)
	{
	case 10: // Passwort speichern Ja/Nein
		{
			SETTINGS.save_password = (selection == 1);
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
void iwLobbyConnect::SetText(std::string text, unsigned int color, bool button)
{
	ctrlText *t = GetCtrl<ctrlText>(9);
	ctrlButton *b = GetCtrl<ctrlButton>(7);
	ctrlButton *b2 = GetCtrl<ctrlButton>(8);

	// Text setzen
	t->SetColor(color);
	t->SetText(text);

	// Button (de)aktivieren
	b->Enable(button);
	b2->Enable(button);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Wir wurden eingeloggt
 *
 *  @author FloSoft
 */
void iwLobbyConnect::LC_LoggedIn(const std::string &email)
{
	// geänderte Daten speichern (also die erhaltene Emailadresse)
	std::string user, pass, email2 = email;

	GetCtrl<ctrlEdit>(5)->SetText(email);
	LobbyForm(user, pass, email2);

	GetCtrl<ctrlButton>(8)->Enable(false);

	WindowManager::inst().Switch(new dskLobby);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Wir wurden registriert.
 *
 *  @author FloSoft
 */
void iwLobbyConnect::LC_Registered()
{
	// Registrierung erfolgreich
	SetText(_("Registration successful!"), COLOR_YELLOW, true);

	GetCtrl<ctrlButton>(8)->Enable(false);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Status: Warten auf Antwort.
 *
 *  @author FloSoft
 */
void iwLobbyConnect::LC_Status_Waiting()
{
	SetText(_("Waiting for Reply..."), COLOR_YELLOW, false);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Status: Benutzerdefinierter Fehler (inkl Conn-Reset u.ä)
 *
 *  @author FloSoft
 */
void iwLobbyConnect::LC_Status_Error(const std::string &error)
{
	SetText(error, COLOR_RED, true);
}
