#!/usr/bin/python

import sys, getopt

def inbrackets(line):

	opt = line[line.find("[")+1:line.find("]")]

	return opt

def project(fname, libpath, includes):

	name   = "main.tos"

	cflags = ""
	aflags = ""
	lflags = ""

	objs   = ""
	src    = list();
	libs   = list();

	clean  = ""

	modules = 0

	with open(fname) as f:

		content = f.readlines()

		for line in content:

			line = line.strip().upper();

			if(line == ""):
				dummy = 0
			elif(line.startswith(";")):
				dummy = 0
			elif(line.startswith("*")):
				if(modules == 0):
					name   = "main.tos"
				else:
					src.append("main.c")

			elif(line.startswith("=")):
				modules = 1
			elif(line.startswith(".C")):
				cflags = cflags + inbrackets(line).strip() + " "
			elif(line.startswith(".S")):
				aflags = aflags + inbrackets(line).strip() + " "
			elif(line.startswith(".L")):
				lflags = lflags + inbrackets(line).strip() + " "
			else:
				items = line.upper().split()

				if(items[0].endswith(".LIB")):
					libs.append(items[0])
				elif(items[0].endswith(".O")):
					objs = objs + libpath + items[0] + " "
				elif(items[0].endswith(".C")):
					src.append(items[0])
				else:
					if(modules == 0):
						name = items[0]
					else:
						print "Unknown syntax: " + line

		for file in src:
			objs = objs + file[:-1] + "o "

		clean = clean + name + " "

		for file in src:
			clean = clean + file[:-1] + "o "

		print "NAME     = " + name
		print "TMPOBJ   = __tmpXYZ.o"
		print "INCLUDES = " + includes
		print "CC       = ./m68k-atari-tos-purec-pcc"
		print "PP       = ./m68k-atari-tos-purec-cpp"
		print "AS       = ./m68k-atari-tos-purec-pasm"
		print "LD       = ./m68k-atari-tos-purec-plink"
		print "CFLAGS   = " + cflags
		print "AFLAGS   = " + aflags
		print "LFLAGS   = " + lflags
		print "OBJECTS  = " + objs
		print ""
		print "all: $(NAME)"
		print ""
		print "$(NAME): $(OBJECTS)"

		print "\t$(LD) $(LDFLAGS) -J -O=$(TMPOBJ) $(OBJECTS)" 

		for lib in libs:
			print "\t$(LD) $(LDFLAGS) -J -O=$(TMPOBJ) $(TMPOBJ) " + libpath + lib 

		print "\t$(LD) $(LDFLAGS) -O=$@ $(TMPOBJ)" 
		print "\trm -f $(TMPOBJ)"

		print ""
		print ".c.o:"
		print "\t$(CC) $(CFLAGS) -O$@ $<"
		print ""
		print "clean:"
		print "\t rm -f " + clean + " $(TMPOBJ)"

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
		project(arg, "./LIB/", "-I./INCLUDES")

if __name__== "__main__":
	main(sys.argv[1:])
