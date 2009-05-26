// $Id: iwEnhancements.cpp 4800 2009-03-29 10:10:02Z Divan $
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
#include <sstream>
#include "iwEnhancements.h"

#include "controls.h"
#include "GameClient.h"
#include "GlobalVars.h"
#include "WindowManager.h"

#include "Settings.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#define BEGIN_ENH_CTRLS 100

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwEnhancements.
 *
 *  @author Divan
 */
iwEnhancements::iwEnhancements(enhOptions * original, const unsigned char policy, void (dskHostGame::*notifyFunc)(), dskHostGame * notifyInst)
	: IngameWindow(CGI_ENHANCEMENTS, 0xFFFF, 0xFFFF, 700, 500, _("Enhancements"), GetImage(resource_dat, 41), 
	  (policy == ENH_POLICY_ALL || policy == ENH_POLICY_READONLY) ? false : true)
{
	this->policy = policy;
	// store a pointer to the options we want to view/change
	this->original = original;

	this->notifyFunc = notifyFunc;	
	this->notifyInst = notifyInst;

	switch(policy)
	{
	case ENH_POLICY_HOSTMENU:
	case ENH_POLICY_SERVERINGAME:
		{
			// make a local copy so we can apply changes later
			this->enhs = new enhOptions(*original);
		} break;
	case ENH_POLICY_ALL:
	case ENH_POLICY_READONLY:
		{
			// we don't need a copy
			enhs = original;
		} break;
	}

	// Buttons
	switch(policy)
	{
	case ENH_POLICY_HOSTMENU:
	case ENH_POLICY_SERVERINGAME:
		{
			AddTextButton(1, width - 220, height - 45 , 200, 22, TC_GREEN2, _("Apply Changes"),NormalFont);
			AddTextButton(2, 20, height - 45 , 200, 22, TC_RED1, _("Discard Changes"),NormalFont);
		} break;
	case ENH_POLICY_ALL:
	case ENH_POLICY_READONLY:
		{
			AddTextButton(2, width - 220, height - 45 , 200, 22, TC_GREEN2, _("Continue"),NormalFont);
		} break;
	}

	// Add controls for each option
	unsigned int BEGIN_OTHER_CTRLS = BEGIN_ENH_CTRLS + enhs->size();
	enhOption* enh;
	ctrlCheck* check;
	ctrlComboBox* combo;
	ctrlEdit* edit;
	ctrlText* text;
	unsigned int y = 30;
	bool isChangeable;
	
	if (enhs->size() > 0) for (unsigned int i = 0; i < enhs->size(); ++i)
	{
		enh = enhs->by_order(i);
		isChangeable = 
			(policy < ENH_POLICY_READONLY) 
			&& 
			(policy < ENH_POLICY_SERVERINGAME 
			 || 
			 enh->isIngameChangeable()
			);
		switch(enh->GetType())
		{
		case OPT_BOOL:
			{
				check = AddCheckBox(i + BEGIN_ENH_CTRLS, 30,  y, width - 60, 22, TC_GREY, enh->GetName(), NormalFont, !isChangeable);
				check->SetCheck(enh->GetState() == 1);
				y += 22;
			} break;
		case OPT_SELECT:
			{
				text = AddText(i + BEGIN_OTHER_CTRLS, 30, y + 3, enh->GetName() + ":", COLOR_YELLOW, 0, NormalFont);
				unsigned short int w = NormalFont->getWidth(text->GetText());
				combo = AddComboBox(i + BEGIN_ENH_CTRLS, 30 + w + 10, y, width - 60 - 10 - w, 20, TC_GREY, NormalFont, width - 160, !isChangeable); 
				if (enh->GetMax()>0) for(int j = 0; j <= enh->GetMax(); ++j)
					combo->AddString(enh->GetSelections(j));
				combo->SetSelection(enh->GetState());
				y += 20;
			} break;
		case OPT_INT:
			{
				text = AddText(i + BEGIN_OTHER_CTRLS, 30, y + 3, enh->GetName() + ":", COLOR_YELLOW, 0, NormalFont);
				unsigned short int w = NormalFont->getWidth(text->GetText());
				edit = AddEdit(i + BEGIN_ENH_CTRLS, 30 + w + 10, y, width - 60 - 10 - w, 22, TC_GREY, NormalFont, width - 160, false, !isChangeable , true); 
				// maybe add a special SpinBox control later
				std::stringstream ssvalue;
				ssvalue << enh->GetState();
				std::string svalue = ssvalue.str();
				edit->SetText(svalue);
				y += 22;
			} break;
		}
		y += 10;
	}

}

/**
 *  Destructor von @p iwEnhancements.
 *
 *  @author Divan
 */
iwEnhancements::~iwEnhancements()
{
	// If we made up an own instance, destroy it!
	// We could as well check the value of policy here
	if (original != enhs) 
		if(enhs != NULL) 
			delete enhs; 
}

void iwEnhancements::Msg_CheckboxChange(const unsigned int ctrl_id, const bool check)
{
	if (ctrl_id >= BEGIN_ENH_CTRLS)
		enhs->by_order(ctrl_id - BEGIN_ENH_CTRLS)->SetState(check ? 1 : 0);
}

void iwEnhancements::Msg_ComboSelectItem(const unsigned int ctrl_id, const  unsigned short selection)
{
	if (ctrl_id >= BEGIN_ENH_CTRLS)
		enhs->by_order(ctrl_id - BEGIN_ENH_CTRLS)->SetState(selection);
}
void iwEnhancements::Msg_EditChange(const unsigned int ctrl_id)
{
	if (ctrl_id >= BEGIN_ENH_CTRLS)
	{
		enhOption* enh = enhs->by_order(ctrl_id - BEGIN_ENH_CTRLS);
		ctrlEdit* edit = GetCtrl<ctrlEdit>(ctrl_id);
		if (!enh->SetState(atoi(edit->GetText().c_str())))
		// load stored value again, for the case of nonvalid input entered
		{
			std::stringstream ssvalue;
			ssvalue << enh->GetState();
			std::string svalue = ssvalue.str();
			edit->SetText(svalue);
		}
	}
}

void iwEnhancements::Msg_EditEnter(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 1:
		{
		} break;
	case 3:
		{
		} break;
	case 5:
		{
		} break;
	}
}




void iwEnhancements::Msg_ButtonClick(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	// Save Changes
	case 1: 
	{
		if (policy < ENH_POLICY_READONLY)
		{
			*original = *enhs;
			// Notify about Changes if neccessary
			if (policy > ENH_POLICY_ALL)
				if (notifyFunc != NULL)
					(notifyInst->*notifyFunc)();
		}
		Close();
	} break;
	// Discard Changes
	case 2:
	{
		Close();
	} break;
	}
}

