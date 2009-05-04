// $Id: iwOptionsWindow.cpp 4809 2009-05-04 20:10:11Z OLiver $
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
#include "iwOptionsWindow.h"

#include "WindowManager.h"
#include "Loader.h"
#include "Settings.h"
#include "files.h"

#include "controls.h"

#include "iwEndgame.h"
#include "iwSurrender.h"
#include "iwTextfile.h"
#include "iwSave.h"

#include "SoundManager.h"
#include "AudioDriverWrapper.h"
#include "MusicPlayer.h"
#include "iwMusicPlayer.h"


///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwOptionsWindow.
 *
 *  @author OLiver
 */
iwOptionsWindow::iwOptionsWindow()
	: IngameWindow(CGI_OPTIONSWINDOW, 0xFFFF, 0xFFFF, 300, 505, _("Game menu"), GetImage(resource_dat, 41))
{
	

	// Der Soldat oben
	AddImage(1, 150, 36, GetImage(io_dat, 30));

	// Versionszeile
	AddVarText(2, 150, 76, _("The Settlers II.5 RTTR, v%s-%s"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_BOTTOM, NormalFont, 2, GetWindowVersion(), GetWindowRevision());
	// Copyright
	AddText(3, 150, 96, "° 2005 - 2009 Settlers Freaks", COLOR_YELLOW, glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_BOTTOM, NormalFont);

	// "Tastaturbelegung"
	AddImageButton(4, 35, 120, 35, 35,TC_GREEN2, GetImage(io_dat, 79));
	AddText(5, 85, 140, _("Keyboard layout"), COLOR_YELLOW, 0 | glArchivItem_Font::DF_BOTTOM, NormalFont);

	// "'Lies mich'-Datei laden"
	AddImageButton(6, 35, 160, 35, 35, TC_GREEN2, GetImage(io_dat, 79));
	AddText(7, 85, 180, _("Load 'ReadMe' file"), COLOR_YELLOW, 0 | glArchivItem_Font::DF_BOTTOM, NormalFont);

	// "Spiel laden!"
	AddImageButton( 8, 35, 210, 35, 35, TC_GREEN2, GetImage(io_dat, 48));
	AddText(9, 85, 230, _("Load game!"), COLOR_YELLOW, 0 | glArchivItem_Font::DF_BOTTOM, NormalFont);

	// "Spiel speichern!"
	AddImageButton(10, 35, 250, 35, 35, TC_GREEN2, GetImage(io_dat, 47));
	AddText(11, 85, 270, _("Save game!"), COLOR_YELLOW, 0 | glArchivItem_Font::DF_BOTTOM, NormalFont);

	// Geräusche an/aus
	AddImageButton(12, 35, 300, 35, 35, TC_GREEN2, GetImage(io_dat, 114+!SETTINGS.effekte));

	// Musik an/aus
	AddImageButton(13, 35, 340, 35, 35, TC_GREEN2, GetImage(io_dat, 116+!SETTINGS.musik));

	// Geräuschlautstärke
	AddProgress(14, 100, 306, 160, 22, TC_GREEN2, 139, 138, 10)
		->SetPosition(SETTINGS.effekte_volume*10/255);

	// Musiklautstärke
	AddProgress(15, 100, 346, 160, 22, TC_GREEN2, 139, 138, 10)
		->SetPosition(SETTINGS.musik_volume*10/255);

	//// Music Player
	AddTextButton(16,100,380,160,22,TC_GREEN2,_("Music player"),NormalFont);

	// "Spiel aufgeben"
	AddTextButton(17, 67, 425, 168, 24, TC_RED1, _("Surrender"),NormalFont);
	// "Spiel beenden"
	AddTextButton(0, 67, 460, 168, 24, TC_RED1, _("End game"),NormalFont);
	
}


void iwOptionsWindow::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 0: // "Spiel beenden"
		{
			WindowManager::inst().Show(new iwEndgame);
			Close();
		} break;
	case 4: // "Tastaturbelegung laden"
		{
			WindowManager::inst().Show(new iwTextfile("keyboardlayout.txt",_("Keyboard layout")));
		} break;
	case 6: // "'Lies mich'-Datei laden"
		{
			WindowManager::inst().Show(new iwTextfile("readme.txt",_("Readme!")));
		} break;
	case 10: // "Spiel speichern"
		{
			WindowManager::inst().Show(new iwSave);
		} break;

	case 12: // Geräusche an/aus
		{
			SETTINGS.effekte = !SETTINGS.effekte;
			GetCtrl<ctrlImageButton>(12)->SetImage(GetImage(io_dat, 114+!SETTINGS.effekte));

			if(!SETTINGS.effekte)
				SoundManager::inst().StopAll();
		} break;

	case 13: // Musik an/aus
		{
			SETTINGS.musik = !SETTINGS.musik;
			GetCtrl<ctrlImageButton>(13)->SetImage(GetImage(io_dat, 116+!SETTINGS.musik));
			if(SETTINGS.musik)
				MusicPlayer::inst().StartPlaying();
			else
				MusicPlayer::inst().Stop();
		} break;
	case 16: // Music player
		{
			WindowManager::inst().Show(new iwMusicPlayer);
		} break;
	case 17: // Aufgeben
		{
			WindowManager::inst().Show(new iwSurrender);
			Close();
		} break;
	}
}

void iwOptionsWindow::Msg_ProgressChange(const unsigned int ctrl_id, const unsigned short position)
{
	switch(ctrl_id)
	{
	case 14:
		{
			SETTINGS.effekte_volume = (unsigned char)position*255/10 + (position<10 ? 1 : 0);
			AudioDriverWrapper::inst().SetMasterEffectVolume(SETTINGS.effekte_volume);
		} break;
	case 15:
		{
			SETTINGS.musik_volume = (unsigned char)position*255/10 + (position<10 ? 1 : 0);
			AudioDriverWrapper::inst().SetMasterMusicVolume(SETTINGS.musik_volume);
		} break;
	}
}
