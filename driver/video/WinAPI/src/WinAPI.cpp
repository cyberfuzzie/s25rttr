// $Id: WinAPI.cpp 5195 2009-07-05 09:44:30Z FloSoft $
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
#include "WinAPI.h"

#include "../../../../win32/resource.h"
#include <VideoInterface.h>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Instanzierungsfunktion von @p VideoWinAPI.
 *
 *  @param[in] CallBack DriverCallback f�r R�ckmeldungen.
 *
 *  @return liefert eine Instanz des jeweiligen Treibers
 *
 *  @author FloSoft
 */
DRIVERDLLAPI VideoDriver *CreateVideoInstance(VideoDriverLoaderInterface * CallBack)
{
	return new VideoWinAPI(CallBack);
}

DRIVERDLLAPI const char * GetDriverName(void)
{
	return "(WinAPI) OpenGL via the glorious WinAPI";
}


///////////////////////////////////////////////////////////////////////////////
/** @class VideoWinAPI
 *
 *  Klasse f�r den WinAPI Videotreiber.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var VideoWinAPI::dm_prev
 *
 *  Bildschirmmodus.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var VideoWinAPI::screen
 *
 *  Fensterhandle.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var VideoWinAPI::screen_dc
 *
 *  Zeichenkontext des Fensters.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var VideoWinAPI::screen_rc
 *
 *  OpenGL-Kontext des Fensters.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeiger auf die aktuelle Instanz.
 *
 *  @author FloSoft
 */
static VideoWinAPI *pVideoWinAPI = NULL;

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Konstruktor von @p VideoWinAPI.
 *
 *  @param[in] CallBack DriverCallback f�r R�ckmeldungen.
 *
 *  @author FloSoft
 */
