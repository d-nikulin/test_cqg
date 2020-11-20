CC            = gcc
CXX           = g++
#CFLAGS        = -pipe -O2 -fsanitize=address,undefined -std=c++17 -Wextra -Wno-unused-parameter -Wsign-conversion -Wswitch-default -Wswitch-enum -Werror -Wall -W -D_REENTRANT
#CXXFLAGS      = -pipe -O2 -fsanitize=address,undefined -std=c++17 -Wextra -Wno-unused-parameter -Wsign-conversion -Wswitch-default -Wswitch-enum -Werror -Wall -W -D_REENTRANT
CFLAGS        = -fPIC -pipe -O2 -Wextra -Wno-unused-parameter -Wsign-conversion -Wswitch-default -Wswitch-enum -Werror -Wall -W -D_REENTRANT
CXXFLAGS      = -fPIC -pipe -O2 -Wextra -Wno-unused-parameter -Wsign-conversion -Wswitch-default -Wswitch-enum -Werror -Wall -W -D_REENTRANT
INCPATH       = -Iinclude -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS) -lrt
#LIBS          = $(SUBLIBS) -lasan -lubsan -lrt 
AR            = ar cqs
RANLIB        = 
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = SpellChecker.cpp \
				FileParser.cpp \
				main.cpp

OBJECTS       = SpellChecker.o \
				FileParser.o \
				main.o

DESTDIR       = bin/
TARGET        = $(DESTDIR)spellchecker

first: all

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	@$(CHK_DIR_EXISTS) $(DESTDIR) || $(MKDIR) $(DESTDIR) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

compiler_clean: 

####### Compile


SpellChecker.o: SpellChecker.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -c -o SpellChecker.o SpellChecker.cpp

FileParser.o: FileParser.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -c -o FileParser.o FileParser.cpp

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -c -o main.o main.cpp



####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

