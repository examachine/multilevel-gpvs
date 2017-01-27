#
# makefile for egcs
#
# by exa
#

SHELL = /bin/sh

.SUFFIXES:

PROJECT = libgraph

srcdir = .
curdir := $(shell pwd)
parentdir := $(shell cd ..;pwd)
dirname := $(subst $(parentdir)/,,$(curdir))

# Project files

EXEC = vsep
OBJS = Application.o
CXXFILES = Application.cxx Simple_Graph.cxx
CCFILES = 
CFILES =
AFILES =
INCFILES = General.hxx ANSILibrary.hxx Graph.hxx Allocators.hxx \
	Attributes.hxx Graph_Test.hxx Graph_Utility.hxx Array.hxx \
	Simple_Graph.hxx Simple_Graph_Impl.hxx Simple_Graph_Coarsening.hxx \
	Simple_Graph_Partitioning.hxx Simple_Graph_Refinement.hxx
GRAMMARFILES =
LEXFILES =
INCLUDEDIR =
LIBS = -lnana
TEXFILES = report.tex
OTHERDOCFILES = report.bib edgesep_nodesep.eps simple_graph.eps
AUX = Makefile todo.txt graph1 graph2 $(TEXFILES) $(OTHERDOCFILES)
ARCPATH = /home/exa/arc

# Derived project files

IMPLEMENTATION = $(CFILES) $(CXXFILES) $(AFILES) $(GRAMMARFILES) $(LEXFILES)
INTERFACE = $(INCFILES)
SRC = $(IMPLEMENTATION) $(INTERFACE)
VERSUFFIX:=,v
#VER = $(addsuffix $(VERSUFFIX), $(SRC))
ARCHIVE = $(PROJECT).tgz
DVIFILES = $(addsuffix .dvi, $(basename $(TEXFILES)))
LIBDIR = 
PROG = -o $(EXEC)

# Options

GENERAL_DEBUG = -D NDEBUG #-D DGN3 # 
GENERAL_OPTIMISE = -O3
GENERAL_WARN = -Wcomment -Wimplicit -Wmain -Wswitch
CPPFLAGS =
CXXOPTIMISE =
CXXDEBUG =
CXXWARN = -ftemplate-depth-64 #-Wtemplate-debugging -Weffc++
CXXOTHER = -foperator-names
CXXFLAGS = $(GENERAL_DEBUG) $(GENERAL_OPTIMISE) $(GENERAL_WARN)\
	   $(CXXDEBUG) $(CXXOPTIMISE) $(CXXWARN) $(CXXOTHER)
COPTIMISE =
CDEBUG =
CWARN =
COTHER = 
CFLAGS = $(GENERAL_DEBUG) $(GENERAL_OPTIMISE) $(GENERAL_WARN)\
	 $(CDEBUG) $(COPTIMISE) $(CWARN) $(COTHER)
ASFLAGS =
LDFLAGS = $(GENERAL_DEBUG)

# Programs used

MAKE =	make # For subsequent make's
CC =	egcc # C compiler
CXX =	g++  # C++ compiler
ASM =	as   # Assembler
LD = 	g++  # Linker
BISON =	bison --defines
FLEX =	flex
LS =	ls -l
CP =    cp --interactive
ED =	emacs
LATEX = latex
BIBTEX = bibtex

# PAGER = most

# Rules

# Pattern Rules

# Automatic dependency rules

# C/C++

%.d: %.cxx
	@echo Generating dependencies for $*.cxx
	@$(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< \
                 | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'
%.d: %.cc
	@echo Generating dependencies for $*.cc
	@$(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< \
                 | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'
%.d: %.c
	@echo Generating dependencies for $*.c
	@$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
                 | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'
%.d: %.
	@echo Generating dependencies for $*.c
	@$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
                 | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'

# Compilation rules

%.o : %.cxx
	@echo Compiling $<
	@$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@
%.o : %.cc
	@echo Compiling $<
	@$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@
%.o : %.c
	@echo Compiling $<
	@$(CC) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@
%.dvi : %.tex
	@echo Compiling latex to dvi $<
	@$(LATEX) $<

# Flex - Bison rules

%.tab.c %.tab.h : %.grammar %.hxx
	$(BISON) $< --output-file=$*.tab.c

%.flex.c : %.flex %.tab.h %.hxx
	$(FLEX) -o$@ $<

# Phony Targets

.PHONY: all defaultmesg compileinfo clean realclean list listsrc \
	arc listarc extractarc rmarc backup restore

# Default rule

all: defaultmesg compileinfo $(EXEC)

defaultmesg:
	@echo Default rule invoked, building everything

compileinfo:
	@echo Project name: $(PROJECT) 
	@echo C preprocessor flags: $(CPPFLAGS)
	@echo C flags: $(CFLAGS)
	@echo C++ flags: $(CXXFLAGS)

$(EXEC): $(OBJS) $(DVIFILES)
	@echo Building executable: $(EXEC)
	@echo LD flags: $(LDFLAGS)
	@echo Libraries used: $(LIBS)
	@$(LD) $(LDFLAGS) $(LIBDIR) $(INCLUDEDIR) $(OBJS) $(LIBS) $(PROG)

# Include automatic dependencies
#ifdef $(CXXFILES)
include $(CXXFILES:.cxx=.d)
#endif

ifdef $(CCFILES)
include $(CCFILES:.cc=.d)
endif

ifdef $(CFILES)
include $(CFILES:.c=.d)
endif

clean:
	@echo Deleting object modules and executables
	@-rm -f $(OBJS) $(EXEC)

distclean: clean
	@echo Deleting archive
	@-rm -f $(ARCHIVE)

realclean: distclean
	@echo Deleting miscallenous files
	@-rm -f *.bak *~

list:
	$(LS) $(SRC) $(AUX)

listsrc:
	$(LS) $(SRC)

# Archive

arc:	$(ARCHIVE)

listarc:
	@echo Listing content of tar archive
	@tar tzvf $(ARCHIVE) 

extractzip:
	@cd .. ;\
	echo Extracting from tar archive ;\
	tar xvf $(dirname)/$(ARCHIVE)

updatearc:
	@cd .. ;\
	echo Updating tar archive ;\
	tar uzvf $(dirname)/$(ARCHIVE) $(addprefix $(dirname)/,$?)

$(ARCHIVE): $(SRC) $(VER) $(AUX)
	@cd .. ;\
	echo Creating tar archive ;\
	tar czvf $(dirname)/$(ARCHIVE) $(addprefix $(dirname)/,$?)

rmarc:	$(ARCHIVE)
	@-rm -f $(ARCHIVE)

backup:
	@echo Backing up archive
	@$(CP) $(ARCHIVE) $(ARCPATH)

restore:
	@echo Restoring archive
	@$(CP) $(ARCPATH)/$(ARCHIVE) .
