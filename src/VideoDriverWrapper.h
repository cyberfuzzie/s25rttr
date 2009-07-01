// $Id: VideoDriverWrapper.h 5155 2009-07-01 15:37:53Z FloSoft $
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
#ifndef VIDEODRIVERWRAPPER_H_INCLUDED
#define VIDEODRIVERWRAPPER_H_INCLUDED


#include "Singleton.h"
#include "ExtensionList.h"
#include "DriverWrapper.h"
#include "../driver/src/VideoDriver.h"

class VideoDriver;

///////////////////////////////////////////////////////////////////////////////
// DriverWrapper
class VideoDriverWrapper : public Singleton<VideoDriverWrapper>
{
public:
	/// Konstruktor von @p DriverWrapper
	VideoDriverWrapper();
	/// Destruktor von @p DriverWrapper
	~VideoDriverWrapper();

	/// Läd den Treiber
	bool LoadDriver(void);

	/// Erstellt das Fenster.
	bool CreateScreen(const unsigned short screen_width, const unsigned short screen_height, const bool fullscreen);
	/// Verändert Auflösung, Fenster/Fullscreen
	bool ResizeScreen(const unsigned short screen_width, const unsigned short screen_height, const bool fullscreen);
	// zerstört das Fenster.
	bool DestroyScreen();
	// räumt die Texturen auf
	void CleanUp();
	// erstellt eine Textur
	unsigned int GenerateTexture();
	// Swapped den Buffer
	bool SwapBuffers();
	// liefert den Mausstatus (sollte nur beim Zeichnen der Maus verwendet werden, für alles andere die Mausmessages
	// benutzen!!!)
	int GetMouseX();
	int GetMouseY();

	/// Listet verfügbare Videomodi auf
	void ListVideoModes(std::vector<VideoDriver::VideoMode>& video_modes) const;

	/// Gibt Pointer auf ein Fenster zurück (device-dependent!), HWND unter Windows
	void * GetWindowPointer() const;

	unsigned short GetScreenWidth() const  { return screen_width; }
	unsigned short GetScreenHeight() const  { return screen_height; }
	bool IsFullscreen() const { return fullscreen; }

	bool IsLeftDown();
	bool IsRightDown();
	// setzt den Mausstatus
	void SetMousePos(const int x, const int y);
	// Nachrichtenschleife
	bool Run();

	unsigned int GetTickCount();

private:
	// Viewpoint und Co initialisieren
	bool Initialize(const short width, const short height);

	// prüft ob eine Extension verfügbar ist
	bool hasExtension(const char *extension);

	// lädt eine Funktion aus den Extensions
	void *loadExtension(const char *extension);

	// Alle (im Programm benutzen) Extensions laden
	bool LoadAllExtensions();

private:

	DriverWrapper driver_wrapper;
	VideoDriver *videodriver;

	unsigned short screen_width;
	unsigned short screen_height;
	bool fullscreen;

	unsigned int texture_list[100000];
	unsigned int texture_pos;
};


#endif

