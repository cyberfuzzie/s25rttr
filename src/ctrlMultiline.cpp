// $Id: ctrlMultiline.cpp 4793 2009-05-04 15:37:10Z OLiver $
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
#include "ctrlMultiline.h"

#include "ctrlScrollBar.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __Line__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ctrlMultiline.
 *
 *  @author OLiver
 */
ctrlMultiline::ctrlMultiline(Window *parent,
							 unsigned int id,
							 unsigned short x,
							 unsigned short y,
							 unsigned short width,
							 unsigned short height,
							 TextureColor tc,
							 glArchivItem_Font *font,
							 unsigned int format)
	: ctrlRectangle(parent, id, x, y, width, height),
	tc(tc), font(font), format(format), lines_in_control((height-4) / font->getHeight()), draw_box(true)
{
	AddScrollBar(0, width - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, height, SCROLLBAR_WIDTH, tc, lines_in_control);
}


/////////////////////////////////////////////////////////////////////////////////
///**
// *  fügt einen mehrzeiligen Text hinzu.
// *
// *  @author FloSoft
// */
//void ctrlMultiline::AddText(const std::string& text, unsigned int color)
//{
//	size_t i;
//	size_t length = strlen(text);
//	char  *temp   = new char[length + 1];
//	char  *s      = temp;
//
//	// In temporaeren String kopieren, damit man diesen zerlegen kann
//	memcpy(temp, text, length);
//
//	// Zeilenweise einfuegen
//	for (i = 0; i < length; ++i) {
//		if(temp[i] == '\n') {
//			temp[i] = '\0';
//			AddString(s, color, false);
//			s = &temp[i + 1];
//		}
//	}
//	if (s < &temp[i]) AddString(s, color, false);

//	delete[] temp;
//}

///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt eine Zeile hinzu.
 *
 *  @author OLiver
 */
void ctrlMultiline::AddString(const std::string& str, unsigned int color, bool scroll)
{
	Line line = { str, color };
	lines.push_back(line);

	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);
	scrollbar->SetRange(unsigned(lines.size()));

	if (scroll && (scrollbar->GetPos() == (unsigned(lines.size()) - 1) - lines_in_control)) {
		scrollbar->SetPos(scrollbar->GetPos() + 1);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  zeichnet das Fenster.
 *
 *  @author OLiver
 */
bool ctrlMultiline::Draw_(void)
{
	if(draw_box)
		Draw3D(GetX(), GetY(), width, height, tc, 2);

	DrawControls();

	unsigned show_lines = std::min(lines_in_control,unsigned(lines.size())); 

	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	for(unsigned short i = 0; i < show_lines; ++i)
		font->Draw(GetX() + 2, GetY() + 2 + i * font->getHeight(), lines[i + scrollbar->GetPos()].str, format, lines[i + scrollbar->GetPos()].color);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlMultiline::Msg_LeftDown(const MouseCoords& mc) 
{
	return GetCtrl<Window>(0)->Msg_LeftDown(mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
bool ctrlMultiline::Msg_LeftUp(const MouseCoords& mc)
{
	return GetCtrl<Window>(0)->Msg_LeftUp(mc);
}
///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlMultiline::Msg_WheelUpUp(const MouseCoords& mc)
{
	// Forward to ScrollBar
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	//If mouse in list
	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - /*2*/2, height - 4))
	{
		// Simulate three Button Clicks
		scrollbar->Msg_ButtonClick(0);
		scrollbar->Msg_ButtonClick(0);
		scrollbar->Msg_ButtonClick(0);
		return true;
	}
	else
		return false;
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlMultiline::Msg_WheelDownUp(const MouseCoords& mc)
{
	// Forward to ScrollBar
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// If mouse in list
	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - /*2*/2, height - 4))
	{
		// Simulate three Button Clicks
		scrollbar->Msg_ButtonClick(1);
		scrollbar->Msg_ButtonClick(1);
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
bool ctrlMultiline::Msg_MouseMove(const MouseCoords& mc)
{
	return GetCtrl<Window>(0)->Msg_MouseMove(mc);
}
