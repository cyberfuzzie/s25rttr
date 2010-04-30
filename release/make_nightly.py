#!/usr/bin/python
# $Author$
###############################################################################

import os, time, socket, sys, glob

os.environ['TZ'] = 'Europe/Berlin'
time.tzset()

###############################################################################

debug     = int(os.environ.get('DEBUG', 0))
buildfarm = os.environ.get('BUILDFARM', '/srv/buildfarm')
target    = os.environ.get('TARGET',    '/www/ra-doersch.de/nightly')

try: os.makedirs(buildfarm)
except: pass
try: os.makedirs(target)
except: pass

###############################################################################

projects  = [ 
	's25client' 
]

systems = [
	#[ 'linux',   True,  [ 'i386', 'x86_64' ],                     []                       ],
	#[ 'apple',   True,  [ 'i386', 'x86_64', 'ppc', 'universal' ], [ '--disable-arch=ppc' ] ],
	#[ 'windows', True,  [ 'i386', 'x86_64' ],                     []                       ],
	[ 'music',   False, [],                                       []                       ],
	[ 'common',  False, [],                                       []                       ],
]

flags = [
	'-j 3'
]

###############################################################################

def cycle_log(file):
	try: os.makedirs(os.path.dirname(file))
	except: pass

	oldfile = file.replace(".log", "_old.log");

	try: os.remove(oldfile)
	except: pass
	try: os.rename(file, oldfile)
	except: pass

	return open(file, "a")

###############################################################################

def log_msg(log, msg):
	if debug == 1 or log == None:
		print msg
	if log != None:
		print >> log, msg
		log.flush()

###############################################################################

def run_command(log, command):
	global debug
	
	logger = ""
	if log != None:
		if debug == 1:
			logger = " | tee %s" % log.name
		else:
			logger = " >> %s" % log.name

	log_msg(log, "Executing \"%s\"" % command)
	ret = os.system(command + logger)
	if ret != 0:
		log_msg(log, "Error: \"%s\" returned %d" % (command, socket.ntohs(ret)))
		return False
		
	return True

###############################################################################

def configure(log, arch, params):
	log_msg(log, "Configuring %s" % arch)
	
	return run_command(log, "../build/cmake.sh --prefix=. --arch=c.%s %s" % (arch, params))

###############################################################################

def make(log, arch):
	global flags

	log_msg(log, "Building %s" % arch)
	
	make_flags = " ".join(flags)
	return run_command(log, "make %s" % make_flags)

###############################################################################

def pack(log, arch):
	log_msg(log, "Packing %s" % arch)

	if arch == "

	return False

###############################################################################

last_error_log = ""

def build_all():
	global projects
	global systems
	global last_error_log
	
	for project in projects:
		for system in systems:
			project_dir = "%s/%s" % (buildfarm, project)
			project_log = "%s/%s/build_%%s.log" % (target, project)
			
			os.chdir(project_dir)

			if system[1]:
				for architecture in system[2]:
					arch = "%s.%s" % (system[0], architecture)
					build_dir = "%s/build_%s" % (project_dir, arch)
					
					if os.path.exists(build_dir):
						os.chdir(build_dir)

						build_log = project_log % arch
						log = cycle_log(build_log)
	
						params = " ".join(system[3])
						if not configure(log, arch, params):
							last_error_log = build_log
							return False
							
						if not make(log, arch):
							last_error_log = build_log
							return False
	
						if not pack(log, arch):
							last_error_log = build_log
							return False
						
						log_msg(log, "Finished %s" % arch)
						log_msg(log, "-------------------------------------------------------------------------------")
			
			else:
				arch = "%s" % system[0]
				build_log = project_log % arch
				
				log = cycle_log(build_log)
				
				if not pack(log, arch):
					last_error_log = build_log
					return False

				log_msg(log, "Finished %s" % arch)
				log_msg(log, "-------------------------------------------------------------------------------")
	return True

###############################################################################

print >> sys.stderr, "Build started %s" % time.strftime("%X %x")

if not build_all():
	print >> sys.stderr, "Build failed %s" % time.strftime("%X %x")
	#if last_error_log != "":
	#	run_command(None, 'cat %s | /usr/bin/mail -s "Nightly Build Failed" -c bugs@siedler25.org sf-team@siedler25.org' % last_error_log)
else:
	print >> sys.stderr, "Build finished %s" % time.strftime("%X %x")
	
###############################################################################
# EOF
###############################################################################