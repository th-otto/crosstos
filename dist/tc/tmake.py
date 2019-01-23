#!/usr/bin/python

import sys, getopt, string
import collections


def inbrackets(line, open, close):

	opt = line[line.find(open)+1:line.find(close)]

	return opt

def project(fname, libpath, includes):

	name   = ""

	pobjs  = list()
	objs   = list()
	libs   = list()

	modules = 0
	deps   = ""
	parms  = ""

	items = collections.OrderedDict()
	item  = ""

	items['.C'] = dict({ 'Dependencies': "", 'Parameters': ""})
	items['.S'] = dict({ 'Dependencies': "", 'Parameters': ""})
	items['.L'] = dict({ 'Dependencies': "", 'Parameters': ""})

	mode = 0

	with open(fname) as f:

		content = f.readlines()

		for line in content:

			# Strip whitespaces, convert to upper case, and remove comments
			line = line.strip().upper().split(";", 1)[0]

			if(line.startswith("*")):
				if(modules == 0):
					line = "output.tos"
				else:
					line = "default.c"
			elif(line.startswith("=")):
				name = item
				modules = 1

			if(line != ""):
				if(mode == 0):
					items[item] = dict({ 'Dependencies': map(str.strip, deps.split(",")),
										 'Parameters': map(str.strip, parms.split()) } )

					item  = line.upper().split()[0].replace('\\', '/')
					deps  = ""
					parms = ""

				if("(" in line):
					if(")" in line):
						deps = inbrackets(line, "(", ")")
					else:
						deps = line.split("(", 1)[1]
						mode = 1
				elif(")" in line):
					deps = deps + line.split(")", 1)[0]
					mode = 0
				elif(mode == 1):
					deps = deps + line

				if("[" in line):
					if("]" in line):
						parms = inbrackets(line, "[", "]")
					else:
						parms = line.split("[", 1)[1]
						mode = 2
				elif("]" in line):
					parms = parms + line.split("]", 1)[0]
					mode = 0
				elif(mode == 2):
					parms = parms + line


		if(mode == 0):
			items[item] = dict({ 'Dependencies': map(str.strip, deps.split(",")),
								 'Parameters': map(str.strip, parms.split()) } )


		cflags = ' '.join(items['.C']['Parameters'])
		lflags = ' '.join(items['.L']['Parameters'])
		aflags = ' '.join(items['.S']['Parameters'])

		del items['.C']
		del items['.L']
		del items['.S']

		for key, value in items.iteritems():

			if(key.endswith(".O")):
				pobjs.append("$(LIBPATH)" + key)
			elif(key.endswith(".C") or key.endswith(".S")):
				objs.append(key[:-1] + "O")
			elif(key.endswith(".LIB")):
				libs.append("$(LIBPATH)" + key)

		print "NAME     = " + name
		print "TMP      = __tmpXYZ.txt"
		print "INCLUDES = " + includes
		print "CC       = ./m68k-atari-tos-tc-tcc"
		print "PP       = ./m68k-atari-tos-tc-cpp"
		print "AS       = ./m68k-atari-tos-tc-tasm"
		print "LD       = ./m68k-atari-tos-tc-tlink"
		print "CFLAGS   = " + cflags + " $(INCLUDES)"
		print "AFLAGS   = " + aflags
		print "LFLAGS   = " + lflags
		print "LIBPATH  = " + libpath
		print "PREBUILT = " + ' '.join(pobjs)
		print "OBJECTS  = " + ' '.join(objs)
		print "LIBS     = " + ' '.join(libs)
		print ""
		print "all: $(NAME)"
		print ""
		print "$(NAME): $(PREBUILT) $(OBJECTS)"
		print "\t@echo $(PREBUILT) $(OBJECTS) $(LIBS) >$(TMP)"
		print "\t$(LD) $(LDFLAGS) -C=$(TMP) -O=$@"
		print "\t@rm -f $(TMP)"
		print "\t@echo Done."
		print ""

		for key, value in items.iteritems():
			if(key.endswith(".C")):
				print key[:-1] + "O" + ": " + ' '.join(value['Dependencies'])
				print "\t$(CC) $(CFLAGS) " + ' '.join(value['Parameters'])+ " -O$@ " + key
				print ""
			elif(key.endswith(".S")):
				print key[:-1] + "O" + ": " + ' '.join(value['Dependencies'])
				print "\t$(AS) $(AFLAGS) " + ' '.join(value['Parameters'])+ " -O$@ " + key
				print ""

		print "clean:"
		print "\t rm -f $(OBJECTS) $(NAME) $(TMPOBJ)"
		print ""

def main(argv):
  
	try:
		opts, args = getopt.getopt(argv,"h")

	except getopt.GetoptError:
		print "Usage: pmake [options] [target] ..."
		sys.exit(2)

	for opt,arg in opts:
		if opt == '-h':
			print "Usage: pmake [options] [target] ..."
			sys.exit()

	for arg in args:
		project(arg, "./LIB/", "-I./INCLUDE")

if __name__== "__main__":
	main(sys.argv[1:])
