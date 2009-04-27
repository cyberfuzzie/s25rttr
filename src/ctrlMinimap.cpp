// $Id: ctrlMinimap.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "ctrlMinimap.h"
#include "MinimapConsts.h"
#include "Minimap.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
ctrlMinimap::ctrlMinimap( Window *parent, 
				 const unsigned int id, 
				 const unsigned short x, 
				 const unsigned short y, 
				 const unsigned short width, 
				 const unsigned short height,
				 const unsigned short padding_x,
				 const unsigned short padding_y,
				 const unsigned short map_width,
				 const unsigned short map_height) : ctrlRectangle(parent,id,x,y,width,height), padding_x(padding_x), padding_y(padding_y)
{
	SetDisplaySize(width,height,map_width,map_height);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void ctrlMinimap::SetDisplaySize(const unsigned short width, const unsigned short height, const unsigned short map_width,	const unsigned short map_height)
{
	this->width = width;
	this->height = height;

	unsigned short scaled_map_width = static_cast<unsigned short>(map_width*MINIMAP_SCALE_X);
	double x_scale = double(scaled_map_width) / double(width-padding_x*2);
	double y_scale = double(map_height) / double(height-padding_y*2);

	bool scale_width = false;

	if(x_scale > y_scale)
		scale_width = false;
	else
		scale_width = true;

	if(scale_width)
	{
		width_show = scaled_map_width*(height-padding_y*2)/map_height;
		height_show = height-padding_y*2;
	}
	else
	{
		height_show = map_height*(width-padding_x*2)/scaled_map_width;
		width_show = width-padding_x*2;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void ctrlMinimap::DrawMap(Minimap& map)
{
	// Map ansich zeichnen
	map.Draw(GetX()+GetLeft(), GetY()+GetTop(), 
		width_show, height_show);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void ctrlMinimap::RemoveBoundingBox(const unsigned short width_min, const unsigned short height_min)
{
	width = max<unsigned short>(width_show+padding_x*2,width_min);
	height = max<unsigned short>(height_show+padding_y*2,height_min);
}
