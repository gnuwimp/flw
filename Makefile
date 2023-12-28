OS := $(shell uname -s)

ifeq ($(build),sanitize)
	SANITIZE = -fsanitize=address
	BUILD = debug
	BUILD_DIR = build/debug
	BUILD_NAME = _d
else ifeq ($(build),release)
	BUILD = release
	BUILD_DIR = build/release
	BUILD_NAME =
else
	BUILD = debug
	BUILD_DIR = build/debug
	BUILD_NAME = _d
endif

ifeq ($(OS),Linux)
	EXT = $(BUILD_NAME)

	ifeq ($(BUILD),release)
		CXX = g++ -O2 -std=c++17 -DNDEBUG -Wno-deprecated-declarations `/usr/local/bin/fltk-config --cxxflags` -Isrc -c $< -o $@
		LD = g++ -o $@ $^ `/usr/local/bin/fltk-config --ldflags`

	else
		CXX = g++ -g -std=c++17 -DDEBUG $(SANITIZE) -W -Wall -Wno-deprecated-declarations `/usr/local/bin/fltk-config --cxxflags` -Isrc -c $< -o $@
		LD = g++ -o $@ $^ $(SANITIZE) `/usr/local/bin/fltk-config --ldflags`
	endif

else ifeq ($(findstring MINGW64,$(OS)), MINGW64)
	EXT = $(BUILD_NAME).exe
	INSTALL = /C/bin

	ifeq ($(BUILD),release)
		CXX = g++ -O2 -std=c++17 -DNDEBUG -DPCRE_STATIC=1 -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Isrc -c $< -o $@
		LD = g++ -o $@ $^ `/usr/local/bin/fltk-config --ldflags`
	else
		CXX = g++ -g -std=c++17 -DDEBUG -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 -W -Wall -Wno-unused-parameter -Wno-cast-function-type `/usr/local/bin/fltk-config --cxxflags` -Isrc -c $< -o $@
		LD = g++ -o $@ $^ `/usr/local/bin/fltk-config --ldflags`
	endif

else
$(error error: unknown compiler)
endif

OBJ=$(BUILD_DIR)/chart.o \
	$(BUILD_DIR)/date.o \
	$(BUILD_DIR)/datechooser.o \
	$(BUILD_DIR)/dlg.o \
	$(BUILD_DIR)/fontdialog.o \
	$(BUILD_DIR)/grid.o \
	$(BUILD_DIR)/gridgroup.o \
	$(BUILD_DIR)/lcdnumber.o \
	$(BUILD_DIR)/logdisplay.o \
	$(BUILD_DIR)/price.o \
	$(BUILD_DIR)/splitgroup.o \
	$(BUILD_DIR)/tabledisplay.o \
	$(BUILD_DIR)/tableeditor.o \
	$(BUILD_DIR)/tabsgroup.o \
	$(BUILD_DIR)/theme.o \
	$(BUILD_DIR)/util.o \
	$(BUILD_DIR)/waitcursor.o \
	$(BUILD_DIR)/widgets.o \
	$(BUILD_DIR)/test_chart.o \
	$(BUILD_DIR)/test_date.o \
	$(BUILD_DIR)/test_datechooser.o \
	$(BUILD_DIR)/test_dlg.o \
	$(BUILD_DIR)/test_grid.o \
	$(BUILD_DIR)/test_gridgroup1.o \
	$(BUILD_DIR)/test_gridgroup2.o \
	$(BUILD_DIR)/test_lcdnumber.o \
	$(BUILD_DIR)/test_logdisplay.o \
	$(BUILD_DIR)/test_price.o \
	$(BUILD_DIR)/test_splitgroup.o \
	$(BUILD_DIR)/test_tabledisplay.o \
	$(BUILD_DIR)/test_tableeditor.o \
	$(BUILD_DIR)/test_tabsgroup.o \
	$(BUILD_DIR)/test_theme.o \
	$(BUILD_DIR)/test_util.o \
	$(BUILD_DIR)/test_widgets.o \

LIB=$(BUILD_DIR)/libflw.a

EXE=test_chart$(EXT) \
	test_date$(EXT) \
	test_datechooser$(EXT) \
	test_dlg$(EXT) \
	test_grid$(EXT) \
	test_gridgroup1$(EXT) \
	test_gridgroup2$(EXT) \
	test_lcdnumber$(EXT) \
	test_logdisplay$(EXT) \
	test_price$(EXT) \
	test_splitgroup$(EXT) \
	test_tabledisplay$(EXT) \
	test_tableeditor$(EXT) \
	test_tabsgroup$(EXT) \
	test_theme$(EXT) \
	test_util$(EXT) \
	test_widgets$(EXT) \

all: $(OBJ) $(LIB) $(EXE)

#-------------------------------------------------------------------------------

$(BUILD_DIR)/chart.o: src/chart.cpp src/chart.h src/date.h src/price.h src/util.h
	$(CXX)

$(BUILD_DIR)/date.o: src/date.cpp src/date.h
	$(CXX)

$(BUILD_DIR)/datechooser.o: src/datechooser.cpp src/datechooser.h
	$(CXX)

$(BUILD_DIR)/dlg.o: src/dlg.cpp src/dlg.h src/datechooser.h src/gridgroup.h src/util.h
	$(CXX)

$(BUILD_DIR)/fontdialog.o: src/fontdialog.cpp src/fontdialog.h src/util.h
	$(CXX)

$(BUILD_DIR)/grid.o: src/grid.cpp src/grid.h src/tabledisplay.h  src/tableeditor.h src/util.h
	$(CXX)

$(BUILD_DIR)/gridgroup.o: src/gridgroup.cpp src/gridgroup.h
	$(CXX)

