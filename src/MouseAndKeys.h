// $Id: MouseAndKeys.h 4933 2009-05-24 12:29:23Z OLiver $
//
// Copyright (c) 2005-2008 Settlers Freaks (sf-team at siedler25.org)
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
#ifndef MOUSEANDKEYS_H_INCLUDED
#define MOUSEANDKEYS_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
/**
 *  Mausstatusstruct
 *
 *  @author OLiver
 */
class MouseCoords
{ 
public:
	MouseCoords() : x(0), y(0), ldown(false), rdown(false) {}
	MouseCoords(int x, int y, bool ldown, bool rdown) : x(x), y(y), ldown(ldown), rdown(rdown) {}

public:
	int x;      /// x-Koordinate
	int y;      /// y-Koordinate
	bool ldown; /// Linke Maustaste gedrückt
	bool rdown; /// Rechte Maustaste gedrückt
};

///////////////////////////////////////////////////////////////////////////////
/**
 *  Keydown Spezial Tasten
 *
 *  @author OLiver
 */
enum KeyType
{
	KT_INVALID = 0,
	KT_SPACE,
	KT_RETURN,
	KT_LEFT,
	KT_UP,
	KT_RIGHT,
	KT_DOWN,
	KT_BACKSPACE,
	KT_SHIFT,
	KT_TAB,
	KT_F1, KT_F2, KT_F3, KT_F4, KT_F5, KT_F6, KT_F7, KT_F8, KT_F9, KT_F10, KT_F11, KT_F12,
	KT_END,  // Ende
	KT_HOME, // Pos1
	KT_ESCAPE,
	KT_CHAR = 0xFFFFFFFF
};

/// Tastaturevent-Struktur
struct KeyEvent
{
	KeyType kt;
	char c;
	bool ctrl,shift,alt;
};

#endif //!MOUSEANDKEYS_H_INCLUDED
