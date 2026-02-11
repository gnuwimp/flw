include ../makeinclude

CXXFLAGS += $(FLTK_CXXFLAGS)
CXXFLAGS += -DFLW_USE_PNG
CXXFLAGS += -I..
LDFLAGS  += $(FLTK_IMAGE_LDFLAGS)

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
