// $Id: read_int.cpp 5853 2010-01-04 16:14:16Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "libendian.h"

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liest einen Little-Endian kodierten Integer aus einer Datei.
 *
 *  @param[out] to    Pointer auf Zielinteger
 *  @param[in]  file  Datei aus der gelesen werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::le_read_i(int *to, FILE *file)
{
	return le_read_ui( (unsigned int*)to, file);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liest einen Little-Endian kodierten Unsigned Integer aus einer Datei.
 *
 *  @param[out] to    Pointer auf Zielinteger
 *  @param[in]  file  Datei aus der gelesen werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::le_read_ui(unsigned int *to, FILE *file)
{
	if(to == NULL || file == NULL)
		return -1;

	// Integer einlesen
	if(fread(to, 1, 4, file) != 4)
		return 1;

	// müssen wir konvertieren?
	if(BYTE_ORDER != LITTLE_ENDIAN)
	{
		// ja, dann tauschen
		*to = swap_ui(*to);
	}

	// alles ok
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liest einen Big-Endian kodierten Integer aus einer Datei.
 *
 *  @param[out] to    Pointer auf Zielinteger
 *  @param[in]  file  Datei aus der gelesen werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::be_read_i(int *to, FILE *file)
{
	return be_read_ui( (unsigned int*)to, file);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liest einen Big-Endian kodierten Unsigned Integer aus einer Datei.
 *
 *  @param[out] to    Pointer auf Zielinteger
 *  @param[in]  file  Datei aus der gelesen werden soll
 *
 *  @return liefert Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libendian::be_read_ui(unsigned int *to, FILE *file)
{
	if(to == NULL || file == NULL)
		return -1;

	// Integer einlesen
	if(fread(to, 1, 4, file) != 4)
		return 1;

	// müssen wir konvertieren?
	if(BYTE_ORDER != BIG_ENDIAN)
	{
		// ja, dann tauschen
		*to = swap_ui(*to);
	}

	// alles ok
	return 0;
}
