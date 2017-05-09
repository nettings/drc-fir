# DRC base makefile

# DRC version
VERSION=3.2.1

# Where to install
INSTALL_PREFIX=$(DESTDIR)/usr/local

# Compiler
CC=g++

# Standard optimization settings
# Tested also on Mac OS X
# CFLAGS=-O -s -I.

# Standard optimization settings, for systems missing getopt
# CFLAGS=-O -s -I. -I./getopt

# i686 optimizaztions settings
# CFLAGS=-march=pentium3 -O -mfancy-math-387 -msse -mtune=pentium3 -I. 

# Pentium 3 or greater optimizaztions settings with SSE support
# Be careful: -mfpmath=sse sometimes causes accuracy problems
CFLAGS=-march=pentium3 -O -s -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -I. 

# Same as above, but tested on recent Mac OS X
# Be careful: -mfpmath=sse sometimes causes accuracy problems
# CFLAGS=-march=pentium3 -O -s -m32 -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -I. 

# Athlon64 settings
# CFLAGS=-march=athlon64 -O3 -s -fPIC -I. 

# Vectorization settings (GCC 4 Only, experimental)
# CFLAGS=-march=pentium3 -O3 -s -pg -g -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -ftree-vectorize -ftree-vectorizer-verbose=2 -I. 

# GLSweep optimization settings
# GLSweep doesn't need SSE based optimization settings
GLSCFLAGS=-O -s -I.

# Debug settings

# Warning check pedantic settings
# CFLAGS=-Wall -pedantic -I.

# Profiling settings
# CFLAGS=-march=pentium3 -O -pg -g -mfancy-math-387 -msse -mfpmath=sse -mtune=pentium3 -I. 

# DRC sources
DRCSRC=baselib.cpp drc.cpp fft.cpp hd.cpp slprefilt.cpp bwprefilt.cpp convol.cpp dspwind.cpp spline.cpp fir.cpp level.cpp toeplitz.cpp kirkebyfd.cpp drccfg.cpp psychoacoustic.cpp cmdline.cpp cfgparse.cpp fftsg.c gsl/gsl_fft.c gsl/error.c gsl/stream.c minIni/minIni.c

# DRC sources for systems missing getopt
# DRCSRC=baselib.cpp drc.cpp fft.cpp hd.cpp slprefilt.cpp bwprefilt.cpp convol.cpp dspwind.cpp spline.cpp fir.cpp level.cpp toeplitz.cpp kirkebyfd.cpp drccfg.cpp psychoacoustic.cpp cmdline.cpp cfgparse.cpp fftsg.c gsl/gsl_fft.c gsl/error.c gsl/stream.c minIni/minIni.c getopt/getopt.c getopt/getopt1.c

# GLSweep sources
GLSWEEPSRC=glsweep.c

# LSConv sources
LSCONVSRC=lsconv.c fftsg_h.c

# Compiled objects
OBJS=drc lsconv glsweep

# Installation definitions
CONFIG_TARGETS=config/*
TARGET_TARGETS=target/*
MIC_TARGETS=mic/*
IMPULSE_TARGETS=../sample/rs.pcm
DOC_TARGETS=readme.txt ../doc/text/drc.txt
BIN_TARGETS=drc lsconv glsweep

# Targets

ALL: drc glsweep lsconv

drc: $(DRCSRC)
	$(CC) $(CFLAGS) -lm -lstdc++ -o drc $(DRCSRC)

glsweep: $(GLSWEEPSRC)
	$(CC) $(GLSCFLAGS) -lm -o glsweep $(GLSWEEPSRC)
	
lsconv: $(LSCONVSRC)
	$(CC) $(CFLAGS) -lm -o lsconv $(LSCONVSRC)
	
clean:
	rm -f $(OBJS)

install: $(CONFIG_TARGETS) $(TARGET_TARGETS) $(MIC_TARGETS) $(IMPULSE_TARGETS) $(DOC_TARGETS) $(BIN_TARGETS)
	install $(BIN_TARGETS) $(INSTALL_PREFIX)/bin
	install -d $(INSTALL_PREFIX)/share/drc/config/
	install $(CONFIG_TARGETS) $(INSTALL_PREFIX)/share/drc/config/
	install -d $(INSTALL_PREFIX)/share/drc/target/
	install $(TARGET_TARGETS) $(INSTALL_PREFIX)/share/drc/target/
	install -d $(INSTALL_PREFIX)/share/drc/mic/
	install $(MIC_TARGETS) $(INSTALL_PREFIX)/share/drc/mic/
	install -d $(INSTALL_PREFIX)/share/drc/impulse/
	install $(IMPULSE_TARGETS) $(INSTALL_PREFIX)/share/drc/impulse/
	install -d $(INSTALL_PREFIX)/share/drc/filter/
	install -d $(INSTALL_PREFIX)/share/doc/drc-$(VERSION)/
	install $(DOC_TARGETS) $(INSTALL_PREFIX)/share/doc/drc-$(VERSION)/
