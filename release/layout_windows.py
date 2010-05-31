###############################################################################
# $Id: layout_windows.py 6458 2010-05-31 11:38:51Z FloSoft $
# $Author: FloSoft $
###############################################################################

layout_windows = ( 
	'windows',
	[ # architectures
		(
			[ 'i386' ],
			[ # paths
				[
					'',
					[ # include
						'src/s25client.exe',
						'../release/bin/rttr.bat',
						'/usr/i586-mingw32msvc/bin/libiconv2.dll',
						'/usr/i586-mingw32msvc/bin/libintl3.dll',
						'/usr/i586-mingw32msvc/bin/libogg-0.dll',
						'/usr/i586-mingw32msvc/bin/SDL_mixer.dll',
						'/usr/i586-mingw32msvc/bin/SDL.dll',
						'/usr/i586-mingw32msvc/bin/libvorbis-0.dll',
						'/usr/i586-mingw32msvc/bin/libvorbisfile-3.dll',
					],
				],
				[
					'driver/video',
					[ # include
						'driver/video/*/src/*.dll'
					]
				],
				[
					'driver/audio',
					[ # include
						'driver/audio/*/src/*.dll'
					]
				],
				[
					'RTTR',
					[ # include
						's25update/src/s25update.exe',
						's-c/src/sound-convert.exe',
						's-c/resample-1.8.1/src/s-c_resample.exe',
						'/usr/i586-mingw32msvc/bin/libcurl-4.dll',
						'/usr/i586-mingw32msvc/bin/zlib1.dll'
					]
				],
				[
					'RTTR/languages',
					[ # include
						'RTTR/languages/*.mo'
					]
				],
			],
			[ # archives
				'zip',
			]
		)
	],
)

###############################################################################
# EOF
###############################################################################
