// $Id: iwTools.h 4784 2009-05-02 20:43:44Z OLiver $
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
#ifndef iwTOOLS_H_INCLUDED
#define iwTOOLS_H_INCLUDED

#pragma once

#include "IngameWindow.h"

/// Fenster mit den Militäreinstellungen.
class iwTools : public IngameWindow
{
	/// Einstellungen nach dem letzten Netzwerk-Versenden nochmal verändert?
	bool settings_changed;

public:

	iwTools();
	~iwTools();

private:

	/// Updatet die Steuerelemente mit den aktuellen Einstellungen aus dem Spiel
	void UpdateSettings();
	/// Sendet veränderte Einstellungen (an den Client), falls sie verändert wurden
	void TransmitSettings();

	void Msg_ButtonClick(const unsigned int ctrl_id);
	void Msg_ProgressChange(const unsigned int ctrl_id, const unsigned short position);
	void Msg_Timer(const unsigned int ctrl_id);
};

#endif // !iwTOOLS_H_INCLUDED
