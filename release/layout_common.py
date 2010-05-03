###############################################################################
# $Id$
# $Author$
###############################################################################

layout_common = (
	'common',
	[ # architectures
		(
			[ 'linux' ],
			[ # paths
				[
					'share/s25rttr',
					[ # include
					],
					[ # svn export
						'RTTR',
					],
					[ # exclude
						'RTTR/languages/*.mo',
						'RTTR/languages/*.po',
						'RTTR/languages/*.pot',
						'RTTR/REPLAYS/*.rpl',
						'RTTR/sound.lst',
						'RTTR/settings.bin',
						'RTTR/MUSIC',
					]
				],
				[ 
					'share/s25rttr/S2', 
					[ # include
					],
					[ # svn export
					],
					[ # exclude
					],
					[ # create
						'put your S2-Installation in here'
					],
				],
			],
			[ # archives
				'tar',
			]
		),
		(
			[ 'apple' ],
			[ # paths
				[
					's25client.app/Contents/MacOS/share/s25rttr',
					[ # include
					],
					[ # svn export
						'RTTR',
					],
					[ # exclude
						'RTTR/languages/*.mo',
						'RTTR/languages/*.po',
						'RTTR/languages/*.pot',
						'RTTR/REPLAYS/*.rpl',
						'RTTR/sound.lst',
						'RTTR/settings.bin',
						'RTTR/MUSIC',
					]
				],
				[ 
					's25client.app/Contents/MacOS/share/s25rttr/S2', 
					[ # include
					],
					[ # svn export
					],
					[ # exclude
					],
					[ # create
						'put your S2-Installation in here'
					],
				],
			],
			[ # archives
				'tar',
			]
		),
		(
			[ 'windows' ],
			[ # paths
				[
					'',
					[ # include
					],
					[ # svn export
						'RTTR',
					],
					[ # exclude
						'RTTR/languages/*.mo',
						'RTTR/languages/*.po',
						'RTTR/languages/*.pot',
						'RTTR/REPLAYS/*.rpl',
						'RTTR/sound.lst',
						'RTTR/settings.bin',
						'RTTR/MUSIC',
					],
					[ # create
						'put your S2-Installation in here'
					]
				]
			],
			[ # archives
				'zip',
			]
		),
		(
			[ 'all' ],
			[ # paths
				[
					'',
					[ # include
						'RTTR/texte/readme.txt',
						'RTTR/texte/keyboardlayout.txt',
					],
					[ # svn export
					],
					[ # exclude
					],
				],
			],
			[ # archives
			]
		)
	],
)

###############################################################################
# EOF
###############################################################################
