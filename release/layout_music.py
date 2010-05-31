###############################################################################
# $Id: layout_music.py 6458 2010-05-31 11:38:51Z FloSoft $
# $Author: FloSoft $
###############################################################################

layout_music = (
	'music',
	[ # architectures
		(
			[ 'linux' ],
			[ # paths
				[
					'share/s25rttr/RTTR',
					[ # include
					],
					[ # svn export
						'RTTR/MUSIC'
					]
				]
			],
			[ # archives
				'tar',
			]
		),
		(
			[ 'apple' ],
			[ # paths
				[
					's25client.app/Contents/MacOS/share/s25rttr/RTTR',
					[ # include
					],
					[ # svn export
						'RTTR/MUSIC'
					]
				]
			],
			[ # archives
				'tar',
			]
		),
		(
			[ 'windows' ],
			[ # paths
				[
					'RTTR',
					[ # include
					],
					[ # svn export
						'RTTR/MUSIC'
					]
				]
			],
			[ # archives
				'zip',
			]
		),
	],
)

###############################################################################
# EOF
###############################################################################
