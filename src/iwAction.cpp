// $Id: iwAction.cpp 5787 2009-12-10 22:20:45Z OLiver $
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
#include "iwAction.h"

#include "dskGameInterface.h"
#include "iwDemolishBuilding.h"
#include "iwMilitaryBuilding.h"

#include "Loader.h"
#include "controls.h"
#include "Settings.h"
#include "VideoDriverWrapper.h"
#include "GameClient.h"
#include "GameWorld.h"
#include "WindowManager.h"
#include "GameCommands.h"

#include "noFlag.h"
#include "nobMilitary.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


// Tab - Flags
enum TabID
{
	 TAB_BUILD = 1,
	 TAB_SETFLAG ,
	 TAB_WATCH ,
	 TAB_FLAG,
	 TAB_CUTROAD,
	 TAB_ATTACK,
	 TAB_SEAATTACK
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwAction.
 *
 *  @author OLiver
 */
iwAction::iwAction(dskGameInterface *const gi, GameWorldViewer * const gwv, const Tabs& tabs, unsigned short selected_x, unsigned short selected_y, int mouse_x, int mouse_y, unsigned int params, bool military_buildings)
	: IngameWindow(CGI_ACTION, mouse_x, mouse_y, 200, 254, _("Activity window"), LOADER.GetImageN("io", 1)),
	gi(gi), gwv(gwv), selected_x(selected_x), selected_y(selected_y), last_x(mouse_x), last_y(mouse_y)
{
	/*
		TAB_FLAG	1 = Straßenbau
		TAB_FLAG	2 = Wasserbau
		TAB_FLAG	3 = Flagge abreißen
		TAB_FLAG	4 = Geologe rufen
		TAB_FLAG	5 = Späher rufen

		TAB_CUTROAD	1 = Straße abreißen

		TAB_BUILD   100-108, 200-212, 300-303, 400-403 = Gebäude

		TAB_SETFLAG 1 = Flagge setzen

		TAB_WATCH   1 =
		TAB_WATCH   2 =
		TAB_WATCH   3 = zum HQ

		TAB_ATTACK  1 = Weniger Soldaten
		TAB_ATTACK  2 = Mehr Soldaten
		TAB_ATTACK  3 = Optionengruppe Stark/Schwach
		TAB_ATTACK  4 = Angriff
		TAB_ATTACK  10-14 = Direktauswahl Anzahl
	*/

	/// Haupttab
	ctrlTab *main_tab = AddTabCtrl(0, 10, 20, 180);

	// Bau-main_tab
	if(tabs.build)
	{
		ctrlGroup *group = 	main_tab->AddTab(LOADER.GetImageN("io", 18), _("-> Build house"), TAB_BUILD);

		ctrlTab *build_tab = group->AddTabCtrl(1, 0, 45, 180);

		// Gebäudetabs hinzufügen
		switch(tabs.build_tabs)
		{
		case Tabs::BT_HUT:
			{
				build_tab->AddTab(LOADER.GetImageN("io", 67), _("-> Build hut"), Tabs::BT_HUT);
			} break;
		case Tabs::BT_HOUSE:
			{
				build_tab->AddTab(LOADER.GetImageN("io", 67), _("-> Build hut"), Tabs::BT_HUT);
				build_tab->AddTab(LOADER.GetImageN("io", 68), _("-> Build house"), Tabs::BT_HOUSE);
			} break;
		case Tabs::BT_CASTLE:
		case Tabs::BT_HARBOR:
			{
				build_tab->AddTab(LOADER.GetImageN("io", 67), _("-> Build hut"), Tabs::BT_HUT);
				build_tab->AddTab(LOADER.GetImageN("io", 68), _("-> Build house"), Tabs::BT_HOUSE);
				build_tab->AddTab(LOADER.GetImageN("io", 69), _("-> Build castle"), Tabs::BT_CASTLE);
			} break;
		case Tabs::BT_MINE:
			{
				build_tab->AddTab(LOADER.GetImageN("io", 76), _("-> Dig mines"), Tabs::BT_MINE);
			} break;
		}

		// Gebäudeicons dem TabCtrl hinzufügen
		const unsigned building_count[4] = { 9, 13, 4, 4 };
		const BuildingType building_icons[4][13] =
		{
			{ BLD_WOODCUTTER, BLD_FORESTER, BLD_QUARRY, BLD_FISHERY, BLD_HUNTER, BLD_BARRACKS, BLD_GUARDHOUSE, BLD_LOOKOUTTOWER, BLD_WELL },
			{ BLD_SAWMILL, BLD_SLAUGHTERHOUSE, BLD_MILL, BLD_BAKERY, BLD_IRONSMELTER, BLD_METALWORKS, BLD_ARMORY, BLD_MINT, BLD_SHIPYARD, BLD_BREWERY, BLD_STOREHOUSE, BLD_WATCHTOWER, BLD_CATAPULT },
			{ BLD_FARM, BLD_PIGFARM, BLD_DONKEYBREEDER, BLD_FORTRESS, BLD_HARBORBUILDING},
			{ BLD_GOLDMINE, BLD_IRONMINE, BLD_COALMINE, BLD_GRANITEMINE}
		};

		const unsigned TABS_COUNT[5] = {1,2,3,1,3};

		for(unsigned char i = 0; i < TABS_COUNT[tabs.build_tabs]; ++i)
		{
			unsigned char k = 0;
			Tabs::BuildTab bt = (tabs.build_tabs == Tabs::BT_MINE) ? Tabs::BT_MINE : Tabs::BuildTab(i);
		
			// Anzahl der Gebäude-Icons in dieser Tab-Gruppe ermiteln
			unsigned count = building_count[bt];
			// Ggf. noch den Hafen dazurechnen
			if(tabs.build_tabs == Tabs::BT_HARBOR && bt == Tabs::BT_CASTLE)
				++count;
			for(unsigned char j = 0; j < count; ++j)
			{
				// Wenn Militärgebäude an der Stelle nicht erlaubt sind, überspringen
				if(!military_buildings && building_icons[bt][j] >= BLD_BARRACKS && 
					building_icons[i][j] <= BLD_FORTRESS)
					continue;

				build_tab->GetGroup(bt)->AddBuildingIcon(j, (k%5)*36, (k/5)*36 + 45, building_icons[bt][j], GameClient::inst().GetLocalPlayer()->nation, 36, _(BUILDING_NAMES[building_icons[bt][j]]));
				++k;
			}
		}

		build_tab->SetSelection(0, true);
	}


	// Wenn es einen Flaggen-main_tab gibt, dann entsprechend die Buttons anordnen, wie sie gebraucht werden
	if(tabs.flag)
	{
		ctrlGroup *group = main_tab->AddTab(LOADER.GetImageN("io", 70), _("Erect flag"), TAB_FLAG);

		switch(params)
		{
		case AWFT_NORMAL: // normale Flagge
			{
				group->AddImageButton(1,  0, 45, 45, 36, TC_GREY, LOADER.GetImageN("io",  65), _("Build road"));
				group->AddImageButton(3,  45, 45, 45, 36, TC_GREY, LOADER.GetImageN("io", 118), _("Pull down flag"));
				group->AddImageButton(4, 90, 45, 45, 36, TC_GREY, LOADER.GetImageN("io",  20), _("Call in geologist"));
				group->AddImageButton(5, 135, 45, 45, 36, TC_GREY, LOADER.GetImageN("io",  96), _("Send out scout"));
			} break;
		case AWFT_WATERFLAG: // Wasserflagge
			{
				group->AddImageButton(1,  0, 45, 36, 36, TC_GREY, LOADER.GetImageN("io",  65), _("Build road"));
				group->AddImageButton(2,  36, 45, 36, 36, TC_GREY, LOADER.GetImageN("io",  95), _("Build waterway"));
				group->AddImageButton(3,  72, 45, 36, 36, TC_GREY, LOADER.GetImageN("io", 118), _("Pull down flag"));
				group->AddImageButton(4, 108, 45, 36, 36, TC_GREY, LOADER.GetImageN("io",  20), _("Call in geologist"));
				group->AddImageButton(5, 144, 45, 36, 36, TC_GREY, LOADER.GetImageN("io",  96), _("Send out scout"));
			} break;
		case AWFT_HQ: // Haupthaus
			{
				group->AddImageButton(1, 0, 45, 180, 36, TC_GREY, LOADER.GetImageN("io", 65), _("Build road"));
			} break;
		case AWFT_STOREHOUSE: // Lagerhaus
			{
				group->AddImageButton(1,0,45,90,36,TC_GREY, LOADER.GetImageN("io", 65), _("Build road"));
				group->AddImageButton(3,90,45,90,36,TC_GREY, LOADER.GetImageN("io", 118), _("Demolish house"));
			} break;
		}
	}

	// Flagge Setzen-main_tab
	if(tabs.setflag)
	{
		ctrlGroup *group = main_tab->AddTab(LOADER.GetImageN("io", 45), _("Erect flag"), TAB_SETFLAG);

		unsigned int nr = 70;
		if(params == AWFT_WATERFLAG)
			nr = 94;

		group->AddImageButton(1, 0, 45, 180, 36, TC_GREY, LOADER.GetImageN("io",  nr),_("Erect flag"));
	}

	// Cut-main_tab
	if(tabs.cutroad)
	{
		ctrlGroup *group = 	main_tab->AddTab(LOADER.GetImageN("io", 19), _("Dig up road"), TAB_CUTROAD);

		group->AddImageButton(1, 0, 45, 180, 36, TC_GREY, LOADER.GetImageN("io", 32), _("Dig up road"));
	}

	if(tabs.attack)
	{
		ctrlGroup *group = main_tab->AddTab(LOADER.GetImageN("io", 98), _("Attack options"), TAB_ATTACK);
		available_soldiers_count = params;
		AddAttackControls(group,params);
		selected_soldiers_count = 1;
	}
	
	// Beobachten-main_tab
	if(tabs.watch)
	{
		ctrlGroup *group = main_tab->AddTab(LOADER.GetImageN("io", 36), _("Display options"), TAB_WATCH);

		group->AddImageButton(1, 0, 45,  60, 36, TC_GREY, LOADER.GetImageN("io", 108), _("Observation window"));
		group->AddImageButton(2,  60, 45,  60, 36, TC_GREY, LOADER.GetImageN("io", 179), _("House names"));
		group->AddImageButton(3, 120, 45,  60, 36, TC_GREY, LOADER.GetImageN("io", 180), _("Go to headquarters"));
	}

	if(tabs.sea_attack)
	{
		ctrlGroup *group = main_tab->AddTab(LOADER.GetImageN("io", 177), _("Attack options"), TAB_SEAATTACK);

		selected_soldiers_count_sea = 1;
		available_soldiers_count_sea = GameClient::inst().GetLocalPlayer()->GetAvailableSoldiersForSeaAttack(selected_x,selected_y);

		AddAttackControls(group,available_soldiers_count_sea);
	}

	
	main_tab->SetSelection(0, true);

	if(x+GetWidth() > SETTINGS.video.width)
		x = mouse_x-GetWidth()-40;
	if(y+GetHeight() > SETTINGS.video.height)
		y = mouse_y-GetHeight()-40;

	VideoDriverWrapper::inst().SetMousePos(GetX()+20,GetY()+75);
}

/// Fügt Angriffs-Steuerelemente für bestimmte Gruppe hinzu
void iwAction::AddAttackControls(ctrlGroup * group, const unsigned attackers_count)
{
	// Verfügbare Soldatenzahl steht in params, wenns keine gibt, einfach Meldung anzeigen: "Angriff nicht möglich!"
	if(attackers_count == 0)
	{
		// Angriff nicht  möglich!
		group->AddText(1, 90, 56, _("Attack not possible."), COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);
	}
	else
	{
		selected_soldiers_count = 1;

		// Minus und Plus - Button
		group->AddImageButton(1, 3, 49, 26, 32, TC_GREY, LOADER.GetImageN("io", 139), _("Less attackers"));
		group->AddImageButton(2, 89, 49, 26, 32, TC_GREY, LOADER.GetImageN("io", 138), _("More attackers"));

		// Starke/Schwache Soldaten
		ctrlOptionGroup *ogroup = group->AddOptionGroup(3, ctrlOptionGroup::ILLUMINATE);
		ogroup->AddImageButton(0, 146, 49, 30, 33, TC_GREY, LOADER.GetImageN("io", 31), _("Weak attackers"));
		ogroup->AddImageButton(1, 117, 49, 30, 33, TC_GREY, LOADER.GetImageN("io", 30), _("Strong attackers"));
		// standardmäßig starke Soldaten
		ogroup->SetSelection(1);

		// Schnellauswahl-Buttons
		unsigned int buttons_count = (attackers_count > 3) ? 4 : attackers_count;
		unsigned short button_width = 112 / buttons_count;

		for(unsigned i = 0;i < buttons_count; ++i)
			group->AddImageButton(10+i, 3 + i*button_width, 83, button_width, 32, TC_GREY, LOADER.GetImageN("io",204+i), _("Number of attackers"));

		// Angriffsbutton
		group->AddImageButton(4, 117, 83, 59, 32, TC_RED1, LOADER.GetImageN("io", 25), _("Attack!"));
	}
}


iwAction::~iwAction()
{
	VideoDriverWrapper::inst().SetMousePos(last_x, last_y);
	gi->ActionWindowClosed();
}

void iwAction::Msg_Group_ButtonClick(const unsigned int group_id, const unsigned int ctrl_id)
{
	switch(GetCtrl<ctrlTab>(0)->GetCurrentTab())
	{
	default:
		break;

	case TAB_ATTACK:
		{
			Msg_ButtonClick_TabAttack(ctrl_id);
		} break;

	case TAB_FLAG:
		{
			Msg_ButtonClick_TabFlag(ctrl_id);
		} break;

	case TAB_BUILD:
		{
			Msg_ButtonClick_TabBuild(ctrl_id);
		} break;

	case TAB_SETFLAG:
		{
			Msg_ButtonClick_TabSetFlag(ctrl_id);
		} break;

	case TAB_CUTROAD:
		{
			Msg_ButtonClick_TabCutRoad(ctrl_id);
		} break;

	case TAB_WATCH:
		{
			Msg_ButtonClick_TabWatch(ctrl_id);
		} break;
	}
}


void iwAction::Msg_TabChange(const unsigned int ctrl_id, const unsigned short tab_id)
{
	switch(ctrl_id)
	{
	case 0: // Haupttabs
		{
			unsigned short height = 0;
			switch(tab_id)
			{
			case TAB_FLAG:    height = 138; break;
			case TAB_CUTROAD: height = 138; break;
			case TAB_BUILD:
				{
					// unterschiedliche Höhe, je nachdem welches Gebäude
					switch(GetCtrl<ctrlTab>(0)->GetGroup(TAB_BUILD)->GetCtrl<ctrlTab>(1)->GetCurrentTab())
					{
					case Tabs::BT_HUT:    height = 222; break;
					case Tabs::BT_HOUSE:  height = 258; break;
					case Tabs::BT_CASTLE: height = 186; break;
					case Tabs::BT_MINE:   height = 186; break;
					}
				} break;
			case TAB_SETFLAG: height = 138; break;
			case TAB_ATTACK:
				{
					if(available_soldiers_count > 0)
						height = 178;
					else
						height = 130;
				} break;
			case TAB_WATCH:   height = 138; break;
			}


			SetHeight(height);
		} break;
	}
	
}

void iwAction::Msg_Group_TabChange(const unsigned group_id,const unsigned int ctrl_id, const unsigned short tab_id)
{
	switch(ctrl_id)
	{
		case 1: // Gebäudetabs
		{
			unsigned short height = 0;
			switch(tab_id)
			{
			case Tabs::BT_HUT:    height = 222; break;
			case Tabs::BT_HOUSE:  height = 258; break;
			case Tabs::BT_CASTLE: height = 186; break;
			case Tabs::BT_MINE:   height = 186; break;
			}
			SetHeight(height);
		} break;
	}
}


void iwAction::Msg_PaintAfter()
{
	ctrlTab *tab = GetCtrl<ctrlTab>(0);
	if(tab)
	{
		// Anzeige Soldaten/mögliche Soldatenanzahl bei Angriffstab
		if(tab->GetCurrentTab() == TAB_ATTACK && available_soldiers_count > 0)
		{
			char str[32];
			sprintf(str, "%u/%u", selected_soldiers_count, available_soldiers_count);
			LargeFont->Draw(GetX()+67, GetY()+79, str, glArchivItem_Font::DF_CENTER, COLOR_YELLOW);
		}
	}
}



void iwAction::Msg_ButtonClick_TabAttack(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 1: // 1 Soldat weniger
		{
			if(selected_soldiers_count > 1)
				--selected_soldiers_count;
		} break;
	case 2: // 1 Soldat mehr
		{
			if(selected_soldiers_count < available_soldiers_count)
				++selected_soldiers_count;
		} break;
	case 10: // auf bestimmte Anzahl setzen
	case 11:
	case 12:
	case 13:
		{
			if(available_soldiers_count > 4)
				selected_soldiers_count = (ctrl_id-9) * available_soldiers_count / 4;
			else
				selected_soldiers_count = ctrl_id-9;
		} break;
	case 4: // Angriff!
		{
			ctrlOptionGroup *ogroup = GetCtrl<ctrlTab>(0)->GetGroup(TAB_ATTACK)->GetCtrl<ctrlOptionGroup>(3);

			GAMECLIENT.AddGC(new gc::Attack(selected_x, selected_y, selected_soldiers_count, (ogroup->GetSelection() == 1)));

			Close();
		} break;
	}
}

