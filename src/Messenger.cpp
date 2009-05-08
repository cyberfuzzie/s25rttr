// $Id: Messenger.cpp 4834 2009-05-08 19:45:33Z FloSoft $
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
#include "Messenger.h"

#include "Loader.h"
#include "VideoDriverWrapper.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/// Chat-Destination-String, der entsprechend angezeigt wird
const std::string CD_STRINGS[4] = 
{
	"", "(All) ", "(Team) ", "(Enemies) "
};

/// Farbe für die einzelnen CDs
const unsigned CD_COLORS[4] =
{
	0, COLOR_WHITE, COLOR_GREEN, COLOR_RED
};

Messenger::~Messenger()
{
}

/// Zeit, die 
void Messenger::Draw()
{
	unsigned y = 100;

	for(list<Messenger::Msg>::iterator it = messages.begin(); it.valid(); ++it, y+=LargeFont->getHeight())
	{
		unsigned diff = VideoDriverWrapper::inst().GetTickCount() - it->starttime;
		if(diff > 20000)
		{
			messages.erase(&it);
			continue;
		}

		// Transparenz der Schrift ausrechnen, da sie am Ende ausgeblendet wird
		unsigned transparency = 0xFF;
		if(diff > 18000)
			transparency = (transparency - transparency*(diff-18000)/2000);


		// Auf Alphaposition verschieben (höchstes Byte)
		transparency=transparency<<24;

		std::string cd_str = (it->cd==CD_SYSTEM)?"":_(CD_STRINGS[it->cd]);

		
		LargeFont->Draw(20,y,it->author,0,(it->color_author&0x00FFFFFF)|transparency);
		LargeFont->Draw(20+LargeFont->getWidth(it->author,static_cast<unsigned>(it->author.length())),y,cd_str,0,(CD_COLORS[it->cd]&0x00FFFFFF)|transparency);
		LargeFont->Draw(20+LargeFont->getWidth(it->author,static_cast<unsigned>(it->author.length()))+
			+LargeFont->getWidth(cd_str,static_cast<unsigned>(cd_str.length())),y,
			it->msg,0,(it->color_msg&0x00FFFFFF)|transparency);
	}
}

/// colored author string for terminal output
void Messenger::PrintTermColor(const std::string& text, const unsigned c)
{
	std::string tmp;
	// On Linux, we insert escape-codes into the string. On Windows call system functions.
	#ifndef _WIN32
		// A switch statement doesn't work here because we compare against the array COLORS[] (although it's constant, it can't be dereferenced at compile time)
		if (c == COLOR_BLUE)
		  tmp += "\033[40m\033[1;34m";
		else if (c == COLOR_RED)
		  tmp += "\033[40m\033[1;31m";
		else if (c == COLOR_YELLOW)
		  tmp += "\033[40m\033[1;33m";
		else if (c == COLOR_GREEN)
		  tmp += "\033[40m\033[1;32m";
		else if (c == COLOR_MAGENTA)
		  tmp += "\033[40m\033[1;35m";
		else if (c == COLOR_CYAN)
		  tmp += "\033[40m\033[1;36m";
		else if (c == COLOR_BLACK)
		  tmp += "\033[47m\033[1;30m";
		else if (c == COLOR_WHITE)
		  tmp += "\033[40m\033[1;37m";
		else if (c == COLOR_ORANGE)
		  tmp += "\033[43m\033[1;30m";
		else if (c == COLOR_BROWN)
		  tmp += "\033[40m\033[33m";
		else tmp += "\033[0m";
	#endif
	tmp += text; 
	#ifndef _WIN32
		tmp += "\033[0m";
	#endif

	#ifdef _WIN32
		// obtain handle
		HANDLE hStdout; 
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 

		hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 		
		if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) 
		{
			WORD colorAttr = 0;
			if (c == COLOR_BLUE || c == COLOR_MAGENTA || c == COLOR_CYAN || c == COLOR_WHITE)
				colorAttr |= FOREGROUND_BLUE;
			if (c == COLOR_YELLOW || c == COLOR_GREEN || c == COLOR_CYAN || c == COLOR_WHITE || c == COLOR_BROWN)
				colorAttr |= FOREGROUND_GREEN;
			if (c == COLOR_RED || c == COLOR_YELLOW || c == COLOR_MAGENTA || c == COLOR_WHITE || c == COLOR_BROWN)
				colorAttr |= FOREGROUND_RED;
			if (c == COLOR_BLACK)
				colorAttr |= BACKGROUND_BLUE;
			if (c == COLOR_BLACK || c == COLOR_ORANGE)
				colorAttr |= BACKGROUND_GREEN;
			if (c == COLOR_BLACK || c == COLOR_ORANGE)
				colorAttr |= BACKGROUND_RED;

			// if color matches any but brown
			if (colorAttr != 0 && c != COLOR_BROWN)
				colorAttr |= FOREGROUND_INTENSITY;

			// if color didn't match any, make default gray
			if (colorAttr == 0)
				colorAttr = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

			SetConsoleTextAttribute(hStdout, colorAttr);
		}
	#endif 

	LOG.lprintf("%s",tmp.c_str());

	// restore white-on-black
	#ifdef _WIN32
		// Obtain handle
		if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) 
		{
			SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		}
	#endif 
}

void Messenger::AddMessage(const std::string& author, const unsigned color_author, const ChatDestination cd, const std::string& msg,const unsigned color_msg)
{
	// print colored message to the log/console	
	PrintTermColor(author, color_author);
	PrintTermColor(CD_STRINGS[cd], CD_COLORS[cd]);
	PrintTermColor(msg + "\n",0);

	glArchivItem_Font::WrapInfo wi;

	// in Zeilen aufteilen, damit alles auf den Bildschirm passt
	LargeFont->GetWrapInfo(msg.c_str(),VideoDriverWrapper::inst().GetScreenWidth()-60
		-LargeFont->getWidth(author.c_str())-((cd==CD_SYSTEM)?0:LargeFont->getWidth(_(CD_STRINGS[cd]))),
		VideoDriverWrapper::inst().GetScreenWidth()-60,wi);

	// Message-Strings erzeugen aus den WrapInfo
	std::string * strings = new std::string[wi.count];

	wi.CreateSingleStrings(msg.c_str(),strings);
	
	for(unsigned i = 0;i<wi.count;++i)
	{
		Messenger::Msg tmp;

		// Nur in erster Zeile den Autor und die ChatDest.!
		if(i == 0)
		{
			tmp.author = author;
			tmp.cd = cd;
		}
		else
			tmp.cd = CD_SYSTEM;

		tmp.msg = strings[i];

		tmp.width = LargeFont->getWidth(msg.c_str());
		if(i == 0)
			tmp.width+=LargeFont->getWidth(author.c_str());

		tmp.color_author = color_author;
		tmp.color_msg = color_msg;
		tmp.starttime = VideoDriverWrapper::inst().GetTickCount();

		messages.push_back(tmp);
	}

	delete [] strings;
}
