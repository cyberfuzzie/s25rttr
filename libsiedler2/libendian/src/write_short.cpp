// $Id: write_short.cpp 4653 2009-03-29 10:17:28Z FloSoft $
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
 *  schreibt einen Little-Endian kodierten Short in eine Datei.
 *
 *  @param[out] from  Pointer auf Quellshort
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::le_write_s(short from, FILE *file)
{
	return le_write_us( (unsigned short)from, file);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  schreibt einen Little-Endian kodierten Unsigned Short in eine Datei.
 *
 *  @param[out] from  Pointer auf Quellshort
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::le_write_us(unsigned short from, FILE *file)
{
	if(file == NULL)
		return -1;

	// müssen wir konvertieren?
	if(BYTE_ORDER != LITTLE_ENDIAN)
	{
		// ja, dann tauschen
		from = swap_us(from);
	}

	// Integer einlesen
	if(fwrite(&from, 1, 2, file) != 2)
		return 1;

	// alles ok
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  schreibt einen Big-Endian kodierten Short in eine Datei.
 *
 *  @param[out] from  Pointer auf Quellshort
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::be_write_s(short from, FILE *file)
{
	return be_write_us( (unsigned short)from, file);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  schreibt einen Big-Endian kodierten Unsigned Short in eine Datei.
 *
 *  @param[out] from  Pointer auf Quellshort
 *  @param[in]  file  Datei in die geschrieben werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::be_write_us(unsigned short from, FILE *file)
{
	if(file == NULL)
		return -1;

	// müssen wir konvertieren?
	if(BYTE_ORDER != BIG_ENDIAN)
	{
		// ja, dann tauschen
		from = swap_us(from);
	}

	// Integer einlesen
	if(fwrite(&from, 1, 2, file) != 2)
		return 1;

	// alles ok
	return 0;
}
