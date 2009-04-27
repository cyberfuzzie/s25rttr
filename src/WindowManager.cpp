// $Id: WindowManager.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "WindowManager.h"

#include "Settings.h"
#include "VideoDriverWrapper.h"
#include "Loader.h"

#include "Window.h"
#include "Desktop.h"
#include "IngameWindow.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p WindowManager.
 *
 *  @author OLiver
 */
WindowManager::WindowManager(void)
	: desktop(NULL), nextdesktop(NULL), nextdesktop_data(NULL), disable_mouse(false), mc(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p WindowManager.
 *
 *  @author OLiver
 */
WindowManager::~WindowManager(void)
{
	CleanUp();
}

void WindowManager::CleanUp()
{
	for(IngameWindowListIterator it = windows.begin(); it.valid(); ++it)
		delete (*it);
	windows.clear();

	if(desktop)
		delete desktop;
	desktop = NULL;
	if(nextdesktop)
		delete nextdesktop;
	nextdesktop = NULL;
	
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zeichenfunktion WindowManager-Klasse.
 *  Zeichnet Desktop und alle Fenster.
 *
 *  @author OLiver
 *  @author FloSoft
 */
void WindowManager::Draw(void)
{
	// ist ein neuer Desktop eingetragen? Wenn ja, wechseln
	if(nextdesktop)
		Switch();

	// haben wir einen gültigen Desktop?
	if(desktop)
	{
		// ja, Msg_PaintBefore aufrufen
		desktop->Msg_PaintBefore();

		// und Desktop zeichnen
		desktop->Draw();

		// haben wir Fenster?
		if(windows.size())
		{
			for(IngameWindowListIterator it = windows.begin(); it.valid(); ++it)
			{
				// ist das Fenster hinter dem Iterator nicht NULL?
				if( (*it) == NULL)
				{
					// nein, also mit dem Nächsten weitermachen
					continue;
				}

				// Soll Fenster geschlossen werden?
				if((*it)->ShouldBeClosed())
				{
					// Fenster schliessen
					Close(*it);

					// und raus (korruption der liste verhindern)
					break;
				}

				// Msg_PaintBefore aufrufen
				(*it)->Msg_PaintBefore();

				// Fenster zeichnen
				(*it)->Draw();

				// wurde es minimiert?
				if((*it)->GetMinimized() == false)
				{
					// nein, Msg_PaintAfter aufrufen
					(*it)->Msg_PaintAfter();
				}
			}
		}

		DrawToolTip();

		// Msg_PaintAfter aufrufen
		desktop->Msg_PaintAfter();
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert ob der aktuelle Desktop den Focus besitzt oder nicht.
 *
 *	@return liefert @p true bei aktivem Desktop, 
 *                  @p false wenn der Desktop nicht den Fokus besitzt.
 *
 *  @author OLiver
 */
bool WindowManager::IsDesktopActive(void)
{ 
	if(desktop)
		return desktop->GetActive();

	return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schickt eine Nachricht an das aktive Fenster bzw den aktiven Desktop.
 *
 *  @param[in] msg   Nachricht welche geschickt werden soll
 *  @param[in] id    ID des Steuerelements
 *  @param[in] param Parameter der Nachricht
 *
 *  @author OLiver
 */

/// Sendet eine Tastaturnachricht an die Fenster.
void WindowManager::RelayKeyboardMessage(bool (Window::*msg)(const KeyEvent&),const KeyEvent& ke)
{
	// ist der Desktop gültig?
	if(desktop)
	{
		// ist der Desktop aktiv?
		if(desktop->GetActive())
		{
			// Ja, dann Nachricht an Desktop weiterleiten
			(desktop->*msg)(ke);
			desktop->RelayKeyboardMessage(msg,ke);
		}
		else
		{
			// Letztes Fenster schließen? (Escape oder Alt-W)
			if(ke.kt == KT_ESCAPE ||
				(ke.c == 'w' && ke.alt))
			{
				IngameWindowListIterator it(windows.end());
				Close(it);
			}
			else
			{
				// Nein, dann Nachricht an letztes Fenster weiterleiten (falls vorhanden)
				if( (*windows.end()) != NULL)
				{
					if(!((*windows.end())->*msg)(ke))
					{
						if(!(*windows.end())->RelayKeyboardMessage(msg,ke))
						{
							// Falls Nachrichten nicht behandelt wurden, an Desktop wieder senden
							(desktop->*msg)(ke);
							desktop->RelayKeyboardMessage(msg,ke);
						}
					}
				}
			}
		}
	}
}

/// Sendet eine Mausnachricht weiter an alle Fenster
void WindowManager::RelayMouseMessage(bool (Window::*msg)(const MouseCoords&),const MouseCoords& mc)
{
	// ist der Desktop gültig?
	if(desktop)
	{
		// ist der Desktop aktiv?
		if(desktop->GetActive())
		{
			// Ja, dann Nachricht an Desktop weiterleiten
			(desktop->*msg)(mc);
			desktop->RelayMouseMessage(msg,mc);
		}
		else
		{
			// Nein, dann Nachricht an letztes Fenster weiterleiten (falls vorhanden)
			if( (*windows.end()) != NULL)
			{
				((*windows.end())->*msg)(mc);
				(*windows.end())->RelayMouseMessage(msg,mc);
			}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
/**
 *  öffnet ein IngameWindow und fügt es zur Fensterliste hinzu.
 *
 *  @param[in] desktop       Pointer zum neuen Desktop, auf dem gewechselt werden soll
 *  @param[in] data          Daten für den neuen Desktop
 *  @param[in] disable_mouse Bei true wird bis zum nächsten Release die Maus deaktiviert (Switch-Anschließend-Drück-Bug)
 *
 *  @author OLiver
 */
void WindowManager::Show(IngameWindow *window, bool mouse)
{
	SetToolTip(NULL, "");

	// haben wir ein gültiges Fenster erhalten?
	if(!window)
		return;

	// haben wir einen gültigen Desktop?
	if(!desktop)
		return;

	// war das Fenster schon offen?
	for(IngameWindowListIterator it = windows.begin(); it.valid(); ++it)
	{
		// ist das Fenster gültig?
		if( (*it) == NULL)
			continue;

		// Evtl wird es schon geschlossen?
		if((*it)->ShouldBeClosed())
			continue;

		if(window->id == (*it)->id)
		{
			// Ja, also vorheriges schliessen
			(*it)->Close();
			delete window;
			return;
		}
	}

	// Fenster hinzufügen
	windows.push_back(window);

	// Desktop deaktivieren
	desktop->SetActive(false);

	// alle anderen Fenster deaktivieren
	for(IngameWindowListIterator it = windows.begin(); it.valid(); ++it)
	{
		// ist das Fenster gültig?
		if( (*it) == NULL)
			continue;

		(*it)->SetActive(false);
	}

	// Fenster aktivieren
	window->SetActive(true);


	// Maus deaktivieren, bis sie losgelassen wurde (Fix des Switch-Anschließend-Drück-Bugs)
	disable_mouse = mouse;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schliesst ein IngameWindow und entfernt es aus der Fensterliste.
 *
 *  @param[in] window Pointer auf das Fenster das geschlossen werden soll
 *
 *  @author OLiver
 */
void WindowManager::Close(IngameWindow *window)
{
	for(IngameWindowListIterator it = windows.begin(); it.valid(); ++it)
	{
		// ist das Fenster gültig?
		if( (*it) == NULL)
			continue;

		// ist es das Fenster das wir schliessen wollen?
		if( (*it) == window)
		{
			// ja, dann schliessen
			Close(it);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  merkt einen Desktop zum Wechsel vor.
 *
 *  @param[in] desktop       Pointer zum neuen Desktop, auf dem gewechselt werden soll
 *  @param[in] data          Daten für den neuen Desktop
 *  @param[in] disable_mouse Bei true wird bis zum nächsten Release die Maus deaktiviert (Switch-Anschließend-Drück-Bug)
 *
 *  @author OLiver
 */
void WindowManager::Switch(Desktop *desktop, void *data, bool mouse)
{
	nextdesktop = desktop;
	nextdesktop_data = data;
	disable_mouse = mouse;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Verarbeitung des Drückens der Linken Maustaste.
 *
 *  @param[in] mc Mauskoordinaten Struktur
 *
 *  @author OLiver
 */
void WindowManager::Msg_LeftDown(const MouseCoords& mc)
{
	// ist unser Desktop gültig?
	if(!desktop)
		return;

	// Sound abspielen
	GetSound(sound_lst,112)->Play(255,false);

	// haben wir überhaupt fenster?
	if(!windows.size())
	{
		// nein, dann Desktop aktivieren
		desktop->SetActive(true);

		// ist der Maus-Klick-Fix aktiv?
		if(disable_mouse == false)
		{
			// nein, Msg_LeftDown aufrufen
			desktop->Msg_LeftDown(mc);

			// und allen unten drunter auch Bescheid sagen
			desktop->RelayMouseMessage(&Window::Msg_LeftDown, mc);
		}

		// und raus
		return;
	}

	// haben wir ein zuletzt aktives Fenster? (sollten wir ja eigentlich, aber sicher ist sicher)
	if( (*windows.end()) != NULL)
	{
		// ist das zuletzt aktiv gewesene Fenster Modal?
		if( (*windows.end())->GetModal())
		{
			// ja es ist modal, ist der Maus-Klick-Fix aktiv?
			if(disable_mouse == false)
			{
				// nein, Msg_LeftDownaufrufen
				(*windows.end())->Msg_LeftDown(mc);

				// und allen unten drunter auch Bescheid sagen
				(*windows.end())->RelayMouseMessage(&Window::Msg_LeftDown, mc);

				// und noch MouseLeftDown vom Fenster aufrufen
				(*windows.end())->MouseLeftDown(mc);
			}

			// und raus
			return;
		}
	}

	bool found_window = false;

	// Fenster durchgehen ( von hinten nach vorn, da die vordersten ja zuerst geprüft werden müssen !! )
	for(IngameWindowListIterator it = windows.end(); it.valid(); --it)
	{
		// ist das Fenster gültig?
		if( (*it) == NULL)
			continue;

		// FensterRect für Kollisionsabfrage
		Rect window_rect = {
			(*it)->x,
			(*it)->y,
			(*it)->x + (*it)->GetWidth(),
			(*it)->y + (*it)->GetHeight()
		};

		// trifft die Maus auf ein Fenster?
		if(Coll(mc.x, mc.y, window_rect))
		{
			// ja, also aktives Fenster deaktivieren (falls ok)
			if( (*windows.end()) != NULL)
				(*windows.end())->SetActive(false);

			// Fenster aus der Liste holen und vorne wieder anhängen
			IngameWindow *tmp = *it;
			windows.erase(it); // ACHTUNG!!!!
			windows.push_back(tmp);
			
			// ist das neue Fenster ok?
			if( (*windows.end()) != NULL)
			{
				// ja, dann aktivieren
				(*windows.end())->SetActive(true);
			
				// ist der Maus-Klick-Fix aktiv?
				if(disable_mouse == false)
				{
					// nein, dann Msg_LeftDown aufrufen
					(*windows.end())->Msg_LeftDown(mc);

					// und allen unten drunter auch Bescheid sagen
					(*windows.end())->RelayMouseMessage(&Window::Msg_LeftDown, mc);

					// und noch MouseLeftDown vom Fenster aufrufen
					(*windows.end())->MouseLeftDown(mc);
				}
			}


			// wir haben eins gefunden --> der Desktop muss nicht aktiv werden
			found_window = true;
			
			// Desktop deaktivieren, falls aktiviert
			desktop->SetActive(false);
					
			// wir können mit der Schleife abbrechen
			break;
		}
	}

	// Haben wir ein Fenster gefunden gehabt?
	if(!found_window)
	{
		// letztes Fenster deaktivieren (falls ok), da ja nun der Desktop aktiv werden soll
		if( (*windows.end()) != NULL)
			(*windows.end())->SetActive(false);

		// Desktop aktivieren
		desktop->SetActive(true);
		
		// ist der Maus-Klick-Fix aktiv?
		if(disable_mouse == false)
		{
			// nein, dann Msg_LeftDown aufrufen
			desktop->Msg_LeftDown(mc);

			// und allen unten drunter auch Bescheid sagen
			desktop->RelayMouseMessage(&Window::Msg_LeftDown, mc);;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Verarbeitung des Loslassens der Linken Maustaste.
 *
 *  @param[in] mc Mauskoordinaten Struktur
 *
 *  @author OLiver
 */
void WindowManager::Msg_LeftUp(const MouseCoords& mc)
{
	// ist unser Desktop gültig?
	if(!desktop)
		return;

	// ist der Maus-Klick-Fix aktiv?
	if(disable_mouse == false)
	{
		// ist der Desktop aktiv?
		if(desktop->GetActive())
		{
			// ja, dann Msg_LeftUp aufrufen
			desktop->Msg_LeftUp(mc);

			// und die Fenster darunter auch
			desktop->RelayMouseMessage(&Window::Msg_LeftUp, mc);
		}
		else
		{
			// nein, ist das Fenster ok?
			if( (*windows.end()) != NULL)
			{
				// ja, dann Msg_LeftUp aufrufen
				(*windows.end())->Msg_LeftUp(mc);

				// und den anderen Fenstern auch Bescheid geben
				(*windows.end())->RelayMouseMessage(&Window::Msg_LeftUp, mc);

				// und noch MouseLeftUp vom Fenster aufrufen
				(*windows.end())->MouseLeftUp(mc);
			}
		}
	}

	// Maus-Klick-Fix deaktivieren
	disable_mouse = false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Verarbeitung des Drückens der Rechten Maustaste.
 *
 *  @param[in] mc Mauskoordinaten Struktur
 *
 *  @author OLiver
 */
void WindowManager::Msg_RightDown(const MouseCoords& mc)
{
	// ist unser Desktop gültig?
	if(!desktop)
		return;

	// ist der Desktop aktiv?
	if(desktop->GetActive())
	{
		// ja, dann Msg_RightDown aufrufen
		desktop->Msg_RightDown(mc);
		
		// und die Fenster darunter auch
		desktop->RelayMouseMessage(&Window::Msg_RightDown, mc);
	}
	else
	{
		// nein, ist das Fenster ok?
		if( (*windows.end()) != NULL)
		{
			// ja, dann Nachricht an Fenster weiterleiten
			(*windows.end())->RelayMouseMessage(&Window::Msg_RightDown, mc);
		}
	}

	// Sind Fenster vorhanden?
	if(windows.size())
	{
		// ist das aktive Fenster ok
		if( (*windows.end()) != NULL)
		{
			// ist das Fenster modal? wenn ja, dann raus
			if((*windows.end())->GetModal())
				return;

			// ja, dann prüfen ob Fenster geschlossen werden muss
			for(IngameWindowListIterator it = windows.end(); it.valid(); --it)
			{
				// ist das Fenster gültig?
				if( (*it) == NULL)
					continue;

				// ja, haben wir einen Treffer?
				if(Coll(mc.x, mc.y, (*it)->GetX(), (*it)->GetY(), (*it)->GetWidth(), (*it)->GetHeight()))
				{
					// ja, dann schliessen
					(*it)->Close();

					// und raus
					return;
				}
			}
		}
	}

	// letztes Fenster deaktivieren (falls ok), da ja nun der Desktop aktiv werden soll
	if(windows.size())
		(*windows.end())->SetActive(false);

	// Desktop aktivieren
	desktop->SetActive(true);

	// ja, dann Msg_RightDown aufrufen
	desktop->Msg_RightDown(mc);
	
	// und die Fenster darunter auch
	desktop->RelayMouseMessage(&Window::Msg_RightDown, mc);
}

void WindowManager::Msg_RightUp(const MouseCoords& mc)
{
	RelayMouseMessage(&Window::Msg_RightUp,mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Verarbeitung des Verschiebens der Maus.
 *
 *  @param[in] mc Mauskoordinaten Struktur
 *
 *  @author OLiver
 */
void WindowManager::Msg_MouseMove(const MouseCoords& mc)
{
	this->mc = &mc;

	// ist unser Desktop gültig?
	if(!desktop)
		return;

	// ist der Maus-Klick-Fix aktiv?
	if(disable_mouse == false)
	{
		// nein, ist unser Desktop aktiv?
		if(desktop->GetActive())
		{
			// ja, dann Msg_MouseMove aufrufen
			desktop->Msg_MouseMove(mc);

			// und alles drunter auch benachrichtigen
			desktop->RelayMouseMessage(&Window::Msg_MouseMove, mc);
		}
		else
		{
			// nein, ist das Fenster ok?
			if( (*windows.end()) != NULL)
			{
				// ja, dann Msg_MouseMove aufrufen
				(*windows.end())->Msg_MouseMove(mc);

				// und alles drunter auch benachrichtigen
				(*windows.end())->RelayMouseMessage(&Window::Msg_MouseMove, mc);

				// und MouseMove vom Fenster aufrufen
				(*windows.end())->MouseMove(mc);
			}
		}
	}
}

void WindowManager::Msg_KeyDown(const KeyEvent& ke)
{
	RelayKeyboardMessage(&Window::Msg_KeyDown,ke);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schliesst ein IngameWindow und entfernt es aus der Fensterliste.
 *
 *  @param[in] it Iterator auf das Fenster in der Fensterliste
 *
 *  @author OLiver
 */
void WindowManager::Close(IngameWindowListIterator &it)
{
	SetToolTip(NULL, "");

	// ist das Fenster gültig?
	if( (*it) == NULL)
		return;

	// War es an vorderster Stelle?
	if(it == windows.end())
	{
		// haben wir noch Fenster zum aktivieren?
		if(windows.size() - 1)
		{
			// ja, also das nächste aktivieren
			IngameWindowListIterator tmp = it;
			--tmp;

			// ist das Fenster auch gültig?
			if( (*tmp) != NULL)
			{
				// ja, Fenster aktivieren
				(*tmp)->SetActive(true);
			}
			else
			{
				// nein, also Desktop aktivieren
				desktop->SetActive(true);
			}
		}
		else
		{
			// nein, also Desktop aktivieren
			desktop->SetActive(true);
		}

	}

	// Fenster löschen
	delete (*it);

	// und aus der Liste entfernen
	windows.erase(&it);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Sucht ein Fenster mit der entsprechenden Fenster-ID 
 *  und schließt es (falls es so eins gibt)
 *
 *  @param[in] id ID des/der Fenster(s) welche(s) geschlossen werden soll
 *
 *  @author OLiver
 */
void WindowManager::Close(unsigned int id)
{
	for(list<IngameWindow*>::iterator it = windows.begin();it.valid();++it)
	{
		if((*it)->id == id)
			Close(it);
	}
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  wechselt den Desktop in den neuen Desktop
 *
 *  @author OLiver
 */
void WindowManager::Switch(void)
{
	// einmal richtig clearen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetToolTip(NULL, "");

	// haben wir einen aktuell gültigen Desktop?
	if(desktop)
	{
		// Alle (alten) Fenster zumachen
		for(IngameWindowListIterator it = windows.begin(); it.valid(); ++it)
		{
			delete (*it);
		}
		windows.clear();
		
		// Desktop löschen
		delete desktop;
	}

	// Desktop auf Neuen umstellen
	desktop = nextdesktop;

	// ist der neue Desktop gültig?
	if(desktop)
	{
		// Desktop aktivieren
		desktop->SetActive(true);

		// aufräumen
		nextdesktop = NULL;
		nextdesktop_data = NULL;
	}
}

void WindowManager::SetToolTip(Window *ttw, const std::string& tooltip)
{
	static Window *lttw = NULL;

	if(tooltip.length() == 0 && !ttw)
		this->tooltip = "";

	if(tooltip.length() == 0 && lttw == ttw)
		this->tooltip = "";
	if(tooltip.length())
	{
		this->tooltip = tooltip;
		lttw = ttw;
	}
}

void WindowManager::DrawToolTip()
{
	// Tooltip zeichnen
	if(tooltip.length() && mc)
	{
		Window::DrawRectangle(mc->x + 30 - 2, mc->y-2, NormalFont->getWidth(tooltip.c_str())+4, NormalFont->getDy()+4, 0x9F000000);
		NormalFont->Draw(mc->x + 30, mc->y + NormalFont->getDy() / 2, tooltip.c_str(), glArchivItem_Font::DF_VCENTER, COLOR_YELLOW);
	}
}
