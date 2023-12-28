include makeinclude

CXXFLAGS += -Wno-cast-function-type
CXXFLAGS += $(FLTK_CXXFLAGS)
LDFLAGS  += $(FLTKI_LDFLAGS)
LDFLAGS  += obj/libflw.a
INC       = -Isrc

ifeq ($(OS), Windows)
	THEME_RESOURCE = obj/theme.res
endif

#-------------------------------------------------------------------------------

all: test_chart.exe \
	test_date.exe \
	test_datechooser.exe \
	test_dlg.exe \
	test_grid.exe \
	test_gridgroup.exe \
	test_lcdnumber.exe \
	test_logdisplay.exe \
	test_plot.exe \
	test_price.exe \
	test_recentmenu.exe \
	test_splitgroup.exe \
	test_tabledisplay.exe \
	test_tableeditor.exe \
	test_tabsgroup.exe \
	test_theme.exe \
	test_util.exe \
	test_widgets.exe \

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
	obj/plot.o \
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

#-------------------------------------------------------------------------------

obj:
	mkdir obj

$(THEME_RESOURCE): res/theme.rc
	windres res/theme.rc -O coff -o $(THEME_RESOURCE)

obj/chart.o: src/chart.cpp src/chart.h src/date.h src/price.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/date.o: src/date.cpp src/date.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/datechooser.o: src/datechooser.cpp src/datechooser.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/dlg.o: src/dlg.cpp src/dlg.h src/datechooser.h src/gridgroup.h src/util.h test/test.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/fontdialog.o: src/fontdialog.cpp src/fontdialog.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/grid.o: src/grid.cpp src/grid.h src/tabledisplay.h  src/tableeditor.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/gridgroup.o: src/gridgroup.cpp src/gridgroup.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/inputmenu.o: src/inputmenu.cpp src/inputmenu.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/lcdnumber.o: src/lcdnumber.cpp src/lcdnumber.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/logdisplay.o: src/logdisplay.cpp src/logdisplay.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/plot.o: src/plot.cpp src/plot.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/price.o: src/price.cpp src/price.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/recentmenu.o: src/recentmenu.cpp src/recentmenu.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/scrollbrowser.o: src/scrollbrowser.cpp src/scrollbrowser.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/splitgroup.o: src/splitgroup.cpp src/splitgroup.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/tabledisplay.o: src/tabledisplay.cpp src/tabledisplay.h src/dlg.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/tableeditor.o: src/tableeditor.cpp src/tableeditor.h src/tabledisplay.h src/date.h src/dlg.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/tabsgroup.o: src/tabsgroup.cpp src/tabsgroup.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/theme.o: src/theme.cpp src/theme.h src/gridgroup.h src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/util.o: src/util.cpp src/util.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@ -DFLW_USE_PNG

obj/waitcursor.o: src/waitcursor.cpp src/waitcursor.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/libflw.a: $(OBJ)
	ar crs $@ $^

#-------------------------------------------------------------------------------

obj/test_chart.o: test/test_chart.cpp obj/chart.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_chart.exe: obj/test_chart.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_date.o: test/test_date.cpp obj/date.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_date.exe: obj/test_date.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_datechooser.o: test/test_datechooser.cpp obj/datechooser.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_datechooser.exe: obj/test_datechooser.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_dlg.o: test/test_dlg.cpp obj/dlg.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_dlg.exe: obj/test_dlg.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_grid.o: test/test_grid.cpp obj/grid.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_grid.exe: obj/test_grid.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_gridgroup.o: test/test_gridgroup.cpp obj/gridgroup.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_gridgroup.exe: obj/test_gridgroup.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_lcdnumber.o: test/test_lcdnumber.cpp obj/lcdnumber.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_lcdnumber.exe: obj/test_lcdnumber.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_logdisplay.o: test/test_logdisplay.cpp obj/logdisplay.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_logdisplay.exe: obj/test_logdisplay.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_plot.o: test/test_plot.cpp obj/plot.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_plot.exe: obj/test_plot.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_price.o: test/test_price.cpp obj/price.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_price.exe: obj/test_price.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_recentmenu.o: test/test_recentmenu.cpp obj/recentmenu.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_recentmenu.exe: obj/test_recentmenu.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_splitgroup.o: test/test_splitgroup.cpp obj/splitgroup.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_splitgroup.exe: obj/test_splitgroup.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_tabledisplay.o: test/test_tabledisplay.cpp obj/tabledisplay.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_tabledisplay.exe: obj/test_tabledisplay.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_tableeditor.o: test/test_tableeditor.cpp obj/tableeditor.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_tableeditor.exe: obj/test_tableeditor.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_tabsgroup.o: test/test_tabsgroup.cpp obj/tabsgroup.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_tabsgroup.exe: obj/test_tabsgroup.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_theme.o: test/test_theme.cpp obj/theme.o obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_theme.exe: obj/test_theme.o obj/libflw.a $(THEME_RESOURCE)
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_util.o: test/test_util.cpp obj/util.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_util.exe: obj/test_util.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_widgets.o: test/test_widgets.cpp obj/inputmenu.o obj/scrollbrowser.o obj/util.o obj/theme.o
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_widgets.exe: obj/test_widgets.o obj/libflw.a
	$(CXX) -o $@ $^ $(LDFLAGS)

#-------------------------------------------------------------------------------

clean:
	rm -f obj/* *.exe
