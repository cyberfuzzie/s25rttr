// $Id: Sound.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED


/// Basisklasse f�r einen Sound.
class Sound
{
public:
	/// Konstruktor von @p Sound.
	Sound();

	/// Destruktor von @p Sound.
	virtual ~Sound(void);

	void SetNr(int nr) { this->nr = nr; }
	int GetNr() { return nr; }

	void SetType(unsigned int type) { this->type = (SoundType)type; }
	unsigned int GetType() { return type; }

	bool IsMusic() { return (type == SD_MUSIC); }
	bool IsEffect() { return (type == SD_EFFECT); }

public:
	enum SoundType {
		SD_UNKNOWN = 0,
		SD_MUSIC,
		SD_EFFECT
	};

protected:

	int nr;
	SoundType type;
};

#endif // !SOUND_H_INCLUDED
