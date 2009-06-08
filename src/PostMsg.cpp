// $Id: PostMsg.cpp jh $
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

#include "PostMsg.h"
#include "GameClient.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

PostMsg::PostMsg(const std::string& text, PostMessageCategory cat)
: text(text), type(PMT_NORMAL), cat(cat), sendFrame(GAMECLIENT.GetGFNumber()) { }

PostMsg::PostMsg(SerializedGameData *sgd)
: text(sgd->PopString()), type(PostMessageType(sgd->PopUnsignedInt())), 
cat(PostMessageCategory(sgd->PopUnsignedInt())), sendFrame(sgd->PopUnsignedInt()) { }


PostMsg::~PostMsg() { }

void PostMsg::Serialize(SerializedGameData *sgd)
{
  sgd->PushString(text);
  sgd->PushUnsignedInt(type);
  sgd->PushUnsignedInt(cat);
  sgd->PushUnsignedInt(sendFrame);
}

PostMsgWithLocation::PostMsgWithLocation(const std::string& text, PostMessageCategory cat, MapCoord x, MapCoord y)
: PostMsg(text, cat), x(x), y(y) { type = PMT_WITH_LOCATION; }

PostMsgWithLocation::PostMsgWithLocation(SerializedGameData *sgd)
: PostMsg(sgd), x(sgd->PopUnsignedShort()), y(sgd->PopUnsignedShort()) { }

void PostMsgWithLocation::Serialize(SerializedGameData *sgd)
{
  PostMsg::Serialize(sgd);
  sgd->PushUnsignedShort(x);
  sgd->PushUnsignedShort(y);
}

ImagePostMsgWithLocation::ImagePostMsgWithLocation(const std::string& text, PostMessageCategory cat, 
                                                   MapCoord x, MapCoord y, BuildingType senderBuilding, Nation senderNation)
: PostMsgWithLocation(text, cat, x, y), senderBuilding(senderBuilding), senderNation(senderNation) 
{ 
  type = PMT_IMAGE_WITH_LOCATION;
}

ImagePostMsgWithLocation::ImagePostMsgWithLocation(SerializedGameData *sgd)
: PostMsgWithLocation(sgd), senderBuilding(BuildingType(sgd->PopUnsignedInt())), senderNation(Nation(sgd->PopUnsignedInt())) { }

glArchivItem_Bitmap *ImagePostMsgWithLocation::GetImage_() const
{
  if (senderBuilding == BLD_NOTHING)
  {
    // mal sehen, nach messagetype bebildern?
  }
  return GetBobImage(senderNation, 250+5*senderBuilding);
}



void ImagePostMsgWithLocation::Serialize(SerializedGameData *sgd)
{
  PostMsgWithLocation::Serialize(sgd);
  sgd->PushUnsignedInt(senderBuilding);
  sgd->PushUnsignedInt(senderNation);
}


DiplomacyPostMsg::DiplomacyPostMsg(const std::string& text, PostMessageCategory cat, unsigned playerID)
: PostMsg(text, cat), playerID(playerID) { type = PMT_DIPLOMACY; }

DiplomacyPostMsg::DiplomacyPostMsg(SerializedGameData *sgd)
: PostMsg(sgd) {
  playerID = sgd->PopUnsignedInt();
}

void DiplomacyPostMsg::Serialize(SerializedGameData *sgd)
{
  PostMsg::Serialize(sgd);
  sgd->PushUnsignedInt(playerID);
}
