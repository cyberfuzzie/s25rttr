#!/bin/bash

. base.inc.sh

cat > /tmp/message.$$ << EOF
	<message>
		<generator>
			<name>Nightly-Build CIA Bot client shell script</name>
			<version>2.0</version>
		</generator>
		<source>
			<project>S25RTTR</project>
		</source>
		<body>
			<commit>
				<revision>${REVISION}</revision>
				<author>${BUILDER}</author>
				<files>$(cat ${WWWDIR}/.files)</files>
				<log>Build for ${VERSION}-${REVISION} successful</log>
			</commit>
		</body>
	</message>
EOF
		
	cat /tmp/message.$$ | mail -s DeliverXML cia@cia.navi.cx
	rm -vf /tmp/message.$$ ${WWWDIR}/.files
fi

## Logs+Links hochladen
scp $WWWDIR/rapidshare.txt root@tyra.ra-doersch.de:/www/siedler25.org/www/uploads
scp $WWWDIR/changelog.txt root@tyra.ra-doersch.de:/www/siedler25.org/www/uploads
scp $WWWDIR/build*.log root@tyra.ra-doersch.de:/www/siedler25.org/www/uploads

exit 0
