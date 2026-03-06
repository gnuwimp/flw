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

ifeq ($(build), release)
	CXXFLAGS += -O2 -DNDEBUG -Wno-unused-result
	CFLAGS   += -O2 -DNDEBUG
	LDFLAGS  += 
else
	CXXFLAGS += -O0 -DDEBUG
	CFLAGS   += -O0 -DDEBUG
	LDFLAGS  +=
endif

FLTK_CXXFLAGS      = $(shell /usr/local/bin/fltk-config --cxxflags)
FLTK_LDFLAGS       = $(shell /usr/local/bin/fltk-config --ldflags)
FLTK_IMAGE_LDFLAGS = $(shell /usr/local/bin/fltk-config --ldflags --use-images)

CXXFLAGS += $(FLTK_CXXFLAGS)
LDFLAGS  += $(FLTK_IMAGE_LDFLAGS)
INC       = -Isrc

$(info OS                 = $(OS))
$(info CXX                = $(CXX))
$(info CC                 = $(CC))
$(info CXXFLAGS           = $(CXXFLAGS))
$(info CFLAGS             = $(CFLAGS))
$(info LDFLAGS            = $(LDFLAGS))
$(info $())

ifeq ($(OS), Windows)
	THEME_RESOURCE = theme.res
else ifeq ($(OS), Linux)
else ifeq ($(OS), macOS)
else
endif

all: test_chart.exe \
	test_datechooser.exe \
	test_dlg.exe \
	test_gridgroup.exe \
	test_inputmenu.exe \
	test_lcddisplay.exe \
	test_logdisplay.exe \
	test_plot.exe \
	test_recentmenu.exe \
	test_scrollbrowser.exe \
	test_settingsdialog.exe \
	test_splitgroup.exe \
	test_table.exe \
	test_tabledisplay.exe \
	test_tableeditor.exe \
	test_tabsgroup.exe \
	test_theme.exe \
	test_toolgroup.exe \

$(THEME_RESOURCE): res/theme.rc
	windres res/theme.rc -O coff -o $(THEME_RESOURCE)

flw.o: flw.cpp flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_chart.exe: test/test_chart.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_datechooser.exe: test/test_datechooser.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_dlg.exe: test/test_dlg.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_gridgroup.exe: test/test_gridgroup.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_inputmenu.exe: test/test_inputmenu.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_lcddisplay.exe: test/test_lcddisplay.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_logdisplay.exe: test/test_logdisplay.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_plot.exe: test/test_plot.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_recentmenu.exe: test/test_recentmenu.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_scrollbrowser.exe: test/test_scrollbrowser.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_settingsdialog.exe: test/test_settingsdialog.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_splitgroup.exe: test/test_splitgroup.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_table.exe: test/test_table.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_tabledisplay.exe: test/test_tabledisplay.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_tableeditor.exe: test/test_tableeditor.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_tabsgroup.exe: test/test_tabsgroup.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_theme.exe: test/test_theme.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

test_toolgroup.exe: test/test_toolgroup.cpp flw.o
	$(CXX) -o $@ $^ $(INC) $(CXXFLAGS) $(LDFLAGS)

#-------------------------------------------------------------------------------

doc:
	cd documentation ; doxygen

clean:
	rm -f *.exe
	rm -f *.o
	rm -f *.ps
	rm -f *.png
	rm -f chart.json
	rm -f plot.json
	rm -f examples/*.o
	rm -f examples/*.exe
	rm -fR documentation/html