void iwAction::Msg_ButtonClick_TabFlag(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 1: // Straße bauen
		{
			gi->ActivateRoadMode(RM_NORMAL);
			Close();
		} break;
	case 2: // Wasserstraße bauen
		{
			gi->ActivateRoadMode(RM_BOAT);
			Close();
		} break;
	case 3: // Flagge abreißen
		{
			// Haben wir ne Baustelle/Gebäude dran?
			if(gwv->GetNO(selected_x-!(selected_y&1), selected_y-1)->GetType() == NOP_BUILDING ||
			   gwv->GetNO(selected_x-!(selected_y&1), selected_y-1)->GetType() == NOP_BUILDINGSITE)
			{
				// Abreißen?
				Close();
				noBaseBuilding * building = gwv->GetSpecObj<noBaseBuilding>(selected_x-!(selected_y&1), selected_y-1);

				// Militärgebäude?
				if(building->GetGOT() == GOT_NOB_MILITARY)
				{
					// Darf das Gebäude abgerissen werden?
					if(!static_cast<nobMilitary*>(building)->IsDemolitionAllowed())
					{
						// Nein, dann Messagebox anzeigen
						iwMilitaryBuilding::DemolitionNotAllowed();
						break;
					}
					
				}

				WindowManager::inst().Show(new iwDemolishBuilding(gwv,gwv->GetNO(selected_x,selected_y)->GetGOT(),
					selected_x, selected_y,building->GetBuildingType(),building->GetNation(),building->CreateGUIID()));
			}
			else
			{
				GAMECLIENT.AddGC(new gc::DestroyFlag(selected_x, selected_y));
				Close();
			}
		} break;
	case 4: // Geologen rufen
		{
			GAMECLIENT.AddGC(new gc::CallGeologist(selected_x, selected_y));
			Close();
		} break;
	case 5: // Späher rufen
		{
			GAMECLIENT.AddGC(new gc::CallScout(selected_x, selected_y));
			Close();
		} break;
	}
}

