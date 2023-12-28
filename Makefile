OS := $(shell uname -s)

ifeq ($(build),sanitize)
	FLAGS = -fsanitize=address -g -DDEBUG
else ifeq ($(build),release)
	FLAGS = -O2 -DNDEBUG
else ifeq ($(build),debug)
	FLAGS = -g -DDEBUG
else
	FLAGS = -DDEBUG
endif

ifeq ($(findstring MINGW64,$(OS)), MINGW64)
	RES = res/test.res

	CXX = g++ $(FLAGS) -std=c++17 -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 -fdiagnostics-color=always -W -Wall `/usr/local/bin/fltk-config --cxxflags` -Isrc -c $< -o $@
	LD  = g++ $(FLAGS) -o $@ $^ -Isrc `/usr/local/bin/fltk-config --cxxflags --ldflags --use-images`
else
	RES =
	CXX = g++ $(FLAGS) -std=c++17 -W -Wall `/usr/local/bin/fltk-config --cxxflags` -Isrc -c $< -o $@
	LD  = g++ $(FLAGS) -o $@ $^ -Isrc `/usr/local/bin/fltk-config --cxxflags --ldflags --use-images`
endif

OBJ=obj/chart.o \
	obj/date.o \
	obj/datechooser.o \
	obj/dlg.o \
	obj/fontdialog.o \
	obj/grid.o \
	obj/gridgroup.o \
	obj/inputmenu.o \
	obj/lcdnumber.o \
	obj/logdisplay.o \
	obj/price.o \
	obj/recentmenu.o \
	obj/scrollbrowser.o \
	obj/splitgroup.o \
	obj/tabledisplay.o \
	obj/tableeditor.o \
	obj/tabsgroup.o \
	obj/theme.o \
	obj/util.o \
	obj/waitcursor.o \

LIB=obj/libflw.a

EXE=test_chart.exe \
	test_date.exe \
	test_datechooser.exe \
	test_dlg.exe \
	test_grid.exe \
	test_gridgroup1.exe \
	test_gridgroup2.exe \
	test_lcdnumber.exe \
	test_logdisplay.exe \
	test_price.exe \
	test_recentmenu.exe \
	test_splitgroup.exe \
	test_tabledisplay.exe \
	test_tableeditor.exe \
	test_tabsgroup.exe \
	test_theme.exe \
	test_util.exe \
	test_widgets.exe \

all: obj $(LIB) $(EXE)

#-------------------------------------------------------------------------------

obj:
	mkdir obj

res/test.res: res/test.rc
	windres res/test.rc -O coff -o res/test.res

obj/chart.o: src/chart.cpp src/chart.h src/date.h src/price.h src/util.h
	$(CXX)

obj/date.o: src/date.cpp src/date.h
	$(CXX)

obj/datechooser.o: src/datechooser.cpp src/datechooser.h src/util.h
	$(CXX)

obj/dlg.o: src/dlg.cpp src/dlg.h src/datechooser.h src/gridgroup.h src/util.h
	$(CXX)

obj/fontdialog.o: src/fontdialog.cpp src/fontdialog.h src/util.h
	$(CXX)

obj/grid.o: src/grid.cpp src/grid.h src/tabledisplay.h  src/tableeditor.h src/util.h
	$(CXX)

obj/gridgroup.o: src/gridgroup.cpp src/gridgroup.h src/util.h
	$(CXX)

obj/inputmenu.o: src/inputmenu.cpp src/inputmenu.h src/util.h
	$(CXX)

obj/lcdnumber.o: src/lcdnumber.cpp src/lcdnumber.h src/util.h
	$(CXX)

obj/logdisplay.o: src/logdisplay.cpp src/logdisplay.h src/util.h
	$(CXX)

obj/price.o: src/price.cpp src/price.h
	$(CXX)

obj/recentmenu.o: src/recentmenu.cpp src/recentmenu.h src/util.h
	$(CXX)

obj/scrollbrowser.o: src/scrollbrowser.cpp src/scrollbrowser.h src/util.h
	$(CXX)

obj/splitgroup.o: src/splitgroup.cpp src/splitgroup.h src/util.h
	$(CXX)

obj/tabledisplay.o: src/tabledisplay.cpp src/tabledisplay.h src/dlg.h src/util.h
	$(CXX)

obj/tableeditor.o: src/tableeditor.cpp src/tableeditor.h src/tabledisplay.h src/date.h src/dlg.h src/util.h
	$(CXX)

obj/tabsgroup.o: src/tabsgroup.cpp src/tabsgroup.h src/util.h
	$(CXX)

obj/theme.o: src/theme.cpp src/theme.h src/gridgroup.h src/util.h
	$(CXX)

obj/util.o: src/util.cpp src/util.h
	$(CXX) -DFLW_USE_PNG -Wno-deprecated-declarations

obj/waitcursor.o: src/waitcursor.cpp src/waitcursor.h
	$(CXX)

obj/libflw.a: $(OBJ)
	ar crs $@ $^

#-------------------------------------------------------------------------------

test_chart.exe: test/test_chart.cpp obj/libflw.a
	$(LD)

test_date.exe: test/test_date.cpp obj/libflw.a
	$(LD)

test_datechooser.exe: test/test_datechooser.cpp obj/libflw.a
	$(LD)

test_dlg.exe: test/test_dlg.cpp obj/libflw.a
	$(LD)

test_grid.exe: test/test_grid.cpp obj/libflw.a
	$(LD)

test_gridgroup1.exe: test/test_gridgroup1.cpp obj/libflw.a
	$(LD)

test_gridgroup2.exe: test/test_gridgroup2.cpp obj/libflw.a
	$(LD)

test_lcdnumber.exe: test/test_lcdnumber.cpp obj/libflw.a
	$(LD)

test_logdisplay.exe: test/test_logdisplay.cpp obj/libflw.a
	$(LD)

test_price.exe: test/test_price.cpp obj/libflw.a
	$(LD)

test_recentmenu.exe: test/test_recentmenu.cpp obj/libflw.a
	$(LD)

test_splitgroup.exe: test/test_splitgroup.cpp obj/libflw.a
	$(LD)

test_tabledisplay.exe: test/test_tabledisplay.cpp obj/libflw.a
	$(LD)

test_tableeditor.exe: test/test_tableeditor.cpp obj/libflw.a
	$(LD)

test_tabsgroup.exe: test/test_tabsgroup.cpp obj/libflw.a
	$(LD)

test_theme.exe: test/test_theme.cpp obj/libflw.a $(RES)
	$(LD)

test_util.exe: test/test_util.cpp obj/libflw.a
	$(LD)

test_widgets.exe: test/test_widgets.cpp obj/libflw.a
	$(LD)

#-------------------------------------------------------------------------------

clean:
	rm -f obj/* $(LIB) $(EXE) res/test.res
