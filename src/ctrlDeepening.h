// $Id: ctrlDeepening.h 4959 2009-05-26 16:17:23Z Demophobie $
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
#ifndef CTRLDEEPENING_H_INCLUDED
#define CTRLDEEPENING_H_INCLUDED

#pragma once

#include "ctrlText.h"

class ctrlDeepening : public ctrlText
{
public:
	ctrlDeepening(Window *parent, unsigned int id, unsigned short x, unsigned short y, unsigned short width, unsigned short height, TextureColor tc, const std::string& text, glArchivItem_Font *font, unsigned int color);

protected:
	virtual bool Draw_(void);

private:
	TextureColor tc;
	unsigned short width;
	unsigned short height;
};

/// Colored Deepening
class ctrlColorDeepening : public ctrlDeepening
{
public:
	ctrlColorDeepening(Window *parent, unsigned int id, unsigned short x, unsigned short y, unsigned short width, unsigned short height, TextureColor tc, unsigned int fillColor);

	inline void SetColor(const unsigned int fillColor) { this->fillColor = fillColor; 
	LOG.lprintf("%d\n", fillColor); }

protected:
	virtual void DrawContent() const;

protected:
	unsigned short width, height;
	unsigned int fillColor;
};

#endif // !CTRLDEEPENING_H_INCLUDED

