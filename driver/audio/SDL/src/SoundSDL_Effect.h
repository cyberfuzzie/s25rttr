// $Id: SoundSDL_Effect.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef SOUNDSDL_EFFECT_H_INCLUDED
#define SOUNDSDL_EFFECT_H_INCLUDED

#include <Sound.h>

class AudioSDL;

class SoundSDL_Effect : public Sound
{
	friend class AudioSDL;

public:
	/// Konstruktor von @p SoundSDL_Effect.
	SoundSDL_Effect();

	/// Destruktor von @p SoundSDL_Effect.
	~SoundSDL_Effect(void);


private:
	Mix_Chunk *sound; ///< Das PCM-Handle.
};

#endif // !SOUNDSDL_EFFECT_H_INCLUDED
