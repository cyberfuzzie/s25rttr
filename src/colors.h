// $Id: colors.h 4929 2009-05-23 23:01:13Z Demophobie $
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
#ifndef COLORS_H_INCLUDED
#define COLORS_H_INCLUDED

#pragma once

/// Texturfarben
enum TextureColor
{
	TC_GREY = 0,
	TC_RED1,
	TC_GREEN1,
	TC_GREEN2,
	TC_RED2,
	TC_BRICKS
};

/// Farbanzahl
const unsigned int GAME_COLORS_COUNT = 11;
const unsigned int COLORS_COUNT = 13;

/// Farbkonstanten
const unsigned int COLORS[COLORS_COUNT] =
{
	0xFF0073FF, // Blau
	0xFFFF0000, // Rot
	0xFFFFFF00, // Gelb
	0xFF00FF00, // Grün
	0xFFFF00FF, // Magenta
	0xFF00FFFF, // Cyan,
	0xFF444444, // Schwarz
	0xFFFFFFFF, // Weiß
	0xFFFF8000, // Orange
	0xFF6C4337, // Braun
	0x25FFFFFF, // Durchsichtig
	0x40000000, // Schatten
	0xFFCCCCCC, // Grau
};

#define COLOR_BLUE		COLORS[0]
#define COLOR_RED		COLORS[1]
#define COLOR_YELLOW	COLORS[2]
#define COLOR_GREEN		COLORS[3]
#define COLOR_MAGENTA	COLORS[4]
#define COLOR_CYAN		COLORS[5]
#define COLOR_BLACK		COLORS[6]
#define COLOR_WHITE		COLORS[7]
#define COLOR_ORANGE	COLORS[8]
#define COLOR_BROWN		COLORS[9]
#define COLOR_SHADOW    COLORS[10]
#define COLOR_GREY		COLORS[11]

/// Liefert die transparente Farbkomponente zurück.
inline unsigned GetAlpha(const unsigned int color) { return (( color & 0xFF000000) >> 24); }
/// Liefert die rote Farbkomponente zurück.
inline unsigned GetRed(  const unsigned int color) { return (( color & 0x00FF0000) >> 16); }
/// Liefert die grüne Farbkomponente zurück.
inline unsigned GetGreen(const unsigned int color) { return (( color & 0x0000FF00) >> 8 ); }
/// Liefert die blaue Farbkomponente zurück.
inline unsigned GetBlue( const unsigned int color) { return (( color & 0x000000FF)      ); }

/// Setzt die transparente Farbkomponente.
inline unsigned SetAlpha(const unsigned int color, const unsigned int alpha) { return (( color & 0x00FFFFFF) | ( alpha << 24) ); }
/// Setzt die rote Farbkomponente.
inline unsigned SetRed(  const unsigned int color, const unsigned int red  ) { return (( color & 0xFF00FFFF) | ( red   << 16) ); }
/// Setzt die grüne Farbkomponente.
inline unsigned SetGreen(const unsigned int color, const unsigned int green) { return (( color & 0xFFFF00FF) | ( green << 8 ) ); }
/// Setzt die blaue Farbkomponente.
inline unsigned SetBlue( const unsigned int color, const unsigned int blue ) { return (( color & 0xFFFFFF00) | ( blue       ) ); }

/// Erzeugt eine fertige Farbe aus einzelnen Farbkomponenten
inline unsigned MakeColor(const unsigned int alpha, const unsigned int red, const unsigned int green, const unsigned int blue) 
{ 
	return ((alpha << 24) | (red << 16) | (green << 8) |  blue);
}

#endif // COLORS_H_INCLUDED
