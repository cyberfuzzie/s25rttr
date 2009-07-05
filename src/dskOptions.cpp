// $Id: dskOptions.cpp 5195 2009-07-05 09:44:30Z FloSoft $
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
#include "dskOptions.h"

#include "WindowManager.h"
#include "Loader.h"

#include "Settings.h"
#include "controls.h"
#include "GlobalVars.h"

#include "dskMainMenu.h"
#include "iwMusicPlayer.h"

#include "languages.h"

#include "VideoDriverWrapper.h"
#include "AudioDriverWrapper.h"
#include "MusicPlayer.h"

#include "iwTextfile.h"
#include "iwMsgbox.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class dskOptions
 *
 *  Klasse des Optionen Desktops.
 *
 *  @author OLiver
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p dskOptions.
 *
 *  @author OLiver
 *  @author FloSoft
 */
dskOptions::dskOptions(void) : Desktop(GetImage(backgrounds, 1))
{
	// Zurück
	AddTextButton(0, 300, 550, 200, 22,   TC_RED1, _("Back"),NormalFont);
	
	// "Optionen"
	AddText(1, 400, 10, _("Options"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, LargeFont);

	ctrlOptionGroup *optiongroup = AddOptionGroup(10, ctrlOptionGroup::CHECK, scale);

	AddTextButton(14, 520, 550, 200, 22, TC_GREEN2,     _("Enhancements"),NormalFont);

	// "Allgemein"
	optiongroup->AddTextButton(11,  80, 510, 200, 22, TC_GREEN2,     _("Common"),NormalFont);
	// "Grafik"
	optiongroup->AddTextButton(12, 300, 510, 200, 22, TC_GREEN2,     _("Graphics"),NormalFont);
	// "Sound"
	optiongroup->AddTextButton(13, 520, 510, 200, 22, TC_GREEN2,     _("Sound/Music"),NormalFont);

	ctrlGroup *groupAllgemein = AddGroup(21, scale);
	ctrlGroup *groupGrafik = AddGroup(22, scale);
	ctrlGroup *groupSound = AddGroup(23, scale);
	ctrlComboBox *combo;

	// "Name"
	groupAllgemein->AddText(30, 80, 80, _("Name in Game:"), COLOR_YELLOW, 0, NormalFont);
	groupAllgemein->AddEdit(31, 280, 75, 190, 22, TC_GREY, NormalFont, 15);

	// "Sprache"
	groupAllgemein->AddText(32, 80, 130, _("Language:"), COLOR_YELLOW, 0, NormalFont);

	combo = groupAllgemein->AddComboBox(33, 280, 125, 190, 20, TC_GREY, NormalFont, 100);

	for(unsigned i = 0 ; i < LANGUAGES.getCount(); ++i)
	{
		const Languages::Language l = LANGUAGES.getLanguage(i);

		combo->AddString(_(l.name));
		if(SETTINGS.language == l.code )
			combo->SetSelection(static_cast<unsigned short>(i));
	}

	groupAllgemein->AddText(34, 80, 180, _("Keyboard layout:"), COLOR_YELLOW, 0, NormalFont);
	groupAllgemein->AddTextButton(35, 280, 175, 120, 22, TC_GREY, _("Readme"), NormalFont);

	// "Auflösung"
	groupGrafik->AddText(  40,  80, 80, _("Resolution:"),COLOR_YELLOW,0,NormalFont);
	groupGrafik->AddComboBox(41, 280, 75, 120, 22, TC_GREY, NormalFont, 150);

	// "Vollbild"
	groupGrafik->AddText(  46,  80, 130, _("Mode:"), COLOR_YELLOW, 0, NormalFont);
	optiongroup = groupGrafik->AddOptionGroup(47, ctrlOptionGroup::CHECK, scale);
	optiongroup->AddTextButton(48, 480, 125, 190, 22, TC_GREY, _("Fullscreen"),NormalFont);
	optiongroup->AddTextButton(49, 280, 125, 190, 22, TC_GREY, _("Windowed"),NormalFont);

	// "VSync"
	groupGrafik->AddText(  50,  80, 180, _("VSync:"), COLOR_YELLOW, 0, NormalFont);
	optiongroup = groupGrafik->AddOptionGroup(51, ctrlOptionGroup::CHECK, scale);

	if(GLOBALVARS.ext_swapcontrol == false) // VSync unterstützt?
		optiongroup->AddText(  52, 280, 180, _("not supported"), COLOR_YELLOW, 0, NormalFont);
	else
		optiongroup->AddTextButton(52, 280, 175, 190, 22, TC_GREY,_("On"),NormalFont);
	optiongroup->AddTextButton(53, 480, 175, 190, 22, TC_GREY, _("Off"),NormalFont);

	// "VBO"
	groupGrafik->AddText(  54,  80, 230, _("Vertex Buffer Objects:"), COLOR_YELLOW, 0, NormalFont);
	optiongroup = groupGrafik->AddOptionGroup(55, ctrlOptionGroup::CHECK, scale);

	if(GLOBALVARS.ext_vbo == false) // VBO unterstützt?
		optiongroup->AddText(  56, 280, 230, _("not supported"), COLOR_YELLOW, 0, NormalFont);
	else
		optiongroup->AddTextButton(56, 280, 225, 190, 22, TC_GREY,_("On"),NormalFont);
	optiongroup->AddTextButton(57, 480, 225, 190, 22, TC_GREY, _("Off"),NormalFont);

	// "Grafiktreiber"
	groupGrafik->AddText(58, 80, 275, _("Graphics Driver"), COLOR_YELLOW, 0, NormalFont);
	combo = groupGrafik->AddComboBox(59, 280, 275, 390, 20, TC_GREY, NormalFont, 100);

	list<DriverWrapper::DriverItem> video_drivers;
	DriverWrapper::LoadDriverList(DriverWrapper::DT_VIDEO, video_drivers);

	for(list<DriverWrapper::DriverItem>::iterator it = video_drivers.begin(); it.valid(); ++it)
	{
		combo->AddString(it->GetName());
		if(it->GetName() == SETTINGS.video_driver)
			combo->SetSelection(combo->GetCount()-1);
	}

	// "Audiotreiber"
	groupSound->AddText(60,  80, 230, _("Sounddriver"), COLOR_YELLOW, 0, NormalFont);
	combo = groupSound->AddComboBox(61, 280, 225, 390, 20, TC_GREY, NormalFont, 100);

	list<DriverWrapper::DriverItem> audio_drivers;
	DriverWrapper::LoadDriverList(DriverWrapper::DT_AUDIO, audio_drivers);

	for(list<DriverWrapper::DriverItem>::iterator it = audio_drivers.begin(); it.valid(); ++it)
	{
		combo->AddString(it->GetName());
		if(it->GetName() == SETTINGS.audio_driver)
			combo->SetSelection(combo->GetCount()-1);
	}

	// Musik
	groupSound->AddText(  62,  80, 80, _("Music"), COLOR_YELLOW, 0, NormalFont);
	optiongroup = groupSound->AddOptionGroup(63, ctrlOptionGroup::CHECK, scale);
	optiongroup->AddTextButton(64, 280, 75, 90, 22, TC_GREY, _("On"),NormalFont);
	optiongroup->AddTextButton(65, 380, 75, 90, 22, TC_GREY, _("Off"),NormalFont);

	ctrlProgress *Mvolume = groupSound->AddProgress(72, 480, 75, 190, 22, TC_GREY, 139, 138, 10);
	Mvolume->SetPosition(SETTINGS.musik_volume*10/255);
	
	// Effekte
	groupSound->AddText(  66,  80, 130, _("Effects"), COLOR_YELLOW, 0, NormalFont);
	optiongroup = groupSound->AddOptionGroup(67, ctrlOptionGroup::CHECK, scale);
	optiongroup->AddTextButton(68, 280, 125, 90, 22, TC_GREY, _("On"),NormalFont);
	optiongroup->AddTextButton(69, 380, 125, 90, 22, TC_GREY, _("Off"),NormalFont);

	ctrlProgress *FXvolume = groupSound->AddProgress(70, 480, 125, 190, 22, TC_GREY, 139, 138, 10);
	FXvolume->SetPosition(SETTINGS.effekte_volume*10/255);

	// Musicplayer-Button
	groupSound->AddTextButton(71,280,175,190,22,TC_GREY,_("Music player"),NormalFont);
	

	ctrlGroup *group;

	// "Allgemein" auswählen
	optiongroup = GetCtrl<ctrlOptionGroup>(10);
	optiongroup->SetSelection(11, true);

	// Gruppe "Allgemein" holen
	group = GetCtrl<ctrlGroup>(21);

	// Name setzen
	group->GetCtrl<ctrlEdit>(31)->SetText(SETTINGS.name);

	// Gruppe "Grafik" holen
	group = GetCtrl<ctrlGroup>(22);

	// "Vollbild" setzen
	optiongroup = group->GetCtrl<ctrlOptionGroup>(47);
	optiongroup->SetSelection( (SETTINGS.fullscreen ? 48 : 49) );

	// "VSync" setzen
	optiongroup = group->GetCtrl<ctrlOptionGroup>(51);

	if(GLOBALVARS.ext_swapcontrol)
		optiongroup->SetSelection( (SETTINGS.vsync ? 52 : 53) );
	else
		optiongroup->SetSelection(53);

	// "VBO" setzen
	optiongroup = group->GetCtrl<ctrlOptionGroup>(55);

	if(GLOBALVARS.ext_vbo)
		optiongroup->SetSelection( (SETTINGS.vbo ? 56 : 57) );
	else
		optiongroup->SetSelection(57);

	// Gruppe "Sound" holen
	group = GetCtrl<ctrlGroup>(23);

	// "Musik" setzen
	optiongroup = group->GetCtrl<ctrlOptionGroup>(63);
	optiongroup->SetSelection( (SETTINGS.musik ? 64 : 65) );

	// "Effekte" setzen
	optiongroup = group->GetCtrl<ctrlOptionGroup>(67);
	optiongroup->SetSelection( (SETTINGS.effekte ? 68 : 69) );

	// Videomodi auflisten
	VideoDriverWrapper::inst().ListVideoModes(video_modes);

	// Und zu der Combobox hinzufügen
	for(unsigned i = 0;i<video_modes.size();++i)
	{
		// >=800x600, alles andere macht keinen Sinn
		if(video_modes[i].width >= 800 && video_modes[i].height >= 600)
		{
			char str[64];
			sprintf(str,"%ux%u",video_modes[i].width,video_modes[i].height);

			groupGrafik->GetCtrl<ctrlComboBox>(41)->AddString(str);

			// Ist das die aktuelle Auflösung? Dann selektieren
			if(video_modes[i].width == SETTINGS.width && 
				video_modes[i].height == SETTINGS.height)
				groupGrafik->GetCtrl<ctrlComboBox>(41)->SetSelection(i);
		}
		else
		{
			video_modes.erase(video_modes.begin()+i);
			--i;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
dskOptions::~dskOptions()
{
	// Name abspeichern
	SETTINGS.name = GetCtrl<ctrlGroup>(21)->GetCtrl<ctrlEdit>(31)->GetText();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void dskOptions::Msg_Group_ProgressChange(const unsigned int group_id, const unsigned int ctrl_id, const unsigned short position)
{
	switch(ctrl_id)
	{
	case 70:
		{
			SETTINGS.effekte_volume = (unsigned char)position*255/10 + (position<10 ? 1 : 0);
			AudioDriverWrapper::inst().SetMasterEffectVolume(SETTINGS.effekte_volume);
		} break;
	case 72:
		{
			SETTINGS.musik_volume = (unsigned char)position*255/10 + (position<10 ? 1 : 0);
			AudioDriverWrapper::inst().SetMasterMusicVolume(SETTINGS.musik_volume);
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void dskOptions::Msg_Group_ComboSelectItem(const unsigned int group_id, const unsigned int ctrl_id, const unsigned short selection)
{
	ctrlGroup *group = GetCtrl<ctrlGroup>(group_id);
	ctrlComboBox *combo = group->GetCtrl<ctrlComboBox>(ctrl_id);

	switch(ctrl_id)
	{
	case 33: // Sprache
		{
			SETTINGS.language = LANGUAGES.setLanguage(selection);
			WindowManager::inst().Switch(new dskOptions);
		} break;
	case 41: // Auflösung
		{
			SETTINGS.width = video_modes[selection].width;
			SETTINGS.height = video_modes[selection].height;
		} break;
	case 59: // Videotreiber
		{
			SETTINGS.video_driver = combo->GetText(selection);
		} break;
	case 61: // Audiotreiber
		{
			SETTINGS.audio_driver =  combo->GetText(selection);
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void dskOptions::Msg_Group_OptionGroupChange(const unsigned int group_id, const unsigned int ctrl_id, const unsigned short selection)
{
	switch(ctrl_id)
	{
	case 47: // Vollbild
		{
			switch(selection)
			{
			case 48: SETTINGS.fullscreen = true;  break;
			case 49: SETTINGS.fullscreen = false; break;
			}
		} break;
	case 51: // VSync
		{
			switch(selection)
			{
			case 52: SETTINGS.vsync = true;  break;
			case 53: SETTINGS.vsync = false; break;
			}
		} break;
	case 55: // VBO
		{
			switch(selection)
			{
			case 56: SETTINGS.vbo = true;  break;
			case 57: SETTINGS.vbo = false; break;
			}
		} break;
	case 63: // Musik
		{
			switch(selection)
			{
			case 64: SETTINGS.musik = true;  break;
			case 65: SETTINGS.musik = false; break;
			}
			if(SETTINGS.musik)
				MusicPlayer::inst().StartPlaying();
			else
				MusicPlayer::inst().Stop();
		} break;
	case 67: // Soundeffekte
		{
			switch(selection)
			{
			case 68: SETTINGS.effekte = true;  break;
			case 69: SETTINGS.effekte = false; break;
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
void dskOptions::Msg_OptionGroupChange(const unsigned int ctrl_id, const unsigned short selection)
{
	switch(ctrl_id)
	{
	case 10: // Optionengruppen anzeigen
		{
			for(unsigned short i = 21; i < 24; ++i)
				GetCtrl<ctrlGroup>(i)->SetVisible( (i == selection + 10 ? true : false) );
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void dskOptions::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 0: // "Zurück"
		{
			SETTINGS.Save();
			
			// Auflösung/Vollbildmodus geändert?
			if(SETTINGS.width != VideoDriverWrapper::inst().GetScreenWidth() ||
				SETTINGS.height != VideoDriverWrapper::inst().GetScreenHeight() ||
				SETTINGS.fullscreen != VideoDriverWrapper::inst().IsFullscreen())
			{
				if(!VideoDriverWrapper::inst().ResizeScreen(SETTINGS.width,SETTINGS.height,SETTINGS.fullscreen))
				{
					WindowManager::inst().Show(new iwMsgbox(_("Sorry!"), _("You need to restart your game to change the screen resolution!"), this, MSB_OK, MSB_EXCLAMATIONGREEN, 1));
					break;
				}
			}

			if(SETTINGS.video_driver != VideoDriverWrapper::inst().GetName() || 
				SETTINGS.audio_driver != AudioDriverWrapper::inst().GetName())
			{
				WindowManager::inst().Show(new iwMsgbox(_("Sorry!"), _("You need to restart your game to change the video or audio driver!"), this, MSB_OK, MSB_EXCLAMATIONGREEN, 1));
				break;
			}

			WindowManager::inst().Switch(new dskMainMenu);
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void dskOptions::Msg_Group_ButtonClick(const unsigned int group_id, const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	default:
		break;
	case 71: // "Music player"
		{
			WindowManager::inst().Show(new iwMusicPlayer);
		} break;
	case 35: // "Keyboard Readme"
		{
			WindowManager::inst().Show(new iwTextfile("keyboardlayout.txt", _("Keyboard layout") ) );
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void dskOptions::Msg_MsgBoxResult(const unsigned int msgbox_id, const MsgboxResult mbr)
{
	switch(msgbox_id)
	{
	default:
		break;
	case 1: // "You need to restart your game ..."
		{
			WindowManager::inst().Switch(new dskMainMenu);
		} break;
	}
}
