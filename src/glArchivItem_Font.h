// $Id: glArchivItem_Font.h 4933 2009-05-24 12:29:23Z OLiver $
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
#ifndef GLARCHIVITEM_FONT_H_INCLUDED
#define GLARCHIVITEM_FONT_H_INCLUDED

#pragma once

/// Klasse für GL-Fontfiles.
class glArchivItem_Font : public libsiedler2::ArchivItem_Font
{
public:

	/// Konstruktor von @p glArchivItem_Font.
	glArchivItem_Font(void) : ArchivItem_Font() {};
	/// Kopierkonstruktor von @p glArchivItem_Font.
	glArchivItem_Font(const glArchivItem_Font *item) : ArchivItem_Font(item) {};

	/// Zeichnet einen Text.
	void Draw(short x, short y, const std::string& text, unsigned int format, unsigned int color = COLOR_WHITE, unsigned short length = 0, unsigned short max = 0xFFFF, const std::string& end = "...", unsigned short end_length = 0);
	/// prüft ob ein Buchstabe existiert.
	bool CharExist(unsigned char c) const;
	/// liefert die Länge einer Zeichenkette.
	unsigned short getWidth(const std::string& text, unsigned length = 0, unsigned short *max = NULL) const;
	/// liefert die Höhe des Textes ( entspricht @p getDy()+1 )
	unsigned short getHeight() const { return dy+1; }

	/// Gibt Infos, über die Unterbrechungspunkte in einem Text
	class WrapInfo 
	{ 
	public:
		/// Konstruktor von @p WrapInfo.
		WrapInfo();
		/// Erzeugt ein Arrays aus eigenständigen Strings aus den Unterbrechungsinfos.
		void CreateSingleStrings(const std::string& origin_text, std::string *dest_strings);

	public:
		/// Array von Positionen (max. 32), wo der Text umbrochen werden soll (jeweils der Anfang vom String)
		unsigned int positions[32];
		/// Anzahl der Positionen
		unsigned int count;
	};

	/// Gibt Infos, über die Unterbrechungspunkte in einem Text, versucht Wörter nicht zu trennen, tut dies aber, falls
	/// es unumgänglich ist (Wort länger als die Zeile)
	void GetWrapInfo(const std::string& text, const unsigned short primary_width, const unsigned short secondary_width, WrapInfo& wi);

	enum {
		DF_LEFT   = 0,
		DF_RIGHT  = 1,
		DF_CENTER = 2
	};

	enum {
		DF_TOP     = 0,
		DF_BOTTOM  = 4,
		DF_VCENTER = 8
	};
};

#endif // !GLARCHIVITEM_FONT_H_INCLUDED
