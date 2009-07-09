// $Id: iwHQ.cpp 5238 2009-07-09 20:50:28Z FloSoft $
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
#include "iwHQ.h"

#include "Loader.h"
#include "nobHQ.h"
#include "ctrlGroup.h"
#include "GameClient.h"
#include "GameCommands.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwHQ.
 *
 *  @author OLiver
 */
iwHQ::iwHQ(GameWorldViewer * const gwv,nobHQ *hq)
	: iwBaseWarehouse(gwv,_("Headquarters"), 3, hq)
{
	// Soldaten Reservierungsseite
	ctrlGroup * reserve = AddGroup(102);

	// "Reserve"-Überschrift
	reserve->AddText(0,83,70,_("Reserve"),0xFFFFFF00,glArchivItem_Font::DF_CENTER,NormalFont);

	// Y-Abstand zwischen den Zeilen
	const unsigned Y_DISTANCE = 30;

	for(unsigned i = 0;i<5;++i)
	{
		// Bildhintergrund
		reserve->AddImage(1+i,34,107+Y_DISTANCE*i,GetImage(map_lst,  2298));
		// Rang-Bild
		reserve->AddImage(6+i,34,107+Y_DISTANCE*i,GetImage(map_lst,  2321+i));
		// Minus-Button
		reserve->AddImageButton(11+i,54,95+Y_DISTANCE*i,24,24,TC_RED1,LOADER.GetImageN("io",139),_("Less"));
		// Plus-Button
		reserve->AddImageButton(16+i,118,95+Y_DISTANCE*i,24,24,TC_GREEN2,LOADER.GetImageN("io",138),_("More"));
		// Anzahl-Text
		reserve->AddVarText(21+i,100,100+Y_DISTANCE*i,_("%u/%u"),0xFFFFFF00,glArchivItem_Font::DF_CENTER,NormalFont,2,
			hq->GetReservePointerAvailable(i),hq->GetReservePointerClaimed(i));
	}

	reserve->SetVisible(false);
}

void iwHQ::Msg_Group_ButtonClick(const unsigned int group_id, const unsigned int ctrl_id)
{
	// Minus-Button
	if(ctrl_id >= 11 && ctrl_id < 16)
	{
		// Netzwerk-Nachricht generieren
		GameClient::inst().AddGC(new gc::ChangeReserve(wh->GetX(),wh->GetY(),ctrl_id-11,wh->DecreaseReserveVisual(ctrl_id-11)));
	}
	// Plus-Button
	else if(ctrl_id >= 16 && ctrl_id < 21)
	{
		// Netzwerk-Nachricht generieren
		GameClient::inst().AddGC(new gc::ChangeReserve(wh->GetX(),wh->GetY(),ctrl_id-16,wh->IncreaseReserveVisual(ctrl_id-16)));
	}

	

	 // an Basis weiterleiten
	iwBaseWarehouse::Msg_Group_ButtonClick(group_id,ctrl_id);
}
