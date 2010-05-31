// $Id: iwAIDebug.h 6458 2010-05-31 11:38:51Z FloSoft $
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

#ifndef iwAIDEBUG_H_INCLUDED
#define iwAIDEBUG_H_INCLUDED

#include "iwAIDebug.h"
#include "IngameWindow.h"

class AIPlayerJH;
class ctrlList;
class ctrlText;
class GameWorldViewer;

class iwAIDebug : public IngameWindow
{
public:
	iwAIDebug(GameWorldViewer * const gwv);

private:
	void Msg_ComboSelectItem(const unsigned int ctrl_id, const unsigned short select);
	//void Msg_ButtonClick(const unsigned int ctrl_id);
	//void Msg_ProgressChange(const unsigned int ctrl_id, const unsigned short position);
	void Msg_PaintBefore();

private:
	std::vector<AIPlayerJH *> ais;
	ctrlList *jobs;
	unsigned selection;
	unsigned overlay;
	ctrlText *text;
	GameWorldViewer *gwv;
};

#endif
