// $Id: ctrlOptionGroup.cpp 4793 2009-05-04 15:37:10Z OLiver $
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
#include "ctrlOptionGroup.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ctrlOptionGroup.
 *
 *  @author OLiver
 */
ctrlOptionGroup::ctrlOptionGroup(Window *parent,
								 unsigned int id,
								 int select_type,
								 bool scale)
	: ctrlGroup(parent, id, scale),
	selection(0xFFFF), select_type(select_type)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeichenmethode.
 *
 *  @author OLiver
 */
bool ctrlOptionGroup::Draw_(void)
{
	DrawControls();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  w�hlt einen Button aus der Gruppe aus.
 *
 *  @author OLiver
 */
void ctrlOptionGroup::SetSelection(unsigned short selection, bool notify)
{
	// Aktuellen ausgew�hlten Button wieder normal machen
	if(this->selection != 0xFFFF)
	{
		ctrlButton *button = GetCtrl<ctrlButton>(this->selection);
		switch(select_type)
		{
		case ILLUMINATE: button->SetIlluminated(false); break;
		case CHECK:      button->SetCheck(false);       break;
		case SHOW:       button->SetVisible(true);     break;
		}
	}

	// Neuen Button ausw�hlen
	if(selection != 0xFFFF)
	{
		ctrlButton *button = GetCtrl<ctrlButton>(selection);
		switch(select_type)
		{
		case ILLUMINATE: button->SetIlluminated(true); break;
		case CHECK:      button->SetCheck(true);       break;
		case SHOW:       button->SetVisible(false);     break;
		}
	}

	this->selection = selection;

	if(notify && parent)
		parent->Msg_OptionGroupChange(GetID(), selection);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void ctrlOptionGroup::Msg_ButtonClick(const unsigned int ctrl_id)
{
	SetSelection(ctrl_id, true);
} 

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlOptionGroup::Msg_LeftDown(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_LeftDown, mc);
} 

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlOptionGroup::Msg_LeftUp(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_LeftUp, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlOptionGroup::Msg_WheelUpDown(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_WheelUpDown,mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlOptionGroup::Msg_WheelUpUp(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_WheelUpUp,mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlOptionGroup::Msg_WheelDownDown(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_WheelDownDown,mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlOptionGroup::Msg_WheelDownUp(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_WheelDownUp,mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlOptionGroup::Msg_MouseMove(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_MouseMove, mc);
}
