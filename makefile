PREFIX      = 	m68k-atari-tos

PUREC   	 = 	./dist/purec/$(PREFIX)-purec-pcc \
				./dist/purec/$(PREFIX)-purec-cpp \
				./dist/purec/$(PREFIX)-purec-pasm \
				./dist/purec/$(PREFIX)-purec-bgiobj \
				./dist/purec/$(PREFIX)-purec-dispobj \
				./dist/purec/$(PREFIX)-purec-hc \
				./dist/purec/$(PREFIX)-purec-plink

DEVPAC   	 = 	./dist/devpac/$(PREFIX)-devpac-clink \
				./dist/devpac/$(PREFIX)-devpac-gen \
				./dist/devpac/$(PREFIX)-devpac-crsplit \
				./dist/devpac/$(PREFIX)-devpac-strip 

all:		$(PUREC) $(DEVPAC)


./dist/purec/$(PREFIX)-purec-pcc:
			make -f makefile.single TOS_BINARY="./abin/purec/PCC.TTP" NAME=$@
./dist/purec/$(PREFIX)-purec-cpp:
			make -f makefile.single TOS_BINARY="./abin/purec/CPP.TTP" NAME=$@
./dist/purec/$(PREFIX)-purec-pasm:
			make -f makefile.single TOS_BINARY="./abin/purec/PASM.TTP" NAME=$@
./dist/purec/$(PREFIX)-purec-bgiobj:
			make -f makefile.single TOS_BINARY="./abin/purec/BGIOBJ.TTP" NAME=$@
./dist/purec/$(PREFIX)-purec-dispobj:
			make -f makefile.single TOS_BINARY="./abin/purec/DISPOBJ.TTP" NAME=$@
./dist/purec/$(PREFIX)-purec-hc:
			make -f makefile.single TOS_BINARY="./abin/purec/HC.TTP" NAME=$@
./dist/purec/$(PREFIX)-purec-plink:
			make -f makefile.single TOS_BINARY="./abin/purec/PLINK.TTP" NAME=$@


./dist/devpac/$(PREFIX)-devpac-clink:
			make -f makefile.single TOS_BINARY="./abin/devpac/bin/clink.ttp" NAME=$@
./dist/devpac/$(PREFIX)-devpac-gen:
			make -f makefile.single TOS_BINARY="./abin/devpac/bin/gen.ttp" NAME=$@
./dist/devpac/$(PREFIX)-devpac-crsplit:
			make -f makefile.single TOS_BINARY="./abin/devpac/bin/srsplit.ttp" NAME=$@
./dist/devpac/$(PREFIX)-devpac-strip:
			make -f makefile.single TOS_BINARY="./abin/devpac/bin/strip.ttp" NAME=$@

# Remove all build files
clean:
		rm -f $(PUREC) $(DEVPAC)
		make -f makefile.single clean
