// $Id: mygettext.h 7521 2011-09-08 20:45:55Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.
#ifndef MYGETTEXT_H_INCLUDED
#define MYGETTEXT_H_INCLUDED

#pragma once

const char *mysetlocale(int category, const char *locale);

#undef gettext
#define gettext mygettext
const char *mygettext(const char *msgid);

#undef bindtextdomain
#define bindtextdomain mybindtextdomain
const char *mybindtextdomain(const char *domainname, const char *dirname);

#undef textdomain
#define textdomain mytextdomain
const char *mytextdomain(const char *domainname);

#undef bind_textdomain_codeset
#define bind_textdomain_codeset mybind_textdomain_codeset
const char *mybind_textdomain_codeset(const char *domainname, const char *codeset);

#include <string>

const std::string EMPTY_STRING = "";

inline const char * _(const char * const txt) { return gettext(txt); }
inline const char * _(const std::string& txt) { return gettext(txt.c_str()); }
inline const char * const gettext_noop(const char * const str) { return str; }

#endif // !MYGETTEXT_H_INCLUDED
