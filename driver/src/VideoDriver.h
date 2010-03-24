// $Id: VideoDriver.h 6176 2010-03-24 10:39:41Z FloSoft $
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
#ifndef VIDEODRIVER_H_INCLUDED
#define VIDEODRIVER_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Header
#include "Interface.h"
#include "../../src/VideoDriverLoaderInterface.h"
#include <vector>

/// Basisklasse f�r einen Videotreiber.
class VideoDriver
{
public:
	/// Konstruktor von @p Videotreiber.
	VideoDriver(VideoDriverLoaderInterface * CallBack);

	/// Destruktor von @p Videotreiber.
	virtual ~VideoDriver(void);

	/// Funktion zum Auslesen des Treibernamens.
	virtual const char *GetName(void) const;

	/// Treiberinitialisierungsfunktion.
	virtual bool Initialize(void);

	/// Treiberaufr�umfunktion.
	virtual void CleanUp(void);

	/// Erstellt das Fenster mit entsprechenden Werten.
	virtual bool CreateScreen(unsigned short width, unsigned short height, const bool fullscreen);

	/// Erstellt oder ver�ndert das Fenster mit entsprechenden Werten.
	virtual bool ResizeScreen(unsigned short width, unsigned short height, const bool fullscreen);

	/// Schliesst das Fenster.
	virtual void DestroyScreen(void);

	/// Wechselt die OpenGL-Puffer.
	virtual bool SwapBuffers(void);

	/// Die Nachrichtenschleife.
	virtual bool MessageLoop(void);

	/// Funktion zum Auslesen des TickCounts.
	virtual unsigned long GetTickCount(void) const;

	/// Funktion zum Holen einer Subfunktion.
	virtual void *GetFunction(const char *function) const;

	/// Listet verf�gbare Videomodi auf
	struct VideoMode {
		unsigned short width;
		unsigned short height; 

		bool operator==(const VideoMode& o)	{	return (width == o.width && height == o.height);	}
	};
	virtual void ListVideoModes(std::vector<VideoMode>& video_modes) const;

	/// Funktion zum Auslesen der Mauskoordinaten.
	virtual void GetMousePos(int &x, int &y) const;

	/// Funktion zum Setzen der Mauskoordinaten.
	virtual void SetMousePos(int x, int y);

	/// Funktion zum Auslesen der X-Koordinate der Maus.
	virtual int GetMousePosX() const;

	/// Funktion zum Setzen der X-Koordinate der Maus.
	virtual void SetMousePosX(int x);

	/// Funktion zum Auslesen der Y-Koordinate der Maus.
	virtual int GetMousePosY() const;

	/// Funktion zum Setzen der Y-Koordinate der Maus.
	virtual void SetMousePosY(int y);

	/// Funktion zum Auslesen ob die Linke Maustaste gedr�ckt ist.
	virtual bool GetMouseStateL(void) const;

	/// Funktion zum Auslesen ob die Rechte Maustaste gedr�ckt ist.
	virtual bool GetMouseStateR(void) const;
	
	/// Get state of the modifier keys
	virtual KeyEvent GetModKeyState(void) const = 0;// { const KeyEvent ke = {KT_INVALID,0,false,false,false}; return ke; }

	/// Gibt Pointer auf ein Fenster zur�ck (device-dependent!), HWND unter Windows
	virtual void * GetWindowPointer() const = 0;

	/// pr�ft auf Initialisierung.
	bool IsInitialized() { return initialized; }

protected:
	VideoDriverLoaderInterface * CallBack; ///< Das DriverCallback f�r R�ckmeldungen.
	bool initialized;         ///< Initialisierungsstatus.
	MouseCoords mouse_xy;     ///< Status der Maus.
    bool keyboard[512];       ///< Status der Tastatur;
	bool fullscreen;          ///< Vollbild an/aus?
};

#endif // !VIDEODRIVER_H_INCLUDED
