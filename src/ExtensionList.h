// $Id: ExtensionList.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef EXTENSIONLIST_H_INCLUDED
#define EXTENSIONLIST_H_INCLUDED

#pragma once

#ifdef _WIN32
	#include <windows.h>
#endif // _WIN32

#include <GL/gl.h>
#include <GL/glext.h>

// WGL_EXT_swap_control
#ifdef _WIN32
	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
#else
	typedef int (*PFNWGLSWAPINTERVALFARPROC)(int);
#endif

extern PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;

// GL_ARB_vertex_buffer_object
extern PFNGLBINDBUFFERARBPROC glBindBufferARB; // VBO Bind-Prozedur
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB; // VBO Lösch-Prozedur
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB; // VBO Namens Generations-Prozedur
extern PFNGLBUFFERDATAARBPROC glBufferDataARB; // VBO Daten-Lade-Prozedur
extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB; /// VBO Daten-Änder-Prozedur

// GL_EXT_paletted_texture
extern PFNGLCOLORTABLEEXTPROC glColorTableEXT;

#endif // EXTENSIONLIST_H_INCLUDED
