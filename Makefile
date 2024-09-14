include makeinclude

ifneq ($(OS), macOS)
	CXXFLAGS += -Wno-cast-function-type
endif

ifeq ($(OS), macOS)
	CXXFLAGS += -Wno-overloaded-virtual
endif

CXXFLAGS += $(FLTK_CXXFLAGS)
CXXFLAGS += -DFLW_USE_PNG
LDFLAGS  += $(FLTKI_LDFLAGS)
INC       = -Isrc

ifeq ($(OS), Windows)
	THEME_RESOURCE = obj/theme.res
endif

#-------------------------------------------------------------------------------

OBJ = obj/flw.o \
	obj/chart.o \
	obj/date.o \
	obj/datechooser.o \
	obj/dlg.o \
	obj/grid.o \
	obj/gridgroup.o \
	obj/inputmenu.o \
	obj/json.o \
	obj/lcdnumber.o \
	obj/logdisplay.o \
	obj/plot.o \
	obj/recentmenu.o \
	obj/scrollbrowser.o \
	obj/splitgroup.o \
	obj/tabledisplay.o \
	obj/tableeditor.o \
	obj/tabsgroup.o \
	obj/toolgroup.o \
	obj/waitcursor.o \

ifeq ($(amalgam), 1)
	CXXFLAGS += -DFLW_AMALGAM
	OBJ       = obj/flw_amalgam.o
	INC       = -I.
endif

all: obj \
	$(OBJ) \
	test_chart.exe \
	test_date.exe \
	test_datechooser.exe \
	test_dlg.exe \
	test_grid.exe \
	test_gridgroup.exe \
	test_inputmenu.exe \
	test_lcdnumber.exe \
	test_logdisplay.exe \
	test_plot.exe \
	test_recentmenu.exe \
	test_scrollbrowser.exe \
	test_splitgroup.exe \
	test_tabledisplay.exe \
	test_tableeditor.exe \
	test_tabsgroup.exe \
	test_theme.exe \
	test_toolgroup.exe \

#-------------------------------------------------------------------------------

obj:
	mkdir obj

$(THEME_RESOURCE): res/theme.rc
	windres res/theme.rc -O coff -o $(THEME_RESOURCE)

obj/flw_amalgam.o: flw.cpp flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/flw.o: src/flw.cpp src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/chart.o: src/chart.cpp src/chart.h src/date.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/date.o: src/date.cpp src/date.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/datechooser.o: src/datechooser.cpp src/datechooser.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/dlg.o: src/dlg.cpp src/dlg.h src/datechooser.h src/gridgroup.h src/flw.h test/test.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/grid.o: src/grid.cpp src/grid.h src/tabledisplay.h  src/tableeditor.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/gridgroup.o: src/gridgroup.cpp src/gridgroup.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/inputmenu.o: src/inputmenu.cpp src/inputmenu.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/json.o: src/json.cpp src/json.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/lcdnumber.o: src/lcdnumber.cpp src/lcdnumber.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/logdisplay.o: src/logdisplay.cpp src/logdisplay.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/plot.o: src/plot.cpp src/plot.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/recentmenu.o: src/recentmenu.cpp src/recentmenu.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/scrollbrowser.o: src/scrollbrowser.cpp src/scrollbrowser.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/splitgroup.o: src/splitgroup.cpp src/splitgroup.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/tabledisplay.o: src/tabledisplay.cpp src/tabledisplay.h src/dlg.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/tableeditor.o: src/tableeditor.cpp src/tableeditor.h src/tabledisplay.h src/date.h src/dlg.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/tabsgroup.o: src/tabsgroup.cpp src/tabsgroup.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/toolgroup.o: src/toolgroup.cpp src/toolgroup.h src/flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/waitcursor.o: src/waitcursor.cpp src/waitcursor.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

#-------------------------------------------------------------------------------

obj/test_chart.o: test/test_chart.cpp src/chart.h $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_chart.exe: obj/test_chart.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_chart: test_chart.exe
	./test_chart.exe $(arg)

obj/test_date.o: test/test_date.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_date.exe: obj/test_date.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_date: test_date.exe
	./test_date.exe $(arg)

obj/test_datechooser.o: test/test_datechooser.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_datechooser.exe: obj/test_datechooser.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_datechooser: test_datechooser.exe
	./test_datechooser.exe $(arg)

obj/test_dlg.o: test/test_dlg.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_dlg.exe: obj/test_dlg.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_dlg: test_dlg.exe
	./test_dlg.exe $(arg)

obj/test_grid.o: test/test_grid.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_grid.exe: obj/test_grid.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_gridgroup.o: test/test_gridgroup.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_gridgroup.exe: obj/test_gridgroup.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_gridgroup: test_gridgroup.exe
	./test_gridgroup.exe $(arg)

obj/test_inputmenu.o: test/test_inputmenu.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_inputmenu.exe: obj/test_inputmenu.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_inputmenu: test_inputmenu.exe
	./test_inputmenu.exe $(arg)

obj/test_lcdnumber.o: test/test_lcdnumber.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_lcdnumber.exe: obj/test_lcdnumber.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_logdisplay.o: test/test_logdisplay.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_logdisplay.exe: obj/test_logdisplay.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_logdisplay: test_logdisplay.exe
	./test_logdisplay.exe $(arg)

obj/test_plot.o: test/test_plot.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_plot.exe: obj/test_plot.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_plot: test_plot.exe
	./test_plot.exe $(arg)

obj/test_recentmenu.o: test/test_recentmenu.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_recentmenu.exe: obj/test_recentmenu.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/test_splitgroup.o: test/test_splitgroup.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

obj/test_scrollbrowser.o: test/test_scrollbrowser.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_scrollbrowser.exe: obj/test_scrollbrowser.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_scrollbrowser: test_scrollbrowser.exe
	./test_scrollbrowser.exe $(arg)

test_splitgroup.exe: obj/test_splitgroup.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_splitgroup: test_splitgroup.exe
	./test_splitgroup.exe $(arg)

obj/test_tabledisplay.o: test/test_tabledisplay.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_tabledisplay.exe: obj/test_tabledisplay.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_tabledisplay: test_tabledisplay.exe
	echo ./test_tabledisplay.exe $(arg)
	./test_tabledisplay.exe $(arg)

obj/test_tableeditor.o: test/test_tableeditor.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_tableeditor.exe: obj/test_tableeditor.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_tableeditor: test_tableeditor.exe
	./test_tableeditor.exe $(arg)

obj/test_tabsgroup.o: test/test_tabsgroup.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_tabsgroup.exe: obj/test_tabsgroup.o $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_tabsgroup: test_tabsgroup.exe
	./test_tabsgroup.exe $(arg)

obj/test_toolgroup.o: test/test_toolgroup.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_toolgroup.exe: obj/test_toolgroup.o $(OBJ) $(THEME_RESOURCE)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_toolgroup: test_toolgroup.exe
	./test_toolgroup.exe $(arg)

obj/test_theme.o: test/test_theme.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

test_theme.exe: obj/test_theme.o $(OBJ) $(THEME_RESOURCE)
	$(CXX) -o $@ $^ $(LDFLAGS)

run_theme: test_theme.exe
	./test_theme.exe $(arg)

#-------------------------------------------------------------------------------

clean:
	rm -f obj/* *.exe *.ps *.pdf
