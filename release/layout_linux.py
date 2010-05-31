###############################################################################
# $Id: layout_linux.py 6458 2010-05-31 11:38:51Z FloSoft $
# $Author: FloSoft $
###############################################################################

layout_linux = ( 
	'linux',
	[ # architectures
		(
			[ 'i386', 'x86_64' ],
			[ # paths
				[ 
					'bin',
					[ # include
						'src/s25client',
						'release/bin/rttr.sh'
					]
				],
				[ 
					'share/s25rttr/driver/video', 
					[ # include
						'driver/video/*.so',
					]
				],
				[ 
					'share/s25rttr/driver/audio', 
					[ # include
						'driver/audio/*.so',
					]
				],
				[ 
					'share/s25rttr/RTTR', 
					[ # include
						's25update/src/s25update',
						's-c/src/sound-convert',
						's-c/resample-1.8.1/src/s-c_resample',
					]
				],
				[
					'share/s25rttr/RTTR/languages',
					[ # include
						'RTTR/languages/*.mo'
					]
				],
			],
			[ # archives
				'tar',
				'deb'
			]
		)
	],
)

###############################################################################
# EOF
###############################################################################
