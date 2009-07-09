// $Id: Loader.cpp 5239 2009-07-09 21:15:45Z FloSoft $
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
	if(!LoadPalettes())
		return false;
	if(!LoadBackgrounds())
		return false;
	if(!LoadTXTs())
		return false;
	if(!LoadSounds())
		return false;

	// rttr.lst laden
	if(!LoadFile(FILE_PATHS[16], GetPalette(0), &rttr_lst))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt die Menüdateien.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 *  @author OLiver
 */
bool Loader::LoadMenu(void)
{
	// resource.dat laden
	if(!LoadFile(FILE_PATHS[11], GetPalette(0)))
		return false;

	// io.dat laden
	if(!LoadFile(FILE_PATHS[12], GetPalette(0)))
		return false;

	// outline_fonts.lst laden
	if(!LoadFile(FILE_PATHS[54], GetPalette(0)))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt die Paletten.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 *  @author OLiver
 */
bool Loader::LoadPalettes(void)
{
	palettes.alloc(6);

	for(unsigned char i = 0; i < 6; ++i)
	{
		libsiedler2::ArchivInfo bbm;
		if(!LoadFile(FILE_PATHS[ i + 5 ], NULL, &bbm ) )
			return false;

		palettes.setC(i, bbm.get(0));
	}
	libsiedler2::ArchivInfo bbm;
	if(!LoadFile(FILE_PATHS[17], NULL, &bbm ) )
		return false;
	palettes.pushC(bbm.get(0));

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt die Hintergrunddateien.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 *  @author OLiver
 */
bool Loader::LoadBackgrounds(void)
{
	backgrounds.alloc(4);

	// Splash anzeigen
	libsiedler2::ArchivInfo splash;
	if(!LoadFile(FP_MENU_BACKGROUNDS[FILE_SPLASH_ID], NULL, &splash ) )
		return false;

	glArchivItem_Bitmap *image = dynamic_cast<glArchivItem_Bitmap*>(splash.get(0));
	image->setFilter(GL_LINEAR);
	image->Draw(0, 0, VideoDriverWrapper::inst().GetScreenWidth(), VideoDriverWrapper::inst().GetScreenHeight(), 0, 0, 0, 0);
	VideoDriverWrapper::inst().SwapBuffers();

	for(unsigned char i = 0; i < 4; ++i)
	{
		libsiedler2::ArchivInfo bgd;
		if(i == FILE_SPLASH_ID)
		{
			backgrounds.setC(i, splash.get(0));
		}
		else
		{
			if(!LoadFile(FP_MENU_BACKGROUNDS[i], NULL, &bgd ) )
				return false;

			if(bgd.get(0) == NULL || bgd.get(0)->getBobType() == libsiedler2::BOBTYPE_PALETTE)
				image = dynamic_cast<glArchivItem_Bitmap*>(bgd.get(1));
			else
				image = dynamic_cast<glArchivItem_Bitmap*>(bgd.get(0));

			if(image)
				image->setFilter(GL_LINEAR);
			backgrounds.setC(i, image);
		}
	}

	#ifndef OLIVER
		pics.alloc(20);

		for(unsigned char i = 0; i < 20; ++i)
		{
	#else
		pics.alloc(1);

		for(unsigned char i = 0; i < 1; ++i)
		{
	#endif

		libsiedler2::ArchivInfo bgd;

		if(!LoadFile(FILE_PATHS[ 64 + i ], NULL, &bgd ) )
			return false;

		if(bgd.get(0) == NULL || bgd.get(0)->getBobType() == libsiedler2::BOBTYPE_PALETTE)
			image = dynamic_cast<glArchivItem_Bitmap*>(bgd.get(1));
		else
			image = dynamic_cast<glArchivItem_Bitmap*>(bgd.get(0));

		if(image)
			image->setFilter(GL_LINEAR);
		pics.setC(i, image);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt alle Textfiles.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::LoadTXTs(void)
{
	// client.ger.local laden
	LOG.lprintf("lade %s: ", FILE_PATHS[87]);
	if(libsiedler2::loader::LoadTXT(GetFilePath( FILE_PATHS[87]).c_str(), &client_txt, false) == 0)
	{
		LOG.lprintf("fertig\n");
		LOG.lprintf("WARNUNG: Benutze lokale Debug-Konfiguration\n");
	}
	else
	{
		LOG.lprintf("fehlgeschlagen\nKeine lokale Debug-Konfiguration gefunden\n");
		// client.ger laden
		LOG.lprintf("lade %s\n", FILE_PATHS[18]);
		if(libsiedler2::loader::LoadTXT(GetFilePath(FILE_PATHS[18]).c_str(), &client_txt, false) != 0)
			return false;
	}

	// lang.ger laden
	LOG.lprintf("lade %s: ", FILE_PATHS[89]);
	if(libsiedler2::loader::LoadTXT(GetFilePath(FILE_PATHS[89]).c_str(), &lang_txt, false) == 0)
		LOG.lprintf("fertig\n");
	else
	{
		LOG.lprintf("fehlgeschlagen\n");
		return false;
	}

	

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
	// konvertierte sound.lst laden
	if(!LoadFile(FILE_PATHS[55], GetPalette(0), &sound_lst))
	{
		char cmd[4096];
		char name[4096];
#ifdef _WIN32
		strcpy(name, "\\sound-convert.exe");
#else
		strcpy(name, "/sound-convert");
#endif
		snprintf(cmd, 4096, "%s%s -s \"%s\" -f \"%s\" -t \"%s\"", GetFilePath(FILE_PATHS[57]).c_str(), name, GetFilePath(FILE_PATHS[56]).c_str(), GetFilePath(FILE_PATHS[49]).c_str(), GetFilePath(FILE_PATHS[55]).c_str());
		//LOG.lprintf("%s\n", cmd);
#ifdef _WIN32
		for(unsigned int x = 0; x < 4096; ++x)
		{
			if(cmd[x] == '/')
				cmd[x] = '\\';
		}
#endif // !_WIN32
		if(system(cmd) == -1)
			return false;

		// 2ter versuch
		if(!LoadFile(FILE_PATHS[55], GetPalette(0), &sound_lst))
		{
			// Datei kopieren, und zwar schön portabel :P ...

			LOG.lprintf("Kopiere Datei %s nach %s: ", GetFilePath(FILE_PATHS[49]).c_str(), GetFilePath(FILE_PATHS[55]).c_str());

			FILE *f = fopen(GetFilePath(FILE_PATHS[49]).c_str(), "rb");
			FILE *t = fopen(GetFilePath(FILE_PATHS[55]).c_str(), "wb");
			if(!f || !t)
			{
				LOG.lprintf("fehlgeschlagen\n");
				return false;
			}
			LOG.lprintf("fertig\n");

			while(!feof(f))
			{
				char buf[4096];
				size_t read = fread(buf, 1, 4096, f);
				if(read > 0)
					fwrite(buf, 1, read, t);
			}

			fclose(f);
			fclose(t);

			// last chance
			if(!LoadFile(FILE_PATHS[55], GetPalette(0), &sound_lst))
				return false;
		}
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
 *
 *  @return @p true bei Erfolg, @p false bei Fehler.
 *
 *  @author FloSoft
 */
bool Loader::LoadFile(const char *pfad, const libsiedler2::ArchivItem_Palette *palette)
{
	std::string p = pfad;
	transform ( p.begin(), p.end(), p.begin(), tolower );

	unsigned int pp = p.find_last_of('.');
	if(pp != std::string::npos)
		p = p.substr(0, pp);

	pp = p.find_last_of("/\\");
	if(pp != std::string::npos)
		p = p.substr(pp+1);

	// leeres Archiv in Map einfügen
	libsiedler2::ArchivInfo archiv;
	files.insert(std::make_pair(p, archiv));

	// dann daten reinladen um kopieren zu vermeiden
	if(!LoadFile(pfad, palette, &files.find(p)->second))
		return false;

	/// @todo: ggf items nur überschreiben falls eintrag in map schon vorhanden, so könnte man "overrides" implementieren, 
	/// das man einzelen originalbilder durch eigene dateien überschreibt

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
			if(!LoadFile(FILE_PATHS[27 + i + (gfxset == 2)*NATION_COUNT], GetPalette(0), &nation_bobs[i]))
				return false;
	
			nation_icons[i].clear();
			if(!LoadFile(FILE_PATHS[35 + i], GetPalette(0), &nation_icons[i]))
				return false;
		}
	}

	// ab hier nur noch bei anderem gfxset laden
	static unsigned char lastgfx = 0xFF;
	if(lastgfx == gfxset)
		return true;

	// rttr.lst laden
	rttr_lst.clear();
	if(!LoadFile(FILE_PATHS[16], GetPalette(0), &rttr_lst))
	{
		lastgfx = 0xFF;
		return false;
	}

	// map_?_z.lst laden
	map_lst.clear();
	if(!LoadFile(FILE_PATHS[23+gfxset], GetPalette(0), &map_lst))
	{
		lastgfx = 0xFF;
		return false;
	}

	for(unsigned char i = 0; i < 6; ++i)
	{
		// mis?bobs.lst laden
		misxbobs[i].clear();
		if(!LoadFile(FILE_PATHS[58+i], GetPalette(0), &misxbobs[i]))
		{
			lastgfx = 0xFF;
			return false;
		}
	}

	// rombobs.lst laden
	rombobs_lst.clear();
	if(!LoadFile(FILE_PATHS[26], GetPalette(0), &rombobs_lst))
	{
		lastgfx = 0xFF;
		return false;
	}

	// carrier.bob laden
	carrier_bob.clear();
	if(!LoadFile(FILE_PATHS[44], GetPalette(0), &carrier_bob))
	{
		lastgfx = 0xFF;
		return false;
	}

	// jobs.bob laden
	jobs_bob.clear();
	if(!LoadFile(FILE_PATHS[45], GetPalette(0), &jobs_bob))
	{
		lastgfx = 0xFF;
		return false;
	}

	// BOOT.LST laden
	boat_lst.clear();
	if(!LoadFile(FILE_PATHS[86], GetPalette(0), &boat_lst))
	{
		lastgfx = 0xFF;
		return false;
	}

	// BOOT.LST laden
	boot_lst.clear();
	if(!LoadFile(FILE_PATHS[92], GetPalette(0), &boot_lst))
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
	if(!LoadFile(FILE_PATHS[20 + gfxset], GetPalette(0), &lbm))
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
	libsiedler2::ArchivItem_Palette *palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(source->get(0));
	glArchivItem_Bitmap *image = dynamic_cast<glArchivItem_Bitmap*>(source->get(1));

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
	libsiedler2::ArchivItem_Palette *palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(source->get(0));
	glArchivItem_Bitmap *image = dynamic_cast<glArchivItem_Bitmap*>(source->get(1));

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