$(BUILD_DIR)/lcdnumber.o: src/lcdnumber.cpp src/lcdnumber.h
	$(CXX)

$(BUILD_DIR)/logdisplay.o: src/logdisplay.cpp src/logdisplay.h src/util.h
	$(CXX)

$(BUILD_DIR)/price.o: src/price.cpp src/price.h
	$(CXX)

$(BUILD_DIR)/splitgroup.o: src/splitgroup.cpp src/splitgroup.h
	$(CXX)

$(BUILD_DIR)/tabledisplay.o: src/tabledisplay.cpp src/tabledisplay.h src/dlg.h src/util.h
	$(CXX)

$(BUILD_DIR)/tableeditor.o: src/tableeditor.cpp src/tableeditor.h src/tabledisplay.h src/date.h src/dlg.h src/util.h
	$(CXX)

$(BUILD_DIR)/tabsgroup.o: src/tabsgroup.cpp src/tabsgroup.h src/util.h
	$(CXX)

$(BUILD_DIR)/theme.o: src/theme.cpp src/theme.h src/gridgroup.h src/util.h
	$(CXX)

$(BUILD_DIR)/util.o: src/util.cpp src/util.h
	$(CXX)

$(BUILD_DIR)/waitcursor.o: src/waitcursor.cpp src/waitcursor.h
	$(CXX)

$(BUILD_DIR)/widgets.o: src/widgets.cpp src/widgets.h
	$(CXX)

$(BUILD_DIR)/test_chart.o: test/test_chart.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_date.o: test/test_date.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_datechooser.o: test/test_datechooser.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_dlg.o: test/test_dlg.cpp test/test.h $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_grid.o: test/test_grid.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_gridgroup1.o: test/test_gridgroup1.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_gridgroup2.o: test/test_gridgroup2.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_lcdnumber.o: test/test_lcdnumber.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_logdisplay.o: test/test_logdisplay.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_price.o: test/test_price.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_splitgroup.o: test/test_splitgroup.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_tabledisplay.o: test/test_tabledisplay.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_tableeditor.o: test/test_tableeditor.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_tabsgroup.o: test/test_tabsgroup.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_theme.o: test/test_theme.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_util.o: test/test_util.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

$(BUILD_DIR)/test_widgets.o: test/test_widgets.cpp $(BUILD_DIR)/libflw.a
	$(CXX)

#-------------------------------------------------------------------------------

$(BUILD_DIR)/libflw.a: $(BUILD_DIR)/chart.o $(BUILD_DIR)/date.o $(BUILD_DIR)/datechooser.o $(BUILD_DIR)/dlg.o $(BUILD_DIR)/fontdialog.o $(BUILD_DIR)/grid.o $(BUILD_DIR)/gridgroup.o $(BUILD_DIR)/lcdnumber.o $(BUILD_DIR)/logdisplay.o $(BUILD_DIR)/price.o $(BUILD_DIR)/splitgroup.o $(BUILD_DIR)/tabledisplay.o $(BUILD_DIR)/tableeditor.o $(BUILD_DIR)/tabsgroup.o $(BUILD_DIR)/theme.o $(BUILD_DIR)/util.o $(BUILD_DIR)/waitcursor.o $(BUILD_DIR)/widgets.o
	ar crs $@ $^

#-------------------------------------------------------------------------------

test_chart$(EXT): $(BUILD_DIR)/test_chart.o $(BUILD_DIR)/libflw.a
	$(LD)

test_date$(EXT): $(BUILD_DIR)/test_date.o $(BUILD_DIR)/libflw.a
	$(LD)

test_datechooser$(EXT): $(BUILD_DIR)/test_datechooser.o $(BUILD_DIR)/libflw.a
	$(LD)

test_dlg$(EXT): $(BUILD_DIR)/test_dlg.o $(BUILD_DIR)/libflw.a
	$(LD)

test_grid$(EXT): $(BUILD_DIR)/test_grid.o $(BUILD_DIR)/libflw.a
	$(LD)

test_gridgroup1$(EXT): $(BUILD_DIR)/test_gridgroup1.o $(BUILD_DIR)/libflw.a
	$(LD)

test_gridgroup2$(EXT): $(BUILD_DIR)/test_gridgroup2.o $(BUILD_DIR)/libflw.a
	$(LD)

test_lcdnumber$(EXT): $(BUILD_DIR)/test_lcdnumber.o $(BUILD_DIR)/libflw.a
	$(LD)

test_logdisplay$(EXT): $(BUILD_DIR)/test_logdisplay.o $(BUILD_DIR)/libflw.a
	$(LD)

test_price$(EXT): $(BUILD_DIR)/test_price.o $(BUILD_DIR)/libflw.a
	$(LD)

test_splitgroup$(EXT): $(BUILD_DIR)/test_splitgroup.o $(BUILD_DIR)/libflw.a
	$(LD)

test_tabledisplay$(EXT): $(BUILD_DIR)/test_tabledisplay.o $(BUILD_DIR)/libflw.a
	$(LD)

test_tableeditor$(EXT): $(BUILD_DIR)/test_tableeditor.o $(BUILD_DIR)/libflw.a
	$(LD)

test_tabsgroup$(EXT): $(BUILD_DIR)/test_tabsgroup.o $(BUILD_DIR)/libflw.a
	$(LD)

test_theme$(EXT): $(BUILD_DIR)/test_theme.o $(BUILD_DIR)/libflw.a
	$(LD)

test_util$(EXT): $(BUILD_DIR)/test_util.o $(BUILD_DIR)/libflw.a
	$(LD)

test_widgets$(EXT): $(BUILD_DIR)/test_widgets.o $(BUILD_DIR)/libflw.a
	$(LD)

#-------------------------------------------------------------------------------

clean:
	rm -f $(OBJ) $(LIB) $(EXE)
