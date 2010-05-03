###############################################################################
# $Id$
# $Author$
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
