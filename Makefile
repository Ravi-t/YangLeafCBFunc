## 
#@file Makefile
#@author Ravi Tandon
#@brief Makefile for generating source files and compiling sample_app for sysrepo callbacks for respective YANG model leafs


# Binary name
TARGET = yangApp

# Library name
LIB = libyangcb.a

CC = gcc

YANG_MODULE_NAME = sample_moduleconfig   #Mention the name of your YANG module here
YANG_C_CALLBACK_GEN = ./yangCCallBackGen
XPATH_GEN = ./genXpath
XPATH_FILE = xpath.txt

CMN_LIB_PATH = -L/usr/local/lib64

# compile time flags
CFLAGS = -g -Wall -std=c99

# linking flags
LFLAGS   = -I./h -l sysrepo

OBJDIR = obj
SRCDIR = src
HDRDIR = h
LIBDIR = libs
UTILDIR = utils

GEN_SRC_FILES = *gen.c
SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(HDRDIR)/*.h)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)                 

AR = ar rcs
MKDIR  = mkdir -p
MV = mv
RM = rm -f
CD = cd
CHMOD = chmod +x

##help  - Shows help.
help:
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep |sed -e 's/\\$$//' | sed -e 's/##//'

##gen   - Pre-requisite for lib/bin/all options. Generates hdr/src files.
gen:	 
	@echo "Generating Header & Source Files..."
	@$(MKDIR) $(HDRDIR) $(LIBDIR) $(OBJDIR); 
	@$(CD) $(UTILDIR); \
	$(CHMOD) $(XPATH_GEN) $(YANG_C_CALLBACK_GEN); \
	$(XPATH_GEN) $(YANG_MODULE_NAME) > $(XPATH_FILE);\
	$(YANG_C_CALLBACK_GEN) $(XPATH_FILE)
	@echo "[Done]"

##lib   - Generates a static library having Callback functions.
lib: $(LIB)	

##bin   - Generates the sample binary executable.
bin: $(TARGET)

##all   - Compiles all source files to comeup with a library and an executable.
all: lib bin


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(CMN_LIB_PATH) $(LFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(LFLAGS) -c $< -o $@

$(LIB): $(OBJECTS)
	$(AR) $(LIBDIR)/$@ $(OBJECTS)


.PHONY: clean

##clean - Removes all generated files including objs, library and executable.
clean:
	@echo "Cleaning Files..."
	@$(RM) $(OBJECTS)
	@$(RM) $(HDRDIR)/*
	@$(RM) $(SRCDIR)/$(GEN_SRC_FILES)
	@$(RM) $(LIBDIR)/*
	@$(RM) $(TARGET)
	@echo "[Done]"
