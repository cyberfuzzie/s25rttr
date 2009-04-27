// $Id: MusicPlayer.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "MusicPlayer.h"
#include "iwMusicPlayer.h"
#include "AudioDriverWrapper.h"
#include <fstream>
#include <algorithm>

Playlist::Playlist() : repeats(1), random_playback(false)
{
}

/// Einstellungen in Datei speichern
bool Playlist::SaveAs(const std::string& filename, const bool must_new_file)
{
	// Existiert Datei auch noch nicht?
	std::ifstream in(filename.c_str());

	if(in.good() && must_new_file)
	{
		// Datei darf noch nicht existieren!
		in.close();
		return false;
	}

	std::ofstream out(filename.c_str());

	if(!out.good())
		return false;

	out << repeats << " ";
	out << ((random_playback) ? ("random_playback") : ("in_order")) << std::endl;

	for(unsigned i = 0;i<segments.size();++i)
		out << segments[i] << "\n";

	out.close();

	return true;
}

/// Einstellungen laden
bool Playlist::Load(const std::string& filename)
{
	std::ifstream in(filename.c_str());

	if(!in.good())
		return false;

	std::string random_playback_str;

	in >> repeats;
	in >> random_playback_str;

	if(random_playback_str == "random_playback")
		random_playback = true;
	else if(random_playback_str == "in_order")
		random_playback = false;
	else
		return false;

	// Liste leeren von evtl vorherigen Stücken
	segments.clear();

	// Zeile zuende lesen
	char tmp[512];
	in.getline(tmp,512);

	while(true)
	{
		in.getline(tmp,512);

		// Carriage Returns aus Windows-Dateien ggf. rausfiltern
		size_t length = strlen(tmp);
		if(length)
		{
			if(tmp[length-1] == '\r')
				tmp[length-1] = 0;
		}

		if(strlen(tmp) == 0 || in.eof())
		{
			in.close();
			return true;
		}

		segments.push_back(tmp);
	}

	return false;
}

void Playlist::FillWindow(iwMusicPlayer * window) const
{
	window->SetSegments(segments);
	window->SetRepeats(repeats);
	window->SetRandomPlayback(random_playback);
}

/// Liest die Werte aus dem iwMusicPlayer - Fenster
void Playlist::ReadValuesOfWindow(const iwMusicPlayer * const window)
{
	repeats = window->GetRepeats();
	random_playback = window->GetRandomPlayback();
	window->GetSegments(segments);
}

/////////////////////////////////////////////////////////////////////////////

MusicPlayer::MusicPlayer() : play_position(0), stopped(false)
{
}

/// Startet Abspielvorgang
void MusicPlayer::StartPlaying()
{
	play_position = 0;
	stopped = false;

	if(segments.size())
	{
		// Abspielreihenfolge erstmal normal festlegen
		play_order.resize(segments.size());
		for(unsigned i = 0;i<segments.size();++i)
			play_order[i] = i;

		// Bei Zufall nochmal mischen
		if(random_playback)
			std::random_shuffle(play_order.begin(),play_order.end());

		PlayNextSegment();
	}
}

/// Stoppt das gerade abgespielte
void MusicPlayer::Stop()
{
	AudioDriverWrapper::inst().StopMusic();
	stopped = true;
}



void MusicPlayer::MusicFinished()
{
	if(!stopped)
	{
		if(++play_position >= segments.size())
			play_position = 0;
		PlayNextSegment();
	}
}

/// Spielt nächstes Stück ab
void MusicPlayer::PlayNextSegment()
{
	std::string to_play = segments[play_order[play_position]];

	// Evtl ein Siedlerstück ("sNN")?
	if(to_play.length() == 3)
	{
		unsigned nr;
		if((nr = atoi(to_play.substr(1).c_str())) <= 14)
		{
			// Siedlerstück abspielen (falls es geladen wurde)
			if(GetMusic(sng_lst, nr-1))
				GetMusic(sng_lst, nr-1)->Play(repeats);
		}
	}
	else
	{
		// anderes benutzerdefiniertes Stück abspielen
		// in "sng" speichern, daher evtl. altes Stück erstmal löschen
		sng.clear();
		LOG.lprintf("lade \"%s\": ", to_play.c_str());
		// Neues Stück laden
		if(libsiedler2::loader::LoadSND(to_play.c_str(), &sng) != 0 )
			Stop();

		// Und abspielen
		dynamic_cast<glArchivItem_Music*>(sng.get(0))->Play(repeats);


	}
}
