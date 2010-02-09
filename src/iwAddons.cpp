// $Id$
//
// Copyright (c) 2005-2010 Settlers Freaks (sf-team at siedler25.org)
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
#include "iwAddons.h"
#include "controls.h"

#include "AddonManager.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
iwAddons::iwAddons(ChangePolicy policy)
	: IngameWindow(CGI_ADDONS, 0xFFFF, 0xFFFF, 700, 500, _("Addon Settings"), LOADER.GetImageN("resource", 41), true),
	policy(policy)
{
	AddText(0, 20, 30, _("Additional features:"), COLOR_YELLOW, 0, NormalFont);

	if(policy != READONLY)
		AddTextButton(1,  20, height-40, 200, 22, TC_GREY, _("Apply Changes"), NormalFont);
	
	AddTextButton(2, 250, height-40, 200, 22, TC_RED1, _("Close Without Saving"), NormalFont);
	
	if(policy != READONLY)
		AddTextButton(3, 480, height-40, 200, 22, TC_GREY, _("Use S2 Defaults"), NormalFont);

	// Kategorien
	ctrlOptionGroup *optiongroup = AddOptionGroup(5, ctrlOptionGroup::CHECK, scale);
	// "Alle"
	optiongroup->AddTextButton(ADDONGROUP_ALL,  20, 50, 120, 22, TC_GREEN2, _("All"), NormalFont);
	// "Milit�r"
	optiongroup->AddTextButton(ADDONGROUP_MILITARY, 150, 50, 120, 22, TC_GREEN2, _("Military"), NormalFont);
	// "Wirtschaft"
	optiongroup->AddTextButton(ADDONGROUP_ECONOMY, 290, 50, 120, 22, TC_GREEN2, _("Economy"), NormalFont);
	// "Spielverhalten"
	optiongroup->AddTextButton(ADDONGROUP_GAMEPLAY, 430, 50, 120, 22, TC_GREEN2, _("Gameplay"), NormalFont);
	// "Sonstiges"
	optiongroup->AddTextButton(ADDONGROUP_OTHER, 560, 50, 120, 22, TC_GREEN2, _("Other"), NormalFont);

	optiongroup->SetSelection(ADDONGROUP_ALL, true);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
iwAddons::~iwAddons()
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
void iwAddons::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	default:
		break;

	case 1: // Apply changes
		{
			if(policy == READONLY)
				Close();

			// Einstellungen in ADDONMANAGER �bertragen
			for(unsigned int i = 0; i < ADDONMANAGER.getCount(); ++i)
			{
				unsigned int status;
				const Addon *addon = ADDONMANAGER.getAddon(i, status);

				if(!addon)
					continue;

				ctrlComboBox *c = GetCtrl<ctrlComboBox>(10 + 5*i + 2);
				if(!c)
					continue;

				ADDONMANAGER.setSelection(addon->getId(), c->GetSelection());
			}

			switch(policy)
			{
			default:
				break;
			case SETDEFAULTS:
				{
					ADDONMANAGER.SaveSettings();
				} break;
			case HOSTGAME:
				{
					// send message via msgboxresult
					MsgboxResult mbr = MSR_YES;
					parent->Msg_MsgBoxResult(GetID(), mbr);
				} break;
			}
			Close();
		} break;

	case 2: // Discard changes
		{
			Close();
		} break;

	case 3: // Load S2 Defaults
		{
			// Standardeinstellungen aufs Fenster �bertragen
			for(unsigned int i = 0; i < ADDONMANAGER.getCount(); ++i)
			{
				unsigned int status;
				const Addon *addon = ADDONMANAGER.getAddon(i, status);

				if(!addon)
					continue;

				ctrlComboBox *c = GetCtrl<ctrlComboBox>(10 + 5*i + 2);
				if(!c)
					continue;

				c->SetSelection(addon->getDefaultStatus());
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
void iwAddons::Msg_OptionGroupChange(const unsigned int ctrl_id, const unsigned short selection)
{
	switch(ctrl_id)
	{
	case 5: // richtige Kategorie anzeigen
		{
			int y = 90;
			for(unsigned int i = 0; i < ADDONMANAGER.getCount(); ++i)
			{
				unsigned int status;
				const Addon *addon = ADDONMANAGER.getAddon(i, status);

				if(!addon)
					continue;

				ctrlText *text = GetCtrl<ctrlText>(10 + 5*i);
				ctrlImageButton *button = GetCtrl<ctrlImageButton>(10 + 5*i + 1);
				ctrlComboBox *combo = GetCtrl<ctrlComboBox>(10 + 5*i +2);

				unsigned int groups = addon->getGroups();
				if( (groups & selection) != selection)
				{
					// ggf unsichtbar machen
					if(text)	text->SetVisible(false);
					if(button)	button->SetVisible(false);
					if(combo)	combo->SetVisible(false);
					continue;
				}

				// ggf buttons usw erzeugen
				if(!text)
					text = AddText(10 + 5*i, 52, y+4, "", COLOR_YELLOW, 0, NormalFont);
				if(!button)
					button = AddImageButton(10 + 5*i + 1, 20, y, 22, 22, TC_GREY, LOADER.GetImageN("io", 21));
				if(!combo)
					combo = AddComboBox(10 + 5*i + 2, 430, y, 250, 20,  TC_GREY, NormalFont, 100, (policy == READONLY) );

				// ggf an richtige Stelle schieben
				text->Move(52, y + 4);
				button->Move(20, y);
				combo->Move(430, y); 

				// und sichtbar machen
				text->SetVisible(true);
				button->SetVisible(true);
				combo->SetVisible(true);

				text->SetText(_(addon->getName()));
				button->SetTooltip(addon->getDescription());

				const AddonList *addonl = dynamic_cast<const AddonList *>(addon);
				const AddonBool *addonb = dynamic_cast<const AddonBool *>(addon);
				

				combo->DeleteAllItems();

				if(addonl)
				{
					addonl->fillComboBox(combo);
				}
				else if(addonb)
				{
					combo->AddString(_("Disabled"));
					combo->AddString(_("Use"));
				}
				combo->SetSelection(status);

				y += 30;
			}		
		} break;
	}
}