// $Id: iwPostWindow.h 5853 2010-01-04 16:14:16Z FloSoft $
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

#ifndef WP_POSTOFFICE_H_
#define WP_POSTOFFICE_H_

#include "IngameWindow.h"
#include "GameWorld.h"
#include "PostMsg.h"

class iwPostWindow : public IngameWindow
{
public:
	iwPostWindow(GameWorldViewer& gwv);
  void Msg_PaintBefore();
  void Msg_ButtonClick(const unsigned int ctrl_id);

private:
  GameWorldViewer& gwv;
  ctrlImage *postImage;
  ctrlText *postMsgInfos;

  ctrlImageButton *gotoButton;
  ctrlImageButton *deleteButton;
  
  ctrlImageButton *acceptButton;
  ctrlImageButton *declineButton;

  unsigned currentMessage;

	/// Liefert Pointer auf die pos-te Nachricht zurück
  PostMsg *GetPostMsg(unsigned pos) const;

	/// Passt Steuerelemente an, setzt Einstellung für diverse Controls passend für die aktuelle PostMessage
  void DisplayPostMessage();

	/// Nachricht löschen
	void DeletePostMessage(PostMsg *pm);

	/// Setzt den Text mehrzeilig in das Postfenster
	void SetMessageText(const std::string& message);

  unsigned lastSize;

};

#endif
