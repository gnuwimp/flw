# Makefile generated 2021-03-03 19:57:58 from lmake.conf for win.opt

all: win.makefile \
	obj/chart.o \
	obj/date.o \
	obj/datechooser.o \
	obj/dlg.o \
	obj/fontdialog.o \
	obj/grid.o \
	obj/gridgroup.o \
	obj/lcdnumber.o \
	obj/logdisplay.o \
	obj/price.o \
	obj/splitgroup.o \
	obj/tabledisplay.o \
	obj/tableeditor.o \
	obj/tabsgroup.o \
	obj/theme.o \
	obj/util.o \
	obj/waitcursor.o \
	obj/widgets.o \
	obj/test_chart.o \
	obj/test_date.o \
	obj/test_datechooser.o \
	obj/test_dlg.o \
	obj/test_grid.o \
	obj/test_gridgroup1.o \
	obj/test_gridgroup2.o \
	obj/test_lcdnumber.o \
	obj/test_logdisplay.o \
	obj/test_price.o \
	obj/test_splitgroup.o \
	obj/test_tabledisplay.o \
	obj/test_tableeditor.o \
	obj/test_tabsgroup.o \
	obj/test_theme.o \
	obj/test_util.o \
	obj/libflw.a \
	test_chart.exe \
	test_date.exe \
	test_datechooser.exe \
	test_dlg.exe \
	test_grid.exe \
	test_gridgroup1.exe \
	test_gridgroup2.exe \
	test_lcdnumber.exe \
	test_logdisplay.exe \
	test_price.exe \
	test_splitgroup.exe \
	test_tabledisplay.exe \
	test_tableeditor.exe \
	test_tabsgroup.exe \
	test_theme.exe \
	test_util.exe \

win.makefile: lmake.conf
	lmake -m win.makefile -o win -c lmake.conf -b opt


obj/chart.o: src/chart.cpp src/chart.h src/date.h src/price.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/date.o: src/date.cpp src/date.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/datechooser.o: src/datechooser.cpp src/datechooser.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/dlg.o: src/dlg.cpp src/dlg.h src/datechooser.h src/gridgroup.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/fontdialog.o: src/fontdialog.cpp src/fontdialog.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/grid.o: src/grid.cpp src/grid.h src/tabledisplay.h  src/tableeditor.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/gridgroup.o: src/gridgroup.cpp src/gridgroup.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/lcdnumber.o: src/lcdnumber.cpp src/lcdnumber.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/logdisplay.o: src/logdisplay.cpp src/logdisplay.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/price.o: src/price.cpp src/price.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/splitgroup.o: src/splitgroup.cpp src/splitgroup.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/tabledisplay.o: src/tabledisplay.cpp src/tabledisplay.h src/dlg.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/tableeditor.o: src/tableeditor.cpp src/tableeditor.h src/tabledisplay.h src/date.h src/dlg.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/tabsgroup.o: src/tabsgroup.cpp src/tabsgroup.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/theme.o: src/theme.cpp src/theme.h src/gridgroup.h src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/util.o: src/util.cpp src/util.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/waitcursor.o: src/waitcursor.cpp src/waitcursor.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/widgets.o: src/widgets.cpp src/widgets.h
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_chart.o: test/test_chart.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_date.o: test/test_date.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_datechooser.o: test/test_datechooser.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_dlg.o: test/test_dlg.cpp test/test.h obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_grid.o: test/test_grid.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_gridgroup1.o: test/test_gridgroup1.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_gridgroup2.o: test/test_gridgroup2.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_lcdnumber.o: test/test_lcdnumber.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_logdisplay.o: test/test_logdisplay.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_price.o: test/test_price.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_splitgroup.o: test/test_splitgroup.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_tabledisplay.o: test/test_tabledisplay.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_tableeditor.o: test/test_tableeditor.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_tabsgroup.o: test/test_tabsgroup.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_theme.o: test/test_theme.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/test_util.o: test/test_util.cpp obj/libflw.a
	g++ -O2 -std=c++17 -I.. -DNDEBUG -Isrc -D__USE_MINGW_ANSI_STDIO=1 -D__MSVCRT_VERSION__=0x0800 `/usr/local/bin/fltk-config --cxxflags` -Wno-stringop-overflow -Wno-unused-parameter -c $< -o $@

obj/libflw.a: obj/chart.o obj/date.o obj/datechooser.o obj/dlg.o obj/fontdialog.o obj/grid.o obj/gridgroup.o obj/lcdnumber.o obj/logdisplay.o obj/price.o obj/splitgroup.o obj/tabledisplay.o obj/tableeditor.o obj/tabsgroup.o obj/theme.o obj/util.o obj/waitcursor.o obj/widgets.o
	ar crs $@ $^

test_chart.exe: obj/test_chart.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_date.exe: obj/test_date.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_datechooser.exe: obj/test_datechooser.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_dlg.exe: obj/test_dlg.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_grid.exe: obj/test_grid.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_gridgroup1.exe: obj/test_gridgroup1.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_gridgroup2.exe: obj/test_gridgroup2.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_lcdnumber.exe: obj/test_lcdnumber.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_logdisplay.exe: obj/test_logdisplay.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_price.exe: obj/test_price.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_splitgroup.exe: obj/test_splitgroup.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_tabledisplay.exe: obj/test_tabledisplay.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_tableeditor.exe: obj/test_tableeditor.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_tabsgroup.exe: obj/test_tabsgroup.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_theme.exe: obj/test_theme.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

test_util.exe: obj/test_util.o obj/libflw.a
	g++ -static -s $^ `/usr/local/bin/fltk-config --ldflags` -o $@

clean:
	rm -f \
	obj/chart.o \
	obj/date.o \
	obj/datechooser.o \
	obj/dlg.o \
	obj/fontdialog.o \
	obj/grid.o \
	obj/gridgroup.o \
	obj/lcdnumber.o \
	obj/logdisplay.o \
	obj/price.o \
	obj/splitgroup.o \
	obj/tabledisplay.o \
	obj/tableeditor.o \
	obj/tabsgroup.o \
	obj/theme.o \
	obj/util.o \
	obj/waitcursor.o \
	obj/widgets.o \
	obj/test_chart.o \
	obj/test_date.o \
	obj/test_datechooser.o \
	obj/test_dlg.o \
	obj/test_grid.o \
	obj/test_gridgroup1.o \
	obj/test_gridgroup2.o \
	obj/test_lcdnumber.o \
	obj/test_logdisplay.o \
	obj/test_price.o \
	obj/test_splitgroup.o \
	obj/test_tabledisplay.o \
	obj/test_tableeditor.o \
	obj/test_tabsgroup.o \
	obj/test_theme.o \
	obj/test_util.o \
	obj/libflw.a \
	test_chart.exe \
	test_date.exe \
	test_datechooser.exe \
	test_dlg.exe \
	test_grid.exe \
	test_gridgroup1.exe \
	test_gridgroup2.exe \
	test_lcdnumber.exe \
	test_logdisplay.exe \
	test_price.exe \
	test_splitgroup.exe \
	test_tabledisplay.exe \
	test_tableeditor.exe \
	test_tabsgroup.exe \
	test_theme.exe \
	test_util.exe
	# rm -f
