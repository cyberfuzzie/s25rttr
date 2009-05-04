// $Id: ctrlList.cpp 4793 2009-05-04 15:37:10Z OLiver $
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
#include "ctrlList.h"

#include "ctrlScrollBar.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ctrlList.
 *
 *  @author OLiver
 */
ctrlList::ctrlList(Window *parent, 
				   unsigned int id,
				   unsigned short x,
				   unsigned short y,
				   unsigned short width,
				   unsigned short height,
				   TextureColor tc,
				   glArchivItem_Font *font)
	: Window(x, y, id, parent),
	width(width), height(height), tc(tc), font(font), selection(0xFFFF), mouseover(0xFFFF)
{
	pagesize = (height - 4) / font->getHeight();
	
	AddScrollBar(0, width - 20, 0, 20, height, 20, tc, pagesize);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ctrlList.
 *
 *  @author OLiver
 */
ctrlList::~ctrlList()
{
	DeleteAllItems();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
bool ctrlList::Msg_MouseMove(const MouseCoords& mc)
{
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// Wenn Maus in der Liste
	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - 22, height - 4))
	{
		// Neue Selektierung
		mouseover = (mc.y - (GetY() + 2) ) / font->getHeight();
		return true;
	}
	else
	{
		mouseover = 0xFFFF;
		// Für die Scrollbar weiterleiten
		return  scrollbar->Msg_MouseMove(mc);
	}

	
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
bool ctrlList::Msg_LeftDown(const MouseCoords& mc)
{
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// Wenn Maus in der Liste
	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - 22, height - 4))
	{
		// aktuellen Eintrag selektieren
		selection = mouseover + scrollbar->GetPos();
		parent->Msg_ListSelectItem(id,selection);

		return true;
	}
	else
	{
		// Für die Scrollbar weiterleiten
		return scrollbar->Msg_LeftDown(mc);
	}

	
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
bool ctrlList::Msg_LeftUp(const MouseCoords& mc)
{
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// Für die Scrollbar weiterleiten
	return scrollbar->Msg_LeftUp(mc);
} 

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlList::Msg_WheelUpUp(const MouseCoords& mc)
{
	// Forward to ScrollBar
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// If mouse in list or scrollbar
	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - /*2*/2, height - 4))
	{
		// Simulate Button Click
		scrollbar->Msg_ButtonClick(0);
		return true;
	}
	else
		return false;
}

bool ctrlList::Msg_WheelDownUp(const MouseCoords& mc)
{
	// Forward to ScrollBar
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// If mouse in list
	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - /*2*/2, height - 4))
	{
		// Simulate Button Click
		scrollbar->Msg_ButtonClick(1);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeichenmethode.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author OLiver
 */
bool ctrlList::Draw_(void)
{
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// Box malen
	Draw3D(GetX(), GetY(), width, height, tc, 2);

	// Scrolleiste zeichnen
	DrawControls();

	// Wieviele Linien anzeigen?
	unsigned show_lines = (pagesize > lines.size() ? unsigned(lines.size()) : pagesize);

	// Listeneinträge zeichnen
	for(unsigned short i = 0;i < show_lines; ++i)
	{
		// Schwarze Markierung, wenn die Maus drauf ist
		if(i == mouseover)
			DrawRectangle(GetX() + 2, GetY() + 2 + i * font->getHeight(), width - 22, font->getHeight(), 0x80000000);

		// Text an sich
		font->Draw(GetX() + 2, GetY() + 2 + i*font->getHeight(), lines[i+scrollbar->GetPos()], 0, (selection == i + scrollbar->GetPos() ? 0xFFFFAA00 : 0xFFFFFF00), 0, width - 22);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt eine Zeile hinzu.
 *
 *  @author FloSoft
 */
void ctrlList::AddString(const std::string& text)
{
	// lines-Array ggf vergrößern
	lines.push_back(text);

	GetCtrl<ctrlScrollBar>(0)->SetRange(static_cast<unsigned short>(lines.size()));
}

/// Verändert einen String
void ctrlList::SetString(const std::string& text, const unsigned id)
{
	lines[id] = text;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  löscht alle Items.
 *
 *  @author FloSoft
 */
void ctrlList::DeleteAllItems()
{
	lines.clear();
	selection = 0xFFFF;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Wert einer Zeile.
 *
 *  @param[in] line Die Zeile
 *
 *  @return Text der Zeile
 *
 *  @author OLiver
 */
const std::string& ctrlList::GetItemText(unsigned short line) const
{
	if(line < lines.size())
		return lines.at(line);
	else
		return EMPTY_STRING;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Höhe.
 *
 *  @param[in] height Die neue Höhe der Listbox
 *
 *  @author OLiver
 */
void ctrlList::SetHeight(const unsigned short height)
{
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	pagesize = (height-4) / font->getHeight();
	this->height = height;

	scrollbar->SetPageSize(pagesize);
	scrollbar->SetHeight(height);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  vertauscht zwei Zeilen
 *
 *  @author OLiver
 */
void ctrlList::Swap(unsigned short first, unsigned short second)
{
	// Evtl Selection auf das jeweilige Element beibehalten?
	if(first == selection)
		selection = second;
	else if(second == selection)
		selection = first;

	// Strings vertauschen
	::Swap(lines[first], lines[second]);
}


void ctrlList::Remove(const unsigned short index)
{
	if(index < lines.size())
	{
		lines.erase(lines.begin()+index);
		// Ggf. selection korrigieren
		if(selection)
			--selection;
		else
			selection = 0xFFFF;
	}
}
