// $Id: ctrlChat.cpp 6069 2010-02-22 18:56:35Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#include "ctrlChat.h"

#include "ctrlScrollBar.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/// Breite der Scrollbar
static const unsigned short SCROLLBAR_WIDTH = 20;
/// Einschubbreite für sekundäre Zeilen (die umgebrochen wurden)
static const unsigned short SECONDARY_PUGGING = 30;

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ctrlChat.
 *
 *  @param[in] parent Elternfenster
 *  @param[in] id     ID des Steuerelements
 *  @param[in] x      X-Position
 *  @param[in] y      Y-Position
 *  @param[in] width  Breite des Controls
 *  @param[in] height Höhe des Controls
 *  @param[in] tc     Hintergrundtextur
 *  @param[in] font   Schriftart
 *
 *  @author OLiver
 */
ctrlChat::ctrlChat(Window *parent,
				   unsigned int id,
				   unsigned short x,
				   unsigned short y,
				   unsigned short width,
				   unsigned short height,
				   TextureColor tc, 
				   glArchivItem_Font *font)
	: Window(x, y, id, parent), 
	width(width), height(height), tc(tc), font(font), time_color(0xFFFFFFFF)
{
	// Zeilen pro Seite festlegen errechnen
	page_size = (height - 4) / (font->getHeight() + 2);

	// Scrollbalken hinzufügen
	AddScrollBar(0, width - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, height, SCROLLBAR_WIDTH, tc, page_size);

	//// Erst einmal 128 Chatzeilen reservieren
	//ExtendMemory(128);

	// Breite der Klammern <> um die Spielernamen berechnen
	bracket1_size = font->getWidth("<",  1);
	bracket2_size = font->getWidth("> ", 2);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ctrlChat.
 *
 *  @author OLiver
 */
ctrlChat::~ctrlChat()
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeichnet das Chat-Control.
 *
 *  @author OLiver
 */
bool ctrlChat::Draw_()
{
	// Box malen
	Draw3D(GetX(), GetY(), width, height, tc, 2);

	// Scrolleiste zeichnen
	DrawControls();

	// Wieviele Linien anzeigen?
	unsigned show_lines = (page_size > unsigned(chat_lines.size()) ? unsigned(chat_lines.size()) : page_size);

	// Listeneinträge zeichnen
	unsigned int pos = GetCtrl<ctrlScrollBar>(0)->GetPos();
	for(unsigned int i = 0; i < show_lines; ++i)
	{
		// eine zweite oder n-nte Zeile?
		if(chat_lines[i+pos].secondary)
		{
			// dann etwas Platz lassen davor und den entsprechenden Text hinschreiben
			font->Draw(GetX()+2,GetY()+2+i*(font->getHeight()+2),chat_lines[i+pos].msg.c_str(),0,chat_lines[i+pos].msg_color);
		}
		else
		{
			// Breite von Zeitangabe und Spielername ausrechnen
			unsigned short time_width = (chat_lines[i+pos].time_string.length())?font->getWidth(chat_lines[i+pos].time_string):0;
			unsigned short player_width = (chat_lines[i+pos].player.length())?font->getWidth(chat_lines[i+pos].player):0;
			unsigned short x_position = GetX()+2, y_position = GetY()+2+i*(font->getHeight()+2);

			// Zeit, Spieler und danach Textnachricht
			if(time_width)
			{
				font->Draw(x_position,y_position,chat_lines[i+pos].time_string,0,time_color);
				x_position +=  time_width;
			}

			if(player_width)
			{
				// Klammer 1 (<)
				font->Draw(x_position,y_position,"<",0,chat_lines[i+pos].player_color);
				x_position += bracket1_size;
				// Spielername
				font->Draw(x_position,y_position,chat_lines[i+pos].player,0,chat_lines[i+pos].player_color);
				x_position += player_width;
				// Klammer 2 (>)
				font->Draw(x_position,y_position,"> ",0,chat_lines[i+pos].player_color);
				x_position += bracket2_size;
			}

			font->Draw(x_position,y_position,chat_lines[i+pos].msg,0,chat_lines[i+pos].msg_color);

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
void ctrlChat::AddMessage(const std::string& time_string,const std::string& player, const unsigned int player_color, const std::string& msg, const unsigned int msg_color)
{
	// Breite von Zeitstring und Spielername berechnen (falls vorhanden)
	unsigned short prefix_width = ( time_string.length() ? font->getWidth(time_string) : 0) + (player.length() ? (bracket1_size + bracket2_size + font->getWidth(player)) : 0 );

	// Reicht die Breite des Textfeldes noch nichtmal dafür aus?
	if(prefix_width > width - 2 - SCROLLBAR_WIDTH)
	{
		// dann können wir das gleich vergessen
		return;
	}

	glArchivItem_Font::WrapInfo wi;

	// Zeilen ggf. wrappen, falls der Platz nich reicht und die Zeilenanfanänge in wi speichern
	font->GetWrapInfo(msg, width - prefix_width - 2 - SCROLLBAR_WIDTH, width - 2 - SCROLLBAR_WIDTH, wi);

	// Message-Strings erzeugen aus den WrapInfo
	std::string * strings = new std::string[wi.positions.size()];

	wi.CreateSingleStrings(msg, strings);

	unsigned old_size = unsigned(chat_lines.size());
	chat_lines.resize(old_size+wi.positions.size());

	// Zeilen hinzufügen
	for(unsigned int i = 0; i < wi.positions.size(); ++i)
	{
		// Nur bei den ersten Zeilen müssen ja Zeit und Spielername mit angegeben werden
		if(!(chat_lines[old_size+i].secondary = (i ? true : false)))
		{
			chat_lines[old_size+i].time_string = time_string;
			chat_lines[old_size+i].player = player;
			chat_lines[old_size+i].player_color = player_color;

			LOG.lprintf("%s <", time_string.c_str());
			LOG.lcprintf(player_color, "%s", player.c_str());
			LOG.lprintf(">: ");
		}
		else
			LOG.lprintf("    ");

		chat_lines[old_size+i].msg = strings[i];
		chat_lines[old_size+i].msg_color = msg_color;

		LOG.lcprintf(msg_color, "%s\n", strings[i].c_str());
	}

	delete [] strings;

	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	// Scrollbar Bescheid sagen
	scrollbar->SetRange(unsigned(chat_lines.size()));

	// Waren wir am Ende? Dann mit runterscrollen 
	if(scrollbar->GetPos() == (unsigned(chat_lines.size()) - wi.positions.size()) - page_size)
		scrollbar->SetPos(scrollbar->GetPos() + wi.positions.size());
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 * 
 *  @author OLiver
 */
bool ctrlChat::Msg_MouseMove(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_MouseMove, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 * 
 *  @author OLiver
 */
bool ctrlChat::Msg_LeftDown(const MouseCoords& mc) 
{
	return RelayMouseMessage(&Window::Msg_LeftDown, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 * 
 *  @author OLiver
 */
bool ctrlChat::Msg_LeftUp(const MouseCoords& mc)
{
	return RelayMouseMessage(&Window::Msg_LeftUp, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author Divan
 */
bool ctrlChat::Msg_WheelUp(const MouseCoords& mc)
{
	// Forward to ScrollBar, if mouse over control
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - 2, height - 4))
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
bool ctrlChat::Msg_WheelDown(const MouseCoords& mc)
{
	// Forward to ScrollBar, if mouse over control
	ctrlScrollBar *scrollbar = GetCtrl<ctrlScrollBar>(0);

	if(Coll(mc.x, mc.y, GetX() + 2, GetY() + 2, width - 2, height - 4))
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