VideoWinAPI::VideoWinAPI(VideoDriverLoaderInterface * CallBack) : VideoDriver(CallBack), mouse_l(false), mouse_r(false), mouse_z(0)
{
	pVideoWinAPI = this;

	memset(&dm_prev, 0, sizeof(DEVMODE));
	dm_prev.dmSize = sizeof(DEVMODE);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Destruktor von @p VideoWinAPI.
 *
 *  @author FloSoft
 */
VideoWinAPI::~VideoWinAPI(void)
{
	pVideoWinAPI = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Auslesen des Treibernamens.
 *
 *  @return liefert den Treibernamen zur�ck
 *
 *  @author FloSoft
 */
const char *VideoWinAPI::GetName(void) const
{
	return GetDriverName();
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Treiberinitialisierungsfunktion.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool VideoWinAPI::Initialize(void)
{
	memset(&dm_prev, 0, sizeof(DEVMODE));
	dm_prev.dmSize = sizeof(DEVMODE);

	screen = NULL;
	screen_dc = NULL;
	screen_rc = NULL;

	if(CallBack != NULL)
		initialized = true;

	return initialized;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Treiberaufr�umfunktion.
 *
 *  @author FloSoft
 */
void VideoWinAPI::CleanUp(void)
{
	// Fenster zerst�ren
	DestroyScreen();

	memset(&dm_prev, 0, sizeof(DEVMODE));
	dm_prev.dmSize = sizeof(DEVMODE);

	// nun sind wir nicht mehr initalisiert
	initialized = false;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Erstellt das Fenster mit entsprechenden Werten.
 *
 *  @param[in] width      Breite des Fensters
 *  @param[in] height     H�he des Fensters
 *  @param[in] fullscreen Vollbildmodus ja oder nein
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @bug Hardwarecursor ist bei Fenstermodus sichtbar,
 *       Cursor deaktivieren ist fehlerhaft
 *
 *  @author FloSoft
 */
bool VideoWinAPI::CreateScreen(unsigned short width, unsigned short height, const bool fullscreen)
{
	char title[512];

	if(!initialized)
		return false;

	WNDCLASSA  wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WindowProc;
	wc.cbClsExtra	    = 0;
	wc.cbWndExtra	    = 0;
	wc.hInstance	    = GetModuleHandle(NULL);
	wc.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SYMBOL));
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= GetWindowTitle();

	// Fensterklasse registrieren
	if (!RegisterClassA(&wc))
		return false;

	DWORD dwExStyle	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle	= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;

	if(fullscreen)
	{
		dwExStyle	= WS_EX_APPWINDOW;
		dwStyle		= WS_POPUP;

		EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &dm_prev);
	}
	else
	{
		// Bei Fensteranwendung die Breiten und Hoehen der Fensterrahmen, Titelleiste draufaddieren
		width += 2*GetSystemMetrics(SM_CXFIXEDFRAME);
		height+= 2*GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION);
	}


	// Fenster erstellen
	screen = CreateWindowExA(dwExStyle, GetWindowTitle(), GetWindowTitle(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);

	if(screen == NULL)
		return false;

	SetClipboardViewer(screen);

	sprintf(title, "%s - v%s-%s", GetWindowTitle(), GetWindowVersion(), GetWindowRevision());
	SetWindowTextA(screen, title);
	SetWindowText(GetConsoleWindow(), title);

	// Pixelformat zuweisen
	GLuint PixelFormat;
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		8, // 8 Bit
		8, // red
		0,
		8, // green
		0,
		8, // blue
		0,
		8, // alpha
		0,
		0,
		0,
		0,
		0,
		0,
		32, // 32 Bit
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,
		0,
		0
	};

	screen_dc = GetDC(screen);
	if(screen_dc == NULL)
		return false;

	// Pixelformat auswaehlen
	PixelFormat = ChoosePixelFormat(screen_dc, &pfd);
	if(PixelFormat == 0)
		return false;

	// Pixelformat zuweisen
	if(!SetPixelFormat(screen_dc, PixelFormat, &pfd))
		return false;

	// Renderingkontext erstellen
	screen_rc = wglCreateContext(screen_dc);
	if(screen_rc == NULL)
		return false;

	// Renderingkontext aktivieren
	if(!wglMakeCurrent(screen_dc, screen_rc))
		return false;

	// Das Fenster anzeigen
	ShowWindow(screen, SW_SHOW);

	// Das Fenster in den Vordergrund rcken
	SetForegroundWindow(screen);

	// Dem Fenster den Eingabefokus geben
	SetFocus(screen);

	// Mauscursor ausblenden
	ShowCursor(FALSE);

	// Bei Fullscreen Aufloesung umstellen
	if(fullscreen)
	{
		// Aktuelle Framerate holen und die spaeter dann benutzen
		DEVMODE prev;
		EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &prev);

		DEVMODE dm;
		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_DISPLAYFREQUENCY | DM_PELSWIDTH | DM_PELSHEIGHT;
		dm.dmDisplayFrequency = prev.dmDisplayFrequency;
		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;

		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}

	this->fullscreen = fullscreen;

	// buggy
	//glfwDisable(GLFW_MOUSE_CURSOR);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Erstellt oder ver�ndert das Fenster mit entsprechenden Werten.
 *
 *  @param[in] width      Breite des Fensters
 *  @param[in] height     H�he des Fensters
 *  @param[in] fullscreen Vollbildmodus ja oder nein
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @todo Vollbildmodus ggf. wechseln
 *
 *  @author FloSoft
 */
bool VideoWinAPI::ResizeScreen(unsigned short width, unsigned short height, const bool fullscreen)
{
	if(!initialized)
		return false;

	if(this->fullscreen && !fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	ShowWindow(screen,SW_HIDE);

	// Fensterstyle ggf. �ndern
	SetWindowLongPtr(screen, GWL_STYLE, (fullscreen ? WS_POPUP : (WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION) ) );
	SetWindowLongPtr(screen, GWL_EXSTYLE, (fullscreen ? WS_EX_APPWINDOW : (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE) ) );
	SetWindowPos(screen, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	RECT pos = {
		(fullscreen ? 0 : GetSystemMetrics(SM_CXSCREEN) / 2 - (width) / 2),
		(fullscreen ? 0 : GetSystemMetrics(SM_CYSCREEN) / 2 - (height) / 2),
		(width) + (fullscreen ? 0 : 2 * GetSystemMetrics(SM_CXFIXEDFRAME)),
		(height) + (fullscreen ? 0 : 2 * GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION))
	};

	// Fenstergr��e �ndern
	SetWindowPos(screen, HWND_TOP, pos.left, pos.top, pos.right, pos.bottom, SWP_SHOWWINDOW|SWP_DRAWFRAME|SWP_FRAMECHANGED);

	// Bei Vollbild Aufl�sung umstellen
	if(fullscreen)
	{
		DEVMODE dm;
		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;

		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}

	this->fullscreen = fullscreen;

	ShowWindow(screen,SW_SHOW);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Schliesst das Fenster.
 *
 *  @author FloSoft
 */
void VideoWinAPI::DestroyScreen(void)
{
	// Fenster schliessen
	EndDialog(screen, 0);

	if(dm_prev.dmBitsPerPel != 0)
		ChangeDisplaySettings(&dm_prev, CDS_RESET);

	if(screen_rc)
	{
		if(!wglMakeCurrent(NULL, NULL))
			return;

		if(!wglDeleteContext(screen_rc))
			return;

		screen_rc = NULL;
	}

	if((screen_dc) && (!ReleaseDC(screen, screen_dc)))
		return;

	screen_dc = NULL;

	if((screen) && (!DestroyWindow(screen)))
		return;

	screen = NULL;

	UnregisterClassA(GetWindowTitle(), GetModuleHandle(NULL));

	fullscreen = false;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Wechselt die OpenGL-Puffer.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool VideoWinAPI::SwapBuffers(void)
{
	if(!screen_dc)
		return false;

	// Puffer wechseln
	::SwapBuffers(screen_dc);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Die Nachrichtenschleife.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool VideoWinAPI::MessageLoop(void)
{
	MSG msg;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Auslesen des TickCounts.
 *
 *  @return liefert den TickCount
 *
 *  @author FloSoft
 */
unsigned long VideoWinAPI::GetTickCount(void) const
{
	return ::GetTickCount();
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Holen einer Subfunktion.
 *
 *  @param[in] function Name der Funktion welche geholt werden soll.
 *
 *  @return Adresse der Funktion bei Erfolg, @p NULL bei Fehler
 *
 *  @author FloSoft
 */
void *VideoWinAPI::GetFunction(const char *function) const
{
	return (void*)wglGetProcAddress(function);
}

/// Listet verf�gbare Videomodi auf
void VideoWinAPI::ListVideoModes(std::vector<VideoMode>& video_modes) const
{
	DEVMODE dm;
	unsigned m=0;
	while(EnumDisplaySettings(NULL,m++,&dm))
	{
		// Pr�fen, ob es die Aufl�sung nicht schonmal gab (kann es noch mit unterschiedlichen Bit-Tiefen geben
		bool already_in_vector = false;
		for(size_t i = 0;i<video_modes.size();++i)
		{
			if(dm.dmPelsWidth == video_modes[i].width && dm.dmPelsHeight == video_modes[i].height)
			{
				already_in_vector = true;
				break;
			}
		}

		if(already_in_vector)
			continue;

		// Es gibt die Aufl�sung noch nicht --> hinzuf�gen
		VideoMode vm = { static_cast<unsigned short>(dm.dmPelsWidth), 
			static_cast<unsigned short>(dm.dmPelsHeight) };
		video_modes.push_back(vm); 
	}
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Setzen der Mauskoordinaten.
 *
 *  @param[in] x X-Koordinate
 *  @param[in] y Y-Koordinate
 *
 *  @author FloSoft
 */
void VideoWinAPI::SetMousePos(int x, int y)
{
	mouse_xy.x = x;
	mouse_xy.y = y;

	POINT p = {x, y};
	ClientToScreen(screen, &p);
	SetCursorPos(p.x, p.y);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Setzen der X-Koordinate der Maus.
 *
 *  @param[in] x X-Koordinate
 *
 *  @author FloSoft
 */
void VideoWinAPI::SetMousePosX(int x)
{
	SetMousePos(x, mouse_xy.y);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Setzen der Y-Koordinate der Maus.
 *
 *  @param[in] y Y-Koordinate
 *
 *  @author FloSoft
 */
void VideoWinAPI::SetMousePosY(int y)
{
	SetMousePos(mouse_xy.x, y);
}


///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Senden einer gedr�ckten Taste.
 *
 *  @param[in] c Tastencode
 *
 *  @author FloSoft
 */
void VideoWinAPI::OnWMChar(char c, bool disablepaste)
{
	// Keine Leerzeichen als Extra-Zeichen senden!
	if(c == ' ')
		return;

	if(c == 'V' || c == 'v' || c == 0x16)
	{
		short state = GetKeyState(VK_CONTROL);
		if( !disablepaste && (state & 0xFF00) != 0)
		{
			OnWMPaste();
			return;
		}
	}

	//char cc[2] = {c, 0};
	//AnsiToOem(cc, cc);
	
	KeyEvent ke = {KT_CHAR, c, 
		(GetKeyState(VK_CONTROL)&0xFF00)?true:false, 
		(GetKeyState(VK_SHIFT)&0xFF00)?true:false,
		(GetKeyState(VK_MENU)&0xFF00)?true:false};

	CallBack->Msg_KeyDown(ke);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Senden einer gedr�ckten Taste.
 *
 *  @param[in] c Tastencode
 *
 *  @author FloSoft
 */
void VideoWinAPI::OnWMKeyDown(unsigned char c)
{
	KeyEvent ke = {KT_INVALID, 0, false, false, false};

	switch(c)
	{
	case VK_RETURN: ke.kt = KT_RETURN; break;
	case VK_SPACE:  ke.kt = KT_SPACE; break;
	case VK_LEFT:   ke.kt = KT_LEFT; break;
	case VK_RIGHT:  ke.kt = KT_RIGHT; break;
	case VK_UP:     ke.kt = KT_UP; break;
	case VK_DOWN:   ke.kt = KT_DOWN; break;
	case VK_BACK:   ke.kt = KT_BACKSPACE; break;
	case VK_LSHIFT: ke.kt = KT_SHIFT; break;
	case VK_RSHIFT: ke.kt = KT_SHIFT; break;
	case VK_TAB:    ke.kt = KT_TAB; break;
	case VK_END:	ke.kt = KT_END; break;
	case VK_HOME:	ke.kt = KT_HOME; break;
	case VK_ESCAPE:	ke.kt = KT_ESCAPE; break;
	default:
		{
			// Die 12 F-Tasten
			if(c >= VK_F1 && c <= VK_F12)
				ke.kt = static_cast<KeyType>(KT_F1 + int(c)-VK_F1);
		} break;
	}

	if(GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL)) ke.ctrl = true;
	if(GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT)) ke.shift = true;
	if(GetAsyncKeyState(VK_LMENU) || GetAsyncKeyState(VK_RMENU)) ke.alt = true;

	if(ke.kt != KT_INVALID)
		pVideoWinAPI->CallBack->Msg_KeyDown(ke);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Funktion zum Pasten von Text aus dem Clipboard.
 *
 *  @author FloSoft
 */
void VideoWinAPI::OnWMPaste()
{
	if (!IsClipboardFormatAvailable(CF_TEXT)) 
		return;

	OpenClipboard(NULL);
	
	HANDLE hData = GetClipboardData(CF_TEXT);
	const char *pData = (const char *)GlobalLock(hData);

	while( pData && *pData )
		OnWMChar( *(pData++), true );

	GlobalUnlock(hData);
	CloseClipboard();
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Callbackfunktion der WinAPI.
 *
 *  @param[in] window Fensterhandle
 *  @param[in] msg    Fensternachricht
 *  @param[in] wParam Erster Nachrichtenparameter
 *  @param[in] wParam Zweiter Nachrichtenparameter
 *
 *  @author FloSoft
 */
LRESULT CALLBACK VideoWinAPI::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PASTE:
		{
			pVideoWinAPI->OnWMPaste();
		} break;
	case WM_CLOSE:
		{ 
			PostQuitMessage(0);
			return 0;
		} break;
	case WM_ACTIVATE:
		{
			switch(wParam)
			{
			default:
			case WA_ACTIVE:
				{
					ShowCursor(0);
				} break;
			case WA_INACTIVE:
				{
					ShowCursor(1);
				}break;
			}
		} break;
	case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
			case SC_KEYMENU: // F10-Fehler beheben -> will sonst Fenster verschieben, was das
				// das Zeichnen unterbindet
				return 0;
			}
		} break;
	case WM_MOUSEMOVE:
		{
			pVideoWinAPI->mouse_xy.x = LOWORD(lParam);
			pVideoWinAPI->mouse_xy.y = HIWORD(lParam);
			pVideoWinAPI->CallBack->Msg_MouseMove(pVideoWinAPI->mouse_xy);
		} break;
	case WM_LBUTTONDOWN:
		{
			pVideoWinAPI->mouse_l = true;
			pVideoWinAPI->mouse_xy.ldown = true;
			pVideoWinAPI->CallBack->Msg_LeftDown(pVideoWinAPI->mouse_xy);
		} break;
	case WM_LBUTTONUP:
		{
			pVideoWinAPI->mouse_l = false;
			pVideoWinAPI->mouse_xy.ldown = false;
			pVideoWinAPI->CallBack->Msg_LeftUp(pVideoWinAPI->mouse_xy);
		} break;
	case WM_RBUTTONDOWN:
		{
			pVideoWinAPI->mouse_r = true;
			pVideoWinAPI->mouse_xy.rdown = true;
			pVideoWinAPI->CallBack->Msg_RightDown(pVideoWinAPI->mouse_xy);
		} break;
	case WM_RBUTTONUP:
		{
			pVideoWinAPI->mouse_r = false;
			pVideoWinAPI->mouse_xy.rdown = false;
			pVideoWinAPI->CallBack->Msg_RightUp(pVideoWinAPI->mouse_xy);
		} break;
	case WM_MOUSEWHEEL:
		{
			// Obtain scrolling distance. For every multiple of WHEEL_DELTA, we have to fire an event, because we treat the wheel like two buttons. 
			// One wheel "step" usually produces a mouse_z  of +/- WHEEL_DELTA. But there may exist wheels without "steps" that result in lower values we have to cumulate.
			pVideoWinAPI->mouse_z += GET_WHEEL_DELTA_WPARAM(wParam);

			// We don't want to crash if there were even wheels that produce higher values...
			while (abs(pVideoWinAPI->mouse_z) >= WHEEL_DELTA)
			{
				if (pVideoWinAPI->mouse_z > 0) // Scrolled to top
				{
					pVideoWinAPI->mouse_z -= WHEEL_DELTA;
					pVideoWinAPI->CallBack->Msg_WheelUp(pVideoWinAPI->mouse_xy);
				}
				else // Scrolled to bottom
				{
					pVideoWinAPI->mouse_z += WHEEL_DELTA;
					pVideoWinAPI->CallBack->Msg_WheelDown(pVideoWinAPI->mouse_xy);
				}
			}
		} break;
	case WM_KEYDOWN:
		{
			pVideoWinAPI->OnWMKeyDown((unsigned char)wParam);
		} break;
	case WM_CHAR:
	case WM_SYSCHAR: // auch abfangen, wenn ALT etc. gedr�ckt wurde mit
		{
			pVideoWinAPI->OnWMChar((char)wParam);

		} return 0;
	}
	return DefWindowProc(window, msg, wParam, lParam);
}




/// Gibt Pointer auf ein Fenster zur�ck (device-dependent!), HWND unter Windows
void * VideoWinAPI::GetWindowPointer() const
{
	return (void*)this->screen;
}
