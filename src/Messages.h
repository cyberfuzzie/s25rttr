// $Id: Messages.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
/**
 *  Nachrichten des Fenstersystems
 *
 *  MSG_* Fenster Nachrichten
 *  CNM_* Netzwerk Nachrichten
 *  TBL_* Tabellen Nachrichten
 *
 *  @author OLiver
 */
enum Msg
{
	// Client-Netzwerk-Messages ( die kommen VOM Client und werden an einen festgelegten Messagehandler gesendet )
	CNM_NEWPLAYER,
	CNM_PLAYERLEAVE,
	CNM_PSCHANGE,
	CNM_NATIONCHANGE,
	CNM_COLORCHANGE,
	CNM_TEAMCHANGE,
	CNM_PINGCHANGE,
	CNM_READYCHANGE,
	CNM_SWAPPLAYER,
	CNM_CHAT,
	CNM_GGSCHANGE,
	CNM_COS_WAITFORANSWER,
	CNM_COS_QUERYPW,
	CNM_COS_QUERYMAPNAME,
	CNM_COS_QUERYPLAYERLIST,
	CNM_COS_FINISHED,
	CNM_COS_REGISTERED,
	CNM_COS_LOGGEDIN,
	CNM_COS_SERVERCREATED,
	CNM_ERR_INCOMPLETEMESSAGE,
	CNM_ERR_SERVERFULL,
	CNM_ERR_WRONGPW,
	CNM_ERR_CONNECTIONLOST,
	CNM_ERR_INVALIDSERVERTYPE,
	CNM_ERR_WRONGMAP,
	CNM_ERR_WRONGVERSION,
	CNM_ERR_LOBBYFULL,
	CNM_ERROR
};

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

enum MsgboxButton
{
	MSB_OK,
	MSB_OKCANCEL,
	MSB_YESNO,
	MSB_YESNOCANCEL
};

enum MsgboxIcon
{
	MSB_QUESTIONGREEN =	72,
	MSB_EXCLAMATIONGREEN,
	MSB_QUESTIONRED,
	MSB_EXCLAMATIONRED
};

enum MsgboxResult
{
	MSR_OK = 0,
	MSR_CANCEL,
	MSR_YES,
	MSR_NO,
	MSR_NOTHING = 0xFFFFFFFF
};

#endif // !MESSAGES_H_INCLUDED
