// $Id: write_char.cpp 4653 2009-03-29 10:17:28Z FloSoft $
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
#include "libendian.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt Little-Endian kodierte Chars aus einer Datei.
 *
 *  @param[out] from  Quellpuffer
 *  @param[in]  count Anzahl zu schreibende Zeichen
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Anzahl der geschriebenen Bytes, -1 bei Fehler
 *
 *  @author FloSoft
 */
int libendian::le_write_c(const char *from, unsigned int count, FILE *file)
{
	return le_write_uc( (const unsigned char*)from, count, file);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  schreibt Little-Endian kodierte Unsigned Chars aus einer Datei.
 *
 *  @param[out] from  Quellpuffer
 *  @param[in]  count Anzahl zu schreibende Zeichen
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Anzahl der geschriebenen Bytes, -1 bei Fehler
 *
 *  @author FloSoft
 */
int libendian::le_write_uc(const unsigned char *from, unsigned int count, FILE *file)
{
	if(from == NULL || file == NULL)
		return -1;

	// chars k�nnen wir ohne Konvertierung einlesen
	return (int)fwrite(from, 1, count, file);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt Big-Endian kodierte Chars aus einer Datei.
 *
 *  @param[out] from  Quellpuffer
 *  @param[in]  count Anzahl zu schreibende Zeichen
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Anzahl der geschriebenen Bytes, -1 bei Fehler
 *
 *  @author FloSoft
 */
int libendian::be_write_c(const char *from, unsigned int count, FILE *file)
{
	return be_write_uc( (const unsigned char*)from, count, file);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  schreibt Big-Endian kodierte Unsigned Chars aus einer Datei.
 *
 *  @param[out] from  Quellpuffer
 *  @param[in]  count Anzahl zu schreibende Zeichen
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Anzahl der geschriebenen Bytes, -1 bei Fehler
 *
 *  @author FloSoft
 */
int libendian::be_write_uc(const unsigned char *from, unsigned int count, FILE *file)
{
	if(from == NULL || file == NULL)
		return -1;

	// chars k�nnen wir ohne Konvertierung einlesen
	return (int)fwrite(from, 1, count, file);
}
