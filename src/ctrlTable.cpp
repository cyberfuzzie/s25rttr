// $Id: ctrlTable.cpp 4793 2009-05-04 15:37:10Z OLiver $
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
#include "ctrlTable.h"

#include "ctrlButton.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ctrlTable
 *
 *  @author OLiver
 */
ctrlTable::ctrlTable(Window *parent,
					 unsigned int id,
					 unsigned short x,
					 unsigned short y,
					 unsigned short width,
					 unsigned short height,
					 TextureColor tc,
					 glArchivItem_Font *font,
					 unsigned short column_count,
					 va_list liste)
	: Window(x, y, id, parent),
	width(width), height(height), tc(tc), font(font),
	row_l_selection(0xFFFF), row_r_selection(0xFFFF), 
	sort_column(0xFFFF), sort_direction(true)
{
	header_height = font->getHeight() + 10;
	line_count = (height - header_height - 2) / font->getHeight();

	// Scrollbar hinzufügen
	AddScrollBar(0, width - 20, 0, 20, height, 20, tc, line_count);

	if(column_count > 0)
	{
		for(unsigned short i = 0; i < column_count; ++i)
		{
			COLUMN c;

			const char *title = va_arg(liste, const char*);
			if(title != NULL)
				c.title = title;

			c.width = (unsigned short)va_arg(liste, int);

			// Button für die Spalte hinzufügen
			AddTextButton(i+1, 0, 0, 0, header_height, tc, c.title, font);

			columns.push_back(c);
		}

		ResetButtonWidths();
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ctrlTable
 *
 *  @author OLiver
 */
ctrlTable::~ctrlTable(void)
{
	DeleteAllItems();
	columns.clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  löscht alle Items.
 *
 *  @author OLiver
 */
void ctrlTable::DeleteAllItems(void)
{
	rows.clear();

	GetCtrl<ctrlScrollBar>(0)->SetRange(0);

	row_l_selection = 0xFFFF;
	row_r_selection = 0xFFFF;
	sort_column = 0xFFFF;
	sort_direction = true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Auswahl.
 *
 *  @param[in] selection Der Auswahlindex
 *  @param[in] left      Auswahl für linke (@p true) oder rechte (@p false) Maustaste setzen.
 *
 *  @author FloSoft
 *  @author OLiver
 */
void ctrlTable::SetSelection(unsigned short selection, bool left)
{
	if(selection >= rows.size())
		return;

	if(left)
		row_l_selection = selection;
	else
		row_r_selection = selection;

	if(parent)
		parent->Msg_TableSelectItem(id, selection);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt eine Zeile hinzu.
 *
 *  @param[in] alwaysnull Immer 0, wird nur für Liste gebraucht
 *  @param[in] ...        Die Werte für die Spalten.
 *
 *  @author OLiver
 */
void ctrlTable::AddRow(unsigned alwaysnull, ...)
{
	va_list liste;
	va_start(liste, alwaysnull);

	ROW r;
	for(unsigned short i = 0; i < columns.size(); ++i)
	{
		const char *text = va_arg(liste, const char*);

		if(text)
			r.columns.push_back(text);
		else
			r.columns.push_back("");
	}
	va_end(liste);

	rows.push_back(r);
	GetCtrl<ctrlScrollBar>(0)->SetRange(static_cast<unsigned short>(rows.size()));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Wert eines Feldes.
 *
 *  @param[in] row    Die Zeile
 *  @param[in] column Die Spalte
 *
 *  @return Text in Feld <@p column,@p row>
 *
 *  @author OLiver
 */
const std::string &ctrlTable::GetItemText(unsigned short row, unsigned short column) const
{
	static std::string empty;
	if(row >= rows.size() || column >= columns.size())
		return empty;

	return rows.at(row).columns.at(column);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  sortiert die Zeilen.
 *
 *  @param[in] column    Die Spalte nach der sortiert werden soll.
 *  @param[in] direction Die Richtung in die sortiert werden soll
 *                         @p NULL  - Wechsel,
 *                         @p true  - A-Z,
 *                         @p false - Z-A
 *
 *  @author OLiver
 */
void ctrlTable::SortRows(unsigned short column, bool *direction)
{
	if(direction)
		sort_direction = *direction;
	if(column >= columns.size())
		column = 0;
	if(column >= columns.size())
		return;

	for(unsigned short i = 0; i < rows.size(); ++i)
	{
		for(unsigned short r = 0; r < rows.size()-1; ++r)
		{
			if(sort_direction && sort_column == column)
			{
				if(rows.at(r).columns.at(column).compare(rows.at(r+1).columns.at(column)) < 0)
					std::swap(rows.at(r), rows.at(r+1));
			}
			else
			{
				if(rows.at(r).columns.at(column).compare(rows.at(r+1).columns.at(column)) > 0)
					std::swap(rows.at(r), rows.at(r+1));
			}
		}
	}
	if(sort_direction && sort_column == column)
		sort_direction = false;
	else
		sort_direction = true;

	sort_column = column;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeichenmethode
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool ctrlTable::Draw_(void)
{
	Draw3D(GetX(), GetY(), width, height, tc, 2);

	DrawControls();

	unsigned short lines = static_cast<unsigned short>((line_count > rows.size() ? rows.size() : line_count));
	ctrlScrollBar *scroll = GetCtrl<ctrlScrollBar>(0);

	for(unsigned short i = 0; i < lines; ++i)
	{
		if(row_l_selection == i + scroll->GetPos())
		{
			// durchsichtig schwarze Markierung malen
			DrawRectangle(GetX() + 2, GetY() + 2 + header_height + i*font->getHeight(), width - 4 - (scroll->GetVisible() ? 24 : 0), font->getHeight(), 0x80000000);
		}

		unsigned short pos = 0;
		for(unsigned short c = 0; c < columns.size(); ++c)
		{
			if(columns.at(c).width == 0)
				continue;

			font->Draw(GetX()+2+pos, GetY()+2 + header_height + i*font->getHeight(),rows.at(i+scroll->GetPos()).columns.at(c), 0, (row_l_selection == i + scroll->GetPos() ? 0xFFFFAA00 : COLOR_YELLOW), 0, GetCtrl<ctrlButton>(c+1)->GetWidth(), "");
			pos += GetCtrl<ctrlButton>(c+1)->GetWidth();
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void ctrlTable::Msg_ButtonClick(const unsigned int ctrl_id)
{
	SortRows(ctrl_id - 1);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlTable::Msg_LeftDown(const MouseCoords& mc)
{
	if(Coll(mc.x, mc.y, GetX(), GetY() + header_height, width - 20, height - header_height))
	{
		SetSelection((mc.y - header_height - GetY()) / font->getHeight() + GetCtrl<ctrlScrollBar>(0)->GetPos());
		if(parent)
			parent->Msg_TableRightButton(this->id, row_l_selection);

		return true;
	}
	else
		return RelayMouseMessage(&Window::Msg_LeftDown, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlTable::Msg_RightDown(const MouseCoords& mc)
{
	if(Coll(mc.x, mc.y, GetX(), GetY()+header_height, width-20, height))
	{
		SetSelection((mc.y - header_height - GetY()) / font->getHeight() + GetCtrl<ctrlScrollBar>(0)->GetPos(), false);
		if(parent)
			parent->Msg_TableLeftButton(this->id,row_r_selection);

		return true;
	}
	else
		return RelayMouseMessage(&Window::Msg_RightDown, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlTable::Msg_WheelUpUp(const MouseCoords& mc)
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

bool ctrlTable::Msg_WheelDownUp(const MouseCoords& mc)
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
 *  
 *
 *  @author OLiver
 */
bool ctrlTable::Msg_LeftUp(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_LeftUp, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlTable::Msg_MouseMove(const MouseCoords& mc)
{
	// ButtonMessages weiterleiten
	return RelayMouseMessage(&Window::Msg_MouseMove, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void ctrlTable::Msg_ScrollShow(const unsigned int ctrl_id, const bool visible)
{
	if(visible)
	{
		/// Scrollbar wird angezeigt
		// Breite der letzten Spalte entsprechend anpassen, wenn plötzlich ne Scrolleiste sich hier noch reindrängelt	
		// Aufteilen dieser Breite auf die einzelnen Spalten
		unsigned width_col_minus = unsigned(20/columns.size());
		// Rest, der nicht aufgeteilt wurde
		unsigned rest = unsigned(20%columns.size());
		
		for(unsigned i = 0; i < columns.size(); ++i)
		{
			if(GetCtrl<ctrlButton>(i+1)->GetWidth() > width_col_minus)

				GetCtrl<ctrlButton>(i+1)->SetWidth(GetCtrl<ctrlButton>(i+1)->GetWidth() - width_col_minus);
			else
				rest += width_col_minus;

		}


		// Rest einfach von letzter passender Spalte abziehen
		for(unsigned i = 0; i < columns.size(); ++i)
		{
			if(GetCtrl<ctrlButton>(unsigned(columns.size())-i-1 + 1)->GetWidth() > rest)
			{
				GetCtrl<ctrlButton>(unsigned(columns.size())-i-1 + 1)
					->SetWidth(GetCtrl<ctrlButton>(unsigned(columns.size())-i-1 + 1)->GetWidth() - rest);
				break;
			}
		}

		// X-Position der Buttons neu berechnen
		unsigned short x_pos = 0;
		for(unsigned i = 0;i<columns.size();++i)
		{
			GetCtrl<ctrlButton>(i+1)->Move(x_pos,0);
			x_pos += GetCtrl<ctrlButton>(i+1)->GetWidth();
		}
	}
	else
	{
		// Scrollbar wird nicht mehr angezeigt --> Breite und Position wieder zurücksetzen
		ResetButtonWidths();
	}
}


/// Setzt die Breite und Position der Buttons ohne Scrolleiste
void ctrlTable::ResetButtonWidths()
{
	// Scrollbar wird nicht mehr angezeigt --> Breite und Position wieder zurücksetzen
	unsigned short x_pos = 0;
	for(unsigned i = 0;i<columns.size();++i)
	{
		GetCtrl<ctrlButton>(i+1)->SetWidth(columns[i].width*width/1000);
		GetCtrl<ctrlButton>(i+1)->Move(x_pos,0);
		x_pos += GetCtrl<ctrlButton>(i+1)->GetWidth();
	}

	// Rest auf dem letzten aufschlagen
	for(unsigned i = 0; i < columns.size(); ++i)
	{
		if(columns.at(columns.size()-i-1).width)
		{	
			GetCtrl<ctrlButton>(unsigned(columns.size())-i-1 + 1)
				->SetWidth(GetCtrl<ctrlButton>(unsigned(columns.size())-i-1 + 1)->GetWidth() +
				(width-x_pos));
			break;
		}
	}
}