void iwAction::Msg_ButtonClick_TabBuild(const unsigned int ctrl_id)
{
	// Klick auf Gebäudebauicon
	GAMECLIENT.AddGC(new gc::SetBuildingSite(selected_x, selected_y, 
		GetCtrl<ctrlTab>(0)->GetGroup(TAB_BUILD)->GetCtrl<ctrlTab>(1)->GetCurrentGroup()->
		GetCtrl<ctrlBuildingIcon>(ctrl_id)->GetType()));

	// Fenster schließen
	Close();
}

void iwAction::Msg_ButtonClick_TabSetFlag(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 1: // Flagge setzen
		{
			GAMECLIENT.AddGC(new gc::SetFlag(selected_x, selected_y));
		} break;
	}

	Close();
}

void iwAction::Msg_ButtonClick_TabCutRoad(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 1: // Straße abreißen
		{
			unsigned char flag_dir = 0;
			noFlag *flag = gwv->GetRoadFlag(selected_x, selected_y, flag_dir);
			if(flag)
				GAMECLIENT.AddGC(new gc::DestroyRoad(flag->GetX(), flag->GetY(), flag_dir));
		} break;
	}

	Close();
}

void iwAction::Msg_ButtonClick_TabWatch(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 1:
		{
		} break;
	case 2: // Häusernamen/Prozent anmachen
		{
			gwv->ShowNamesAndProductivity();
		} break;
	case 3: // zum HQ
		{
			GameClientPlayer *player = GAMECLIENT.GetLocalPlayer();
			gwv->MoveToMapObject(player->hqx,player->hqy);
		} break;
	}

	Close();

}

