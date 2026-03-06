OS := $(shell uname -s)

ifeq ($(findstring MINGW64, $(OS)), MINGW64)
	OS = Windows
else ifeq ($(findstring inux, $(OS)), inux)
	OS = Linux
else ifeq ($(findstring arwin, $(OS)), arwin)
	OS = macOS
else
$(error Unknow operating system $(OS))
endif

CXXFLAGS +=

ifeq ($(OS), Windows)
	CXX      = g++
	CC       = gcc
	CXXFLAGS = -D__USE_MINGW1_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 -fdiagnostics-plain-output -std=c++17 -W -Wall
	CFLAGS   = -D__USE_MINGW1_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 -fdiagnostics-plain-output -W -Wall
	LDFLAGS  = -static
else ifeq ($(OS), macOS)
	CXX      = clang++
	CC       = clang
	CXXFLAGS = -std=c++17 -W -Wall
	CFLAGS   = -W -Wall
	LDLAGS   =
else
	CFLAGS   = -W -Wall
	CXX      = g++
	CC       = gcc
	CXXFLAGS = -fdiagnostics-plain-output -std=c++17 -W -Wall
	CFLAGS   = -fdiagnostics-plain-output -W -Wall
	LDLAGS   =
endif

ifeq ($(build), sanitize)
	CXXFLAGS += -fsanitize=address -O0 -g -DDEBUG
	CFLAGS   += -O0 -g -DDEBUG
	LDFLAGS  += -fsanitize=address
else ifeq ($(build), release)
	CXXFLAGS += -O2 -DNDEBUG -Wno-unused-result
	CFLAGS   += -O2 -DNDEBUG
	CXXFLAGS += -O0 -g -DDEBUG
	CFLAGS   += -O0 -g -DDEBUG
	LDFLAGS  +=
else
	CXXFLAGS += -O0 -DDEBUG
	CFLAGS   += -O0 -DDEBUG
	LDFLAGS  +=
endif

FLTK_CXXFLAGS       = $(shell /usr/local/bin/fltk-config --cxxflags)
FLTK_LDFLAGS        = $(shell /usr/local/bin/fltk-config --ldflags)
FLTK_IMAGE_LDFLAGS  = $(shell /usr/local/bin/fltk-config --ldflags --use-images)
CXXFLAGS           += $(FLTK_CXXFLAGS)
LDFLAGS            += $(FLTK_IMAGE_LDFLAGS)
INC                 = -I..

$(info OS                 = $(OS))
$(info CXX                = $(CXX))
$(info CC                 = $(CC))
$(info CXXFLAGS           = $(CXXFLAGS))
$(info CFLAGS             = $(CFLAGS))
$(info LDFLAGS            = $(LDFLAGS))
$(info FLTK_CXXFLAGS      = $(FLTK_CXXFLAGS))
$(info FLTK_LDFLAGS       = $(FLTK_LDFLAGS))
$(info FLTK_IMAGE_LDFLAGS = $(FLTK_IMAGE_LDFLAGS))
$(info $())

#-------------------------------------------------------------------------------

all: flw.o \
	file.exe \
	json.exe \
	chart.exe \
	dialog.exe \
	gridgroup.exe \
	inputmenu.exe \
	lcddisplay.exe \
	plot.exe \
	recentmenu.exe \
	splitgroup.exe \
	table.exe \
	tableeditor.exe \
	tabsgroup.exe \
	toolgroup.exe \
	settingsdialog.exe \

#-------------------------------------------------------------------------------

flw.o: ../flw.cpp ../flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

file.exe: file.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

json.exe: json.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

chart.exe: chart.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

dialog.exe: dialog.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

fontdialog.exe: fontdialog.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

gridgroup.exe: gridgroup.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

inputmenu.exe: inputmenu.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

lcddisplay.exe: lcddisplay.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

logdisplay.exe: logdisplay.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

plot.exe: plot.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

recentmenu.exe: recentmenu.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

settingsdialog.exe: settingsdialog.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

splitgroup.exe: splitgroup.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

table.exe: table.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

tableeditor.exe: tableeditor.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

tabsgroup.exe: tabsgroup.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

toolgroup.exe: toolgroup.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

#-------------------------------------------------------------------------------

clean:
	rm -f *.o *.exe *.ps

#-------------------------------------------------------------------------------

file: file.exe
	./file.exe

json: json.exe
	./json.exe

chart: chart.exe
	./chart.exe

dialog: dialog.exe
	./dialog.exe

gridgroup: gridgroup.exe
	./gridgroup.exe

inputmenu: inputmenu.exe
	./inputmenu.exe

lcddisplay: lcddisplay.exe
	./lcddisplay.exe

logdisplay: logdisplay.exe
	./logdisplay.exe

plot: plot.exe
	./plot.exe

recentmenu: recentmenu.exe
	./recentmenu.exe

settingsdialog: settingsdialog.exe
	./settingsdialog.exe

splitgroup: splitgroup.exe
	./splitgroup.exe

table: table.exe
	./table.exe

tableeditor: tableeditor.exe
	./tableeditor.exe

tabsgroup: tabsgroup.exe
	./tabsgroup.exe

toolgroup: toolgroup.exe
	./toolgroup.exe
