#!/usr/bin/python
# $Author$
###############################################################################

import os, time, socket, sys, glob, threading, shutil

os.environ['TZ'] = 'Europe/Berlin'
time.tzset()

###############################################################################

debug     = int(os.environ.get('DEBUG', 0))
buildfarm = os.environ.get('BUILDFARM', '/srv/buildfarm')
target    = os.environ.get('TARGET',    '/www/ra-doersch.de/nightly')
uploads   = os.environ.get('UPLOADS',    '/srv/buildfarm/uploads_new')
unknown_version = [ 'unknown', '0' ]

try: os.makedirs(buildfarm)
except: pass
try: os.makedirs(target)
except: pass
try: os.makedirs(uploads)
except: pass

###############################################################################

projects  = [ 
	's25client' 
]

systems = [
	#[ 'linux',   [ 'i386', 'x86_64' ],                     []                       ],
	#[ 'apple',   [ 'i386', 'x86_64', 'ppc', 'universal' ], [ '--disable-arch=ppc' ] ],
	[ 'windows', [ 'i386', 'x86_64' ],                     []                       ],
	[ 'common',  [ 'linux', 'apple', 'windows' ],                                []                       ],
	[ 'music',   [ 'linux', 'apple', 'windows' ],                                []                       ],
]

flags = [
	'-j 3'
]

tools = [
	(
		'zip',
		'.zip',
		'zip -r $TARGET $SOURCE -x .svn',
	),
	(
		'tar',
		'.tar.bz2',
		'tar --exclude=.svn -cvjf $TARGET $SOURCE'
	)
]

from layout_linux import *
from layout_apple import *
from layout_windows import *
from layout_common import *
from layout_music import *

