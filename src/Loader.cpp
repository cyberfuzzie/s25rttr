// $Id: Loader.cpp 5253 2009-07-12 14:42:18Z FloSoft $
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
#include "Loader.h"

#include "files.h"
#include "GlobalVars.h"
#include "Settings.h"

#include "VideoDriverWrapper.h"
#include "AudioDriverWrapper.h"

#include "CollisionDetection.h"
#include "GameClient.h"
#include "GameClientPlayer.h"
#include "ListDir.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p Loader.
 *
 *  @author FloSoft
 */
Loader::Loader(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p Loader.
 *
 *  @author FloSoft
 */
Loader::~Loader(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt alle allgemeinen Dateien.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 *  @author OLiver
 */
bool Loader::LoadFiles(void)
{
	const unsigned int files_count = 7 + 1 + 2 + 3 + 21;

	const unsigned int files[] = { 
		5, 6, 7, 8, 9, 10, 17, // Paletten:     pal5.bbm, pal6.bbm, pal7.bbm, paletti0.bbm, paletti1.bbm, paletti8.bbm, colors.act
		104,                   // Splashimage:  splash.bmp
		11, 12,                // Menüdateien:  resource.dat, io.dat
		101, 102, 103,         // Hintergründe: menu.bmp, setup013.lbm, setup015.lbm
		64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84 // Die ganzen Spielladescreens.
	};

	if(!LoadFilesFromArray(files_count, files))
		return false;

	if(!LoadSounds())
		return false;

	if(!LoadLsts(95)) // lade systemweite und persönliche lst files
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt Dateien aus FILE_PATHS bzw aus dem Verzeichnis.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::LoadFilesFromArray(const unsigned int files_count, const unsigned int *files, bool load_always)
{
	// load the files or directorys
	for(unsigned int i = 0; i < files_count; ++i)
	{
		// is the entry a directory?
		if(IsDir(FILE_PATHS[ files[i] ]))
		{
			// yes, load all files in the directory
			LOG.lprintf("Lade LST Dateien aus \"%s\"\n", GetFilePath(FILE_PATHS[ files[i] ]).c_str());

			std::list<std::string> lst;
			ListDir(GetFilePath(FILE_PATHS[ files[i] ]) + "*.lst", NULL, NULL, &lst);

			for(std::list<std::string>::iterator i = lst.begin(); i != lst.end(); ++i)
			{
				if(!LoadFile( i->c_str(), GetPaletteN("pal5"), load_always ) )
					return false;
			}
			LOG.lprintf("Fertig\n");
		}
		else
		{
			// no, only single file specified
			if(!LoadFile(FILE_PATHS[ files[i] ], GetPaletteN("pal5"), load_always ) )
				return false;

			// ggf Splash anzeigen
			if(files[i] == FILE_SPLASH_ID)
			{
				glArchivItem_Bitmap *image = GetImageN("splash", 0);
				image->setFilter(GL_LINEAR);
				image->Draw(0, 0, VideoDriverWrapper::inst().GetScreenWidth(), VideoDriverWrapper::inst().GetScreenHeight(), 0, 0, 0, 0);
				VideoDriverWrapper::inst().SwapBuffers();
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt die "override" lst-files aus den systemweiten und persönlichen verzeichnissen
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::LoadLsts(unsigned int dir)
{
	// systemweite lsts laden
	unsigned int files_count = 0;
	unsigned int *files = NULL;

	if(GetFilePath(FILE_PATHS[dir]) == GetFilePath(FILE_PATHS[dir+3]))
	{
		files_count = 1;
		files = new unsigned int[1];
		files[0] = dir;
	}
	else
	{
		files_count = 2;
		files = new unsigned int[2];
		files[0] = dir;
		files[1] = dir + 3;
	}

	if(!LoadFilesFromArray(files_count, files) )
	{
		delete[] files;
		return false;
	}

	delete[] files;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt alle Sounds.
 *
 *  @return liefert true bei Erfolg, false bei Fehler
 *
 *  @author FloSoft
 */
bool Loader::LoadSounds(void)
{
	// ist die konvertierte sound.lst vorhanden?
	if(!FileExists(FILE_PATHS[55]))
	{
		// nein, dann konvertieren

		std::stringstream cmdss;
		cmdss << GetFilePath(FILE_PATHS[57]); // pfad zum sound-converter hinzufügen

		// name anhängen
#ifdef _WIN32
		cmdss << "\\sound-convert.exe";
#else
		cmdss << "/sound-convert";
#endif
		
		// parameter anhängen
		cmdss << " -s \"";
		cmdss << GetFilePath(FILE_PATHS[56]); // script
		cmdss << "\" -f \"";
		cmdss << GetFilePath(FILE_PATHS[49]); // quelle
		cmdss << "\" -t \"";
		cmdss << GetFilePath(FILE_PATHS[55]); // ziel
		cmdss << "\"";

		std::string cmd = cmdss.str();
#ifdef _WIN32
		std::replace(cmd.begin(), cmd.end(), '/', '\\'); // Slash in Backslash verwandeln, sonst will "system" unter win nicht
#endif // _WIN32

		if(system(cmd.c_str()) == -1)
			return false;

		// die konvertierte muss nicht extra geladen werden, da sie im override-ordner landet
	}

	// ggf original laden, hier das overriding benutzen wär ladezeitverschwendung
	if(!FileExists(FILE_PATHS[55]))
	{
		// existiert nicht 
		if(!LoadFile(FILE_PATHS[49]))
			return false;
	}

	std::list<std::string> liste;
	ListDir(GetFilePath(FILE_PATHS[50]), NULL, NULL, &liste);

	unsigned int i = 0;
	sng_lst.alloc(unsigned(liste.size()));
	for(std::list<std::string>::iterator it = liste.begin(); it!=liste.end(); ++it)
	{
		libsiedler2::ArchivInfo sng;

		LOG.lprintf("lade \"%s\": ", it->c_str());
		if(libsiedler2::loader::LoadSND(it->c_str(), &sng) != 0 )
		{
			LOG.lprintf("fehlgeschlagen\n");
			return false;
		}
		LOG.lprintf("fertig\n");

		sng_lst.setC(i++, sng.get(0));
	}

	// Siedler I MIDI-Musik
	//sng_lst.pushC(sound_lst.get(0));

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt eine Datei in ein ArchivInfo in @p files auf Basis des Dateinames.
 *
 *  @param[in] pfad    Der Dateipfad
 *  @param[in] palette (falls benötigt) die Palette.
 *  @param[in] load_always bei @p true wird die datei immer geladen (und ggf überschrieben)
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::LoadFile(const char *pfad, const libsiedler2::ArchivItem_Palette *palette, bool load_always)
{
	std::string p = pfad;
	transform ( p.begin(), p.end(), p.begin(), tolower );

	size_t pp = p.find_last_of("/\\");
	if(pp != std::string::npos)
		p = p.substr(pp+1);

	pp = p.find_first_of('.');
	if(pp != std::string::npos)
		p = p.substr(0, pp);

	// bereits geladen und wir wollen kein nochmaliges laden
	if(!load_always && files.find(p) != files.end() && files.find(p)->second.getCount() != 0)
		return true;

	libsiedler2::ArchivInfo archiv;
	libsiedler2::ArchivInfo *to = &archiv;

	bool override_file = false;

	if(files.find(p) == files.end() || files.find(p)->second.getCount() == 0)
	{
		// leeres Archiv in Map einfügen
		files.insert(std::make_pair(p, archiv));
		to = &files.find(p)->second;
	}
	else
		override_file = true;

	// dann daten reinladen um ggf. kopieren zu vermeiden
	if(!LoadFile(pfad, palette, to))
		return false;

	// haben wir eine override file? dann nicht-leere items überschreiben
	if(override_file)
	{
		LOG.lprintf("Ersetze Daten der vorher geladenen Datei\n");
		to = &files.find(p)->second;
		for(unsigned int i = 0; i < archiv.getCount(); ++i)
		{
			if(archiv.get(i))
			{
				if(to->get(i))
					delete to->get(i);
				to->setC(i, archiv.get(i));
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt eine Datei in ein ArchivInfo.
 *
 *  @param[in] pfad    Der Dateipfad
 *  @param[in] palette (falls benötigt) die Palette.
 *  @param[in] archiv  Das Zielarchivinfo.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::LoadFile(const char *pfad, const libsiedler2::ArchivItem_Palette *palette, libsiedler2::ArchivInfo *archiv)
{
	unsigned int ladezeit = VideoDriverWrapper::inst().GetTickCount();

	std::string file = GetFilePath(pfad);

	LOG.lprintf("lade \"%s\": ", file.c_str());
	fflush(stdout);

	if(libsiedler2::Load(file.c_str(), archiv, palette) != 0)
	{
		LOG.lprintf("fehlgeschlagen\n");
		return false;
	}

	LOG.lprintf("fertig (%ums)\n", VideoDriverWrapper::inst().GetTickCount()-ladezeit);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt die Settings.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::LoadSettings()
{
	settings.clear();

	std::string file = GetFilePath(FILE_PATHS[0]);

	LOG.lprintf("lade \"%s\": ", file.c_str());
	fflush(stdout);

	if(libsiedler2::Load(file.c_str(), &settings) != 0)
		return false;

	LOG.lprintf("fertig\n");

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Speichert die Settings.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::SaveSettings()
{
	std::string file = GetFilePath(FILE_PATHS[0]);

	LOG.lprintf("schreibe \"%s\": ", file.c_str());
	fflush(stdout);

	if(libsiedler2::Write(file.c_str(), &settings) != 0)
		return false;

	LOG.lprintf("fertig\n");

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt die Spieldateien.
 *
 *  @param[in] gfxset  Das GFX-Set
 *  @param[in] nations Array der zu ladenden Nationen.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author OLiver
 */
bool Loader::LoadGame(unsigned char gfxset, bool *nations)
{
	for(unsigned char i = 0; i < NATION_COUNT; ++i)
	{
		if(nations[i])
		{
			// Völker-Grafiken laden
			nation_bobs[i].clear();
			if(!LoadFile(FILE_PATHS[27 + i + (gfxset == 2)*NATION_COUNT], GetPaletteN("pal5"), &nation_bobs[i]))
				return false;
		}
	}

	if(!LoadLsts(96)) // lade systemweite und persönliche lst files
		return false;

	// ab hier nur noch bei anderem gfxset laden
	static unsigned char lastgfx = 0xFF;
	if(lastgfx == gfxset)
		return true;

	// map_?_z.lst laden
	map_lst.clear();
	if(!LoadFile(FILE_PATHS[23+gfxset], GetPaletteN("pal5"), &map_lst))
	{
		lastgfx = 0xFF;
		return false;
	}

	const unsigned int files_count = 5 + 6 + 4;

	const unsigned int files[files_count] = { 
		26, 44, 45, 86, 92,      // rom_bobs.lst, carrier.bob, jobs.bob, boat.lst, boot_z.lst
		58, 59, 60, 61, 62, 63,  // mis0bobs.lst, mis1bobs.lst, mis2bobs.lst, mis3bobs.lst, mis4bobs.lst, mis5bobs.lst
		35, 36, 37, 38           // afr_icon.lst, jap_icon.lst, rom_icon.lst, vik_icon.lst
	};

	// dateien ggf nur einmal laden
	if(!LoadFilesFromArray(files_count, files, false))
	{
		lastgfx = 0xFF;
		return false;
	}

	if(!LoadLsts(97)) // lade systemweite und persönliche lst files
	{
		lastgfx = 0xFF;
		return false;
	}

	lastgfx = gfxset;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt das Terrain.
 *
 *  @param[in] gfxset  Das GFX-Set
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author OLiver
 */
bool Loader::LoadTerrain(unsigned char gfxset)
{
	if(gfxset > 2)
	{
		error("Loader::LoadTerrain: Invalid gfxset \"%d\"\n", gfxset);
		return false;
	}

	static unsigned char lastgfx = 0xFF;
	if(lastgfx == gfxset)
		return true;

	libsiedler2::ArchivInfo lbm;

	// tex?.lbm laden
	if(!LoadFile(FILE_PATHS[20 + gfxset], GetPaletteN("pal5"), &lbm))
		return false;

	// Unanimierte Texturen
	Rect rects[16] = {
		{0,0,48,48},
		{48,0,96,48},
		{96,0,144,48},
		{144,0,192,48},

		{0,48,48,96},
		{48,48,96,96},
		{96,48,144,96},
		{144,48,192,96},

		{0,96,48,144},
		{48,96,96,144},
		{96,96,144,144},
		{144,96,192,144},

		{0,144,48,192},
		{48,144,96,192},

		{192,48,247,104},
		{192,104,247,160},
	};

	// Ränder
	Rect rec_raender[5] =
	{
		{192,176,256,192}, // Schnee
		{192,192,256,208}, // Berg
		{192,208,256,224}, // Wste
		{192,224,256,240}, // Wiese
		{192,240,256,256}  // Wasser
	};

	// Wege
	Rect rec_roads[8] =
	{
		{192,0,242,16},
		{192,16,242,32},
		{192,32,242,48},
		{192,160,242,176},

		{242,0,256,16},
		{242,16,256,32},
		{242,32,256,48},
		{242,160,256,176},
	};

	textures.clear();
	// (unanimiertes) Terrain
	for(unsigned char i = 0; i < 14; ++i)
		ExtractTexture(&lbm, &textures, rects[i]);

	// Wasser und Lava
	water.clear();
	ExtractAnimatedTexture(&lbm, &water, rects[14], 8, 240);

	lava.clear();
	ExtractAnimatedTexture(&lbm, &lava,  rects[15], 4, 248);

	// die 5 Ränder
	borders.clear();
	for(unsigned char i = 0; i < 5; ++i)
		ExtractTexture(&lbm, &borders, rec_raender[i]);

	// Wege
	roads.clear();
	roads_points.clear();
	for(unsigned char i = 0; i < 4; ++i)
	{
		ExtractTexture(&lbm, &roads, rec_roads[i]);
		ExtractTexture(&lbm, &roads_points, rec_roads[4+i]);
	}

	lastgfx = gfxset;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Extrahiert eine Textur aus den Daten.
 *
 *  @author OLiver
 */
void Loader::ExtractTexture(libsiedler2::ArchivInfo *source, libsiedler2::ArchivInfo *destination, Rect &rect)
{
	glArchivItem_Bitmap_Raw bitmap;
	libsiedler2::ArchivItem_Palette *palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(source->get(1));
	glArchivItem_Bitmap *image = dynamic_cast<glArchivItem_Bitmap*>(source->get(0));

	unsigned short width = rect.right - rect.left;
	unsigned short height = rect.bottom - rect.top;

	unsigned char *buffer = new unsigned char[width*height];

	memset(buffer, libsiedler2::TRANSPARENT_INDEX, width*height);
	image->print(buffer, width, height, libsiedler2::FORMAT_PALETTED, palette, 0, 0, rect.left, rect.top, width, height);
	for(unsigned int x = 0; x < (unsigned int)(width*height); ++x)
	{
		if(buffer[x] == 0)
			buffer[x] = libsiedler2::TRANSPARENT_INDEX;
	}

	bitmap.create(width, height, buffer, width, height, libsiedler2::FORMAT_PALETTED, palette);
	bitmap.setPalette(palette);
	bitmap.setFormat(libsiedler2::FORMAT_PALETTED);

	delete[] buffer;

	destination->pushC(&bitmap);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Extrahiert mehrere (animierte) Texturen aus den Daten.
 *
 *  @author OLiver
 */
void Loader::ExtractAnimatedTexture(libsiedler2::ArchivInfo *source, libsiedler2::ArchivInfo *destination, Rect &rect, unsigned char color_count, unsigned char start_index)
{
	glArchivItem_Bitmap_Raw bitmap;
	libsiedler2::ArchivItem_Palette *palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(source->get(1));
	glArchivItem_Bitmap *image = dynamic_cast<glArchivItem_Bitmap*>(source->get(0));

	bitmap.setPalette(palette);
	bitmap.setFormat(libsiedler2::FORMAT_PALETTED);

	unsigned short width = rect.right - rect.left;
	unsigned short height = rect.bottom - rect.top;

	unsigned char *buffer = new unsigned char[width*height];

	// Mit Startindex (also irgendeiner Farbe) füllen, um transparente Pixel und damit schwarze Punke am Rand zu verhindern
	memset(buffer, start_index, width*height);

	image->print(buffer, width, height, libsiedler2::FORMAT_PALETTED, palette, 0, 0, rect.left, rect.top, width, height);

	for(unsigned char i = 0; i < color_count; ++i)
	{
		for(unsigned int x = 0; x < width; ++x)
		{
			for(unsigned int y = 0; y < height; ++y)
			{
				if(buffer[y*width + x] >= start_index && buffer[y*width + x] < start_index+color_count)
				{
					if(++buffer[y*width + x] >= start_index+color_count)
						buffer[y*width + x] = start_index;
				}
			}
		}

		bitmap.create(width, height, buffer, width, height, libsiedler2::FORMAT_PALETTED, palette);

		destination->pushC(&bitmap);
	}

	//libsiedler2::loader::WriteLST("c:\\wasser.lst",palette,destination);

	delete[] buffer;
}
