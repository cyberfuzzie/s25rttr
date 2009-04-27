// $Id: MusicPlayer.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef MUSIC_PLAYER_H_
#define MUSIC_PLAYER_H_

#include "Singleton.h"
#include <vector>
#include <string>

class iwMusicPlayer;

/// Speichert die Daten über eine Playlist und verwaltet diese
class Playlist
{
protected:

	/// Anzahl an Wiederholungen
	unsigned repeats;
	/// Zufällige Wiedergabe?
	bool random_playback;
	/// Einzelne abzuspielende Titel
	std::vector<std::string> segments;

public:

	Playlist();

	/// Einstellungen in Datei speichern
	bool SaveAs(const std::string& filename, const bool must_new_file);
	/// Einstellungen laden
	bool Load(const std::string& filename);

	/// Füllt das iwMusicPlayer - Fenster mit den entsprechenden Werten
	void FillWindow(iwMusicPlayer * window) const;
	/// Liest die Werte aus dem iwMusicPlayer - Fenster
	void ReadValuesOfWindow(const iwMusicPlayer * const window);
};

/// Globaler Musikplayer bzw. eine abspielbare Playlist
class MusicPlayer : public Playlist, public Singleton<MusicPlayer>
{
private:

	/// Reihenfolge der abzuspielenden Titel
	std::vector<unsigned> play_order;
	/// Aktuelle Position
	unsigned play_position;
	/// Wiedergabe gestoppt?
	bool stopped;

	/// externes benutzerdefiniertes Musikstück (z.B. andere mp3)
	libsiedler2::ArchivInfo sng;

private:

	/// Spielt nächstes Stück ab
	void PlayNextSegment();

public:

	MusicPlayer();

	/// Startet Abspielvorgang
	void StartPlaying();
	/// Stoppt das gerade abgespielte
	void Stop(); 

	/// Musik wurde fertiggespielt
	void MusicFinished();

};


#endif
