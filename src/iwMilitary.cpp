// $Id: iwMilitary.cpp 4933 2009-05-24 12:29:23Z OLiver $
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
#include "iwMilitary.h"

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

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwMilitary.
 *
 *  @author OLiver
 */
iwMilitary::iwMilitary(void)
	: IngameWindow(CGI_MILITARY, 0xFFFE, 0xFFFE, 168, 298, _("Military"), GetImage(io_dat, 5)),
	settings_changed(false)
{
	// Einzelne Balken
	AddProgress(0,17,25,132,26,TC_GREY,119,120,5,"",4,4,0,_("Fewer recruits"),_("More recruits")); /* pitch: 4, 4 */
	AddProgress(1,17,57,132,26,TC_GREY,121,122,5,"",4,4,0,_("Weak defense"),_("Strong defense"));
	AddProgress(2,17,89,132,26,TC_GREY,123,124,5,"",4,4,0,_("Fewer defenders"),_("More defenders"));
	AddProgress(3,17,121,132,26,TC_GREY,209,210,5,"",4,4,0,_("Less attackers"),_("More attackers"));
	AddProgress(4,17,153,132,26,TC_GREY,129,130,5,"",4,4,0,_("Interior"),_("Interior"));
	AddProgress(5,17,185,132,26,TC_GREY,127,128,5,"",4,4,0,_("Center of country"),_("Center of country"));
	AddProgress(6,17,217,132,26,TC_GREY,125,126,5,"",4,4,0,_("Border areas"),_("Border areas"));

	// unteren 2 Buttons
	AddImageButton(7,18,250,30,32,TC_GREY, GetImage(io_dat,21), _("Help"));
	AddImageButton(8,120,250,30,32,TC_GREY, GetImage(io_dat,191),_("Default"));

	// Absendetimer, in 2s-Abschnitten wird jeweils das ganze als Netzwerknachricht ggf. abgeschickt
	AddTimer(9,2000);
	UpdateSettings();
}

iwMilitary::~iwMilitary()
{
	TransmitSettings();
}

/// Sendet ver‰nderte Einstellungen (an den Client), falls sie ver‰ndert wurden
void iwMilitary::TransmitSettings()
{
	// Wurden Einstellungen ge‰ndert?
	if(settings_changed)
	{
		// Einstellungen speichern
		for(unsigned char i = 0; i < 7; ++i)
			GAMECLIENT.visual_settings.military_settings[i] = (unsigned char)GetCtrl<ctrlProgress>(i)->GetPosition();

		GAMECLIENT.AddGC(new gc::ChangeMilitary(GAMECLIENT.visual_settings.military_settings));
		settings_changed = false;
	}
}


void iwMilitary::Msg_Timer(const unsigned int ctrl_id)
{
	if(GAMECLIENT.IsReplayModeOn())
		// Im Replay aktualisieren wir die Werte 
		UpdateSettings();
	else
		// Im normalen Spielmodus schicken wir den ganzen Spaﬂ ab
		TransmitSettings();
} 

void iwMilitary::Msg_ProgressChange(const unsigned int ctrl_id, const unsigned short position)
{

	// Einstellungen wurden ge‰ndert
	settings_changed = true;
}

void iwMilitary::UpdateSettings()
{
	// Einstellungen festlegen
	for(unsigned i = 0;i<7;++i)
		GetCtrl<ctrlProgress>(i)->SetPosition(GAMECLIENT.visual_settings.military_settings[i]);
}

