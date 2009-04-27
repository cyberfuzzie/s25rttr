// $Id: ctrlComboBox.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "ctrlComboBox.h"

#include "ctrlButton.h"
#include "ctrlList.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ctrlComboBox.
 *
 *  @author OLiver
 */
ctrlComboBox::ctrlComboBox(Window *parent, 
					 unsigned int id, 
					 unsigned short x, 
					 unsigned short y, 
					 unsigned short width,
					 unsigned short height,
					 TextureColor tc, 
					 glArchivItem_Font *font, 
					 unsigned short list_height, 
					 bool readonly)
	: Window(x, y, id, parent), 
	width(width), height(height), tc(tc), font(font), list_height(list_height), readonly(readonly), selection(0), last_show(false)
{
	ctrlList *liste = AddList(0, 0, height, width, height, tc, font);

	// Liste am Anfang nicht anzeigen
	liste->SetVisible(false);

	if(!readonly)
		AddImageButton(1, width-height, 0, height, height, tc, GetImage(io_dat, 34));
}

void ctrlComboBox::Msg_PaintAfter()
{
	// Button und Liste malen
	DrawControls();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlComboBox::Msg_MouseMove(const MouseCoords& mc)
{
	// Für Button und Liste weiterleiten
	return RelayMouseMessage(&Window::Msg_MouseMove, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlComboBox::Msg_LeftDown(const MouseCoords& mc)
{
	ctrlList *list = GetCtrl<ctrlList>(0);

	// Irgendwo anders hingeklickt --> Liste ausblenden
	if(!readonly && !Coll(mc.x, mc.y, GetX(), GetY(), width, height + list->GetHeight()))
	{
		// Liste wieder ausblenden
		ShowList(false);
		return false;
	}
	else if(!readonly && Coll(mc.x, mc.y, GetX(), GetY(), width, height))
	{
		// Liste wieder ein/ausblenden
		ShowList(!list->GetVisible());
		return true;
	}
	else
	{
		// Für Button und Liste weiterleiten
		return RelayMouseMessage(&Window::Msg_LeftDown, mc);
	}
} 

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlComboBox::Msg_LeftUp(const MouseCoords& mc)
{
	// Für Button und Liste weiterleiten
	return RelayMouseMessage(&Window::Msg_LeftUp, mc);
} 

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void ctrlComboBox::Msg_ListSelectItem(const unsigned int ctrl_id, const unsigned short selection)
{
	// Liste wieder ausblenden
	ShowList(false);

	ctrlList *list = GetCtrl<ctrlList>(0);

	// ist in der Liste überhaupt was drin?
	if(list->GetLineCount() > 0)
	{
		list->SetSelection(selection);

		this->selection = selection;

		// Nachricht an übergeordnetes Fenster verschicken
		parent->Msg_ComboSelectItem(GetID(),selection);
	}
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt einen String zur Liste hinzu.
 * 
 *  @author OLiver
 */
void ctrlComboBox::AddString(const std::string& text)
{
	ctrlList *liste = GetCtrl<ctrlList>(0);

	liste->AddString(text);

	unsigned short height = 4;

	// Langsam die Höhe der maximalen annähern
	for(unsigned int i = 0; i < liste->GetLineCount(); ++i)
	{
		// zu große geworden?
		height += font->getHeight();

		if(height > list_height)
		{
			// kann nicht mal ein Item aufnehmen, dann raus
			if(i == 0)
				return;

			// Höhe um eins erniedrigen, damits wieder kleiner ist als die maximale
			height -= font->getHeight();;

			break;
		}
	}

	liste->SetHeight(height);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  löscht alle Items der Liste.
 * 
 *  @author OLiver
 */
void ctrlComboBox::DeleteAllItems()
{
	GetCtrl<ctrlList>(0)->DeleteAllItems();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  wählt ein Item aus
 * 
 *  @author OLiver
 */
void ctrlComboBox::SetSelection(unsigned short selection)
{
	this->selection = selection;
	GetCtrl<ctrlList>(0)->SetSelection(selection);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  zeichnet das Fenster.
 * 
 *  @author OLiver
 */
bool ctrlComboBox::Draw_(void)
{
	ctrlList *liste = GetCtrl<ctrlList>(0);

	// Box
	Draw3D(GetX(), GetY(), width, height, tc, 2);

	// Namen des selektierten Strings in der Box anzeigen
	if(liste->GetLineCount() > 0)
		font->Draw(GetX() + 2, GetY() + height / 2, liste->GetSelItemText(), glArchivItem_Font::DF_VCENTER, COLOR_YELLOW, 0, width-2-height, "");

	return true;
}



///////////////////////////////////////////////////////////////////////////////
/**
 *  blendet die Liste ein oder aus.
 * 
 *  @author OLiver
 */
void ctrlComboBox::ShowList(bool show)
{
	if(last_show == show)
		return;

	last_show = show;

	ctrlList *liste = GetCtrl<ctrlList>(0);

	// Liste entsprechend
	liste->SetVisible(show);

	// Pfeilbutton entsprechend
	GetCtrl<ctrlButton>(1)->SetCheck(show);

	// Region sperren für die Liste, oder freigeben
	if(show)
	{
		Rect list_region = {
			liste->GetX(),
			liste->GetY(),
			liste->GetX()+width,
			liste->GetY()+liste->GetHeight()
		};

		parent->LockRegion(this, list_region);
	}
	else
	{
		parent->FreeRegion(this);
	}

	GetSound(sound_lst, 113)->Play(255,false);
}
