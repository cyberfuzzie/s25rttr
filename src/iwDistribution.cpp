// $Id: iwDistribution.cpp 5153 2009-07-01 13:31:56Z Demophobie $
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
#include "iwDistribution.h"

#include "Loader.h"
#include "controls.h"

#include "GameClient.h"
#include "GameCommands.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/// Anzahl der einzelnen Einstellungen in den Gruppen
// Nahrungsgruppe
const unsigned GROUP_SIZES[6] =
{
	4, 4, 2, 3, 3, 4
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwDistribution.
 *
 *  @author OLiver
 */
iwDistribution::iwDistribution(void)
	: IngameWindow(CGI_DISTRIBUTION, 0xFFFF, 0xFFFF, 260, 272, _("Distribution of goods"), GetImage(resource_dat, 41)),
	settings_changed(false)
{
	ctrlGroup *group;
	ctrlProgress *progress;

	// Werte für die Progressbars
	//const GameClientPlayer *player = GAMECLIENT.GetLocalPlayer();

	// Tab Control
	ctrlTab *tab = AddTabCtrl(0, 10, 20, 240);
	
	// Nahrungsgruppe
	group = tab->AddTab(GetImage(io_dat, 80), _("Foodstuff"), TAB_FOOD);
	// Granitbergwerk
	group->AddText(11, 120,  60, _("Granite mine"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(12, 20,  60, 200, 20, TC_GREY, 139, 138, 10);
	// Kohlebergwerk
	group->AddText(13, 120, 100, _("Coal mine"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(14, 20, 100, 200, 20, TC_GREY, 139, 138, 10);
	// Eisenbergwerk	
	group->AddText(15, 120, 140, _("Iron mine"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(16, 20, 140, 200, 20, TC_GREY, 139, 138, 10);
	// Goldbergwerk	
	group->AddText(17, 120, 180, _("Gold mine"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(18, 20, 180, 200, 20, TC_GREY, 139, 138, 10);

	// Getreidegruppe
	group = tab->AddTab(GetImage(io_dat, 90), _("Grain"), TAB_CORN);

	// Mühle
	group->AddText(21, 120,  60, _("Mill"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(22, 20,  60, 200, 20, TC_GREY, 139, 138, 10);
	// Schweinezucht
	group->AddText(23, 120, 100, _("Pig farm"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(24, 20, 100, 200, 20, TC_GREY, 139, 138, 10);
	// Eselzucht
	group->AddText(25, 120, 140, _("Donkey breeding"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(26, 20, 140, 200, 20, TC_GREY, 139, 138, 10);
	// Brauerei
	group->AddText(27, 120, 180, _("Brewery"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(28, 20, 180, 200, 20, TC_GREY, 139, 138, 10);

	// Eisengruppe
	group = tab->AddTab(GetImage(io_dat, 81), _("Iron"), TAB_IRON);

	// Schmiede
	group->AddText(31, 120,  60, _("Armory"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(32, 20,  60, 200, 20, TC_GREY, 139, 138, 10);
	// Schlosserei
	group->AddText(33, 120, 100, _("Metalworks"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(34, 20, 100, 200, 20, TC_GREY, 139, 138, 10);

	// Kohlegruppe
	group = tab->AddTab(GetImage(io_dat, 91), _("Coal"), TAB_COAL);

	// Schmiede
	group->AddText(41, 120,  60, _("Armory"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(42, 20,  60, 200, 20, TC_GREY, 139, 138, 10);
	// Eisenschmelze
	group->AddText(43, 120, 100, _("Iron smelter"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(44, 20, 100, 200, 20, TC_GREY, 139, 138, 10);
	// Münzprägerei
	group->AddText(45, 120, 140, _("Mint"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(46, 20, 140, 200, 20, TC_GREY, 139, 138, 10);

	// Brettergruppe
	group = tab->AddTab(GetImage(io_dat, 82), _("Boards"), TAB_BOARD);

	// Baustellen
	group->AddText(51, 120,  60, _("Construction"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(52, 20,  60, 200, 20, TC_GREY, 139, 138, 10);
	// Schlosserei
	group->AddText(53, 120, 100, _("Metalworker"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(54, 20, 100, 200, 20, TC_GREY, 139, 138, 10);
	// Werft
	group->AddText(55, 120, 140, _("Shipyard"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(56, 20, 140, 200, 20, TC_GREY, 139, 138, 10);
	// Wasserbüffel äh -gruppe ;-)
	group = tab->AddTab(GetImage(io_dat, 92), _("Water"), TAB_WATER);

	// Bäckerei
	group->AddText(61, 120,  60, _("Bakery"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(62, 20,  60, 200, 20, TC_GREY, 139, 138, 10);
	// Brauerei
	group->AddText(63, 120, 100, _("Brewery"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(64, 20, 100, 200, 20, TC_GREY, 139, 138, 10);
	// Schweinezucht
	group->AddText(65, 120, 140, _("Pig farm"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(66, 20, 140, 200, 20, TC_GREY, 139, 138, 10);
	// Eselzucht
	group->AddText(67, 120, 180, _("Donkey breeding"), COLOR_YELLOW, glArchivItem_Font::DF_CENTER|glArchivItem_Font::DF_BOTTOM, SmallFont);
	progress = group->AddProgress(68, 20, 180, 200, 20, TC_GREY, 139, 138, 10);

	UpdateSettings();

	// Gruppe auswählen
	tab->SetSelection(0);

	// Timer für die Übertragung der Daten via Netzwerk
	AddTimer(1,2000);


	// Hilfe
	AddImageButton(2, 15, height-15-32, 32, 32,TC_GREY, GetImage(io_dat, 21), _("Help"));
	// Standardbelegung
	AddImageButton(10, width-15-32, height-15-32, 32, 32, TC_GREY, GetImage(io_dat, 191), _("Default"));

}

iwDistribution::~iwDistribution()
{
	TransmitSettings();
}

void iwDistribution::TransmitSettings()
{
	if(settings_changed)
	{
		// Werte aus den Progress-Controls auslesen
		unsigned char groups[] = { 4, 4, 2, 3, 3, 4 };

		for(unsigned char i = 1, j = 0; i <= 6; ++i)
		{
			// Werte der Gruppen auslesen
			for(unsigned char k = 0; k < groups[i-1]; ++k)
				GAMECLIENT.visual_settings.distribution[j+k]
			= (unsigned char)GetCtrl<ctrlTab>(0)->GetGroup(i)->GetCtrl<ctrlProgress>(i*10+k*2+2)->
				GetPosition();
			j += groups[i-1];
		}

		// und übermitteln
		GAMECLIENT.AddGC(new gc::ChangeDistribution(GAMECLIENT.visual_settings.distribution));

		settings_changed = false;
	}
}

void iwDistribution::Msg_Group_ProgressChange(const unsigned int group_id, const unsigned int ctrl_id, const unsigned short position)
{
	settings_changed = true;
}

void iwDistribution::Msg_Timer(const unsigned int ctrl_id)
{
	if(GAMECLIENT.IsReplayModeOn())
		// Im Replay aktualisieren wir die Werte 
		UpdateSettings();
	else
		// Im normalen Spielmodus schicken wir den ganzen Spaß ab
		TransmitSettings();
}

void iwDistribution::UpdateSettings()
{
	// Globale Id für alle Gruppen für die visual_settings
	unsigned vsi = 0;
	// Alle Gruppen durchgehen und Einstellungen festlegen
	for(unsigned g = 0;g<6;++g)
	{
		ctrlGroup * group = GetCtrl<ctrlTab>(0)->GetGroup(TAB_FOOD+g);
		for(unsigned i = 0;i<GROUP_SIZES[g];++i,++vsi)
			group->GetCtrl<ctrlProgress>(10*(g+1)+2*(i+1))->SetPosition(GAMECLIENT.visual_settings.distribution[vsi]);
	}
}
