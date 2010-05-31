// $Id: MIDI_Track.h 6460 2010-05-31 11:42:38Z FloSoft $
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
#ifndef MIDI_TRACK_H_INCLUDED
#define MIDI_TRACK_H_INCLUDED

#pragma once

class XMIDI_Track;

class MIDI_Track
{
public:
	MIDI_Track();
	MIDI_Track(const MIDI_Track *item);
	~MIDI_Track();
	void copy(const MIDI_Track *item);

	void allocXMid(unsigned int length);
	int readXMid(FILE *file);
	void clearXMid();

	void allocMid(unsigned int length);
	int readMid(FILE *file);
	void clearMid();

	int XMid2Mid();
	unsigned char *getMid(bool withheader = false) const;
	unsigned int getMidLength(bool withheader = false) const;

	friend class XMIDI_Track;

protected:
	unsigned int xmid_length;
	unsigned char *xmid_data;
	unsigned int mid_length;
	unsigned char *mid_data;
};

#endif // !MIDI_TRACK_H_INCLUDED