layouts = [
	layout_linux,
	layout_apple,
	layout_windows,
	layout_common,
	layout_music
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

def run_command(log, command, fail_is_critical = True, verbose = True):
	global debug
	
	logger = ""
	if log != None:
		if debug == 0:
			logger = " >>%s 2>&1" % log.name

	if verbose:
		log_msg(log, "Executing \"%s\"" % command)
	ret = os.system(command + logger)

	if ret != 0:
		if fail_is_critical:
			log_msg(log, "Error: \"%s\" returned %d" % (command, socket.ntohs(ret)))
		return False
		
	return True

###############################################################################

def configure(log, build_dir, arch, params):
	log_msg(log, "Configuring %s" % arch)
	
	os.chdir(build_dir)
	return run_command(log, "../build/cmake.sh --prefix=. --arch=c.%s %s" % (arch, params))

###############################################################################

def make(log, build_dir, arch):
	global flags

	log_msg(log, "Building %s" % arch)
	
	make_flags = " ".join(flags)
	os.chdir(build_dir)
	return run_command(log, "make %s" % make_flags)

###############################################################################

def pack(log, build_dir, arch):
	global layouts
	global unknown_version
	
	log_msg(log, "Packing %s" % arch)
	
	system = arch.split('.')
	#print system
	
	source_dir = os.path.dirname(build_dir)

	try:
		svn_entries =  open(os.path.join(source_dir, '.svn/entries'), "r")
		lines = svn_entries.readlines()

		version = [
			time.strftime("%Y%m%d"),
			lines[3].strip()
		]
	except:
		version = unknown_version

	#print os.getcwd()

	pack_dir = "%s/pack/%s" % (source_dir, arch)
	upload_dir = "%s/%s_%s_%s" % (uploads, system[0], "-".join(version), system[1])
	
	retval = True
	
	# cycle pack-dir
	if os.path.exists(pack_dir):
		if run_command(log, "rm -rf %s_old" % pack_dir):
			os.rename(pack_dir, pack_dir + "_old")
		else:
			retval = False

	os.makedirs(pack_dir)

	for arch_of_layout, layout in layouts:
		if not retval:
			break;
			
		if arch_of_layout == system[0]:
			archives = []
			
			# process files
			for arch_of_files, directories, archive_of_files in layout:
				if "all" in arch_of_files or system[1] in arch_of_files:
					archives += archive_of_files
					#print directories
					
					for directory in directories:
						#print directory
						dir = pack_dir
						if len(directory[0]) > 0:
							dir = os.path.join(pack_dir, directory[0])
							print "creating directory %s" % dir
							os.makedirs(dir)
						
						# copy included files/dirs
						try:
							for file in directory[1]:
								#print "Copying file/directory %s" % file
								if not run_command(log, "cp -rv %s %s" % (file, dir), True, False):
									retval = False
									break
						except:
							pass
						
						if not retval:
							break;
						
						# svn export files/dirs
						try:
							for file in directory[2]:
								print "Exporting file/directory %s" % file
								if not run_command(log, "svn export --force %s %s" % (file, os.path.join(dir, os.path.basename(file))), True, False):
									retval = False
									break
						except:
							pass
							
						if not retval:
							break;
						
						# remove excluded files/dirs
						try:
							for file in directory[3]:
								print "Removing file/directory %s" % file
								if not run_command(log, "rm -rf %s" % os.path.join(dir, file), True, False):
									retval = False
									break
						except:
							pass
						
						# create (touch) files/dirs
						try:
							for file in directory[4]:
								if file[-1] == '/':
									print "Creating directory %s" % file
									os.makedirs(os.path.join(dir, file))
								else:
									print "creating file %s" % file
									open(os.path.join(dir, file), "w").close()
						except:
							pass

						if not retval:
							break;
					
					if not retval:
						break;
					
			archives = list(set(archives))
			#print archives
			for tool, suffix, command in tools:
				if tool in archives:
					#print tool

					# are old and new identical, then dont create archive
					if not run_command(log, "diff -qr %s_old %s >/dev/null 2>&1" % (pack_dir, pack_dir), False, False):
						target = upload_dir + suffix
					
						# remove old archive
						try:
							os.remove(target)
						except:
							pass

						command = command.replace("$SOURCE", ".")
						command = command.replace("$TARGET", target)

						#print command

						olddir = os.getcwd()
						os.chdir(pack_dir)

						if not run_command(log, command, True, False):
							retval = False
						
						os.chdir(olddir)
					else:
						log_msg(log, "Already Up-To-Date")

				if not retval:
					break;
		
		if not retval:
			break;

	if not retval:
		log_msg(log, "Error: \"pack\" failed")
	return retval

###############################################################################

last_error_logs = []

###############################################################################

class build_thread( threading.Thread ):
	def __init__(self, log, build_dir, arch, params):
		self.log = log
		self.build_dir = build_dir
		self.arch = arch
		self.params = params
		self.retval = False
		threading.Thread.__init__(self)
	
	def run(self):
		global last_error_logs

		log_msg(self.log, "-------------------------------------------------------------------------------")
		log_msg(self.log, "Start %s" % self.arch)
		
		source_dir = os.path.dirname(self.build_dir)

		if os.path.exists(self.build_dir):
			os.chdir(self.build_dir)

			if not configure(self.log, self.build_dir, self.arch, self.params):
				last_error_logs.append(self.log.name)
				self.retval = False
				return False
			
			if not make(self.log, self.build_dir, self.arch):
				last_error_logs.append(self.log.name)
				self.retval = False
				return False
		
		else:
			os.chdir(source_dir)

		if not pack(self.log, self.build_dir, self.arch):
			last_error_logs.append(self.log.name)
			self.retval = False
			return False
			
		log_msg(self.log, "Finished %s" % self.arch)
		self.retval = True
		return True
		
	def finish(self):
		if self.retval:
			log_msg(self.log, "Finished %s" % self.arch)
		else:
			log_msg(self.log, "Failed %s" % self.arch)
		log_msg(self.log, "-------------------------------------------------------------------------------")
		return self.retval

###############################################################################

def build_all():
	global projects
	global systems
	global last_error_logs
	
	retval = True
	
	for project in projects:
		project_dir = "%s/%s" % (buildfarm, project)
		project_log = "%s/%s/build_%%s.log" % (target, project)
		
		all_log = cycle_log(project_log % "all")
		log_msg(all_log, "-------------------------------------------------------------------------------")
		log_msg(all_log, "Start build_all")

		os.chdir(project_dir)
		
		retval_proj = True
		
		# update project
		if True == False:#not run_command(all_log, "svn up %s" % project_dir):
			last_error_logs.append(all_log.name)
			retval_proj = False
		else:
			threads = []
			
			# configure, make, pack
			for system in systems:
				for architecture in system[1]:
					arch = "%s.%s" % (system[0], architecture)
					build_dir = "%s/build_%s" % (project_dir, arch)
					
					arch_log = cycle_log(project_log % arch)

					params = " ".join(system[2])
					
					thread = build_thread(arch_log, build_dir, arch, params)
					threads.append(thread)
					thread.start()
					if debug:
						thread.join()
	
			for t in threads:
				t.join()
				if not t.finish():
					retval_proj = False

		if retval_proj:
			log_msg(all_log, "Finished build_all")
		else:
			log_msg(all_log, "Failed build_all")
			retval = False
		log_msg(all_log, "-------------------------------------------------------------------------------")

	return retval

###############################################################################

print >> sys.stderr, "Build started %s" % time.strftime("%X %x")

if not build_all():
	print >> sys.stderr, "Build failed %s" % time.strftime("%X %x")
	#if len(last_error_logs) > 0:
	if False:
		# Import smtplib for the actual sending function
		import smtplib

		# Here are the email package modules we'll need
		from email.mime.text import MIMEText
		from email.mime.multipart import MIMEMultipart
		
		mail_from = 'sf-team@siedler25.org'
		mail_to = [ 'bugs@siedler25.org', 'sf-team@siedler25.org' ]
		
		# Create the container (outer) email message.
		msg = MIMEMultipart()
		msg['Subject'] = 'Nightly Build Failed'
		# me == the sender's email address
		# family = the list of all recipients' email addresses
		msg['From'] = mail_from
		msg['To'] = ", ".join(mail_to)
		
		msg.attach(MIMEText("Nightly Build Failed\nThe logs of the failed builds are attached\n"))
		
		# Assume we know that the image files are all in PNG format
		for file in last_error_logs:
			# Open the files in binary mode.  Let the MIMEImage class automatically
			# guess the specific image type.
			fp = open(file, 'rb')
			log = MIMEText(fp.read())
			fp.close()
			log.add_header('Content-Disposition', 'attachment', filename=os.path.basename(file))
			msg.attach(log)

		
		# Send the email via our own SMTP server.
		s = smtplib.SMTP("localhost")
		s.sendmail(mail_from, mail_to, msg.as_string())
		s.quit()
else:
	print >> sys.stderr, "Build finished %s" % time.strftime("%X %x")
	
###############################################################################
# EOF
###############################################################################
