include ../makeinclude

CXXFLAGS += $(FLTK_CXXFLAGS)
CXXFLAGS += -DFLW_USE_PNG
CXXFLAGS += -I..
LDFLAGS  += $(FLTK_IMAGE_LDFLAGS)
INC       = -Isrc

#-------------------------------------------------------------------------------

all: flw.o \
	file.exe \
	json.exe \
	dialog.exe \
	gridgroup.exe \
	inputmenu.exe \
	recentmenu.exe \
	tabsgroup.exe \

#-------------------------------------------------------------------------------

flw.o: ../flw.cpp ../flw.h
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

file.exe: file.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

json.exe: json.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

dialog.exe: dialog.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

fontdialog.exe: fontdialog.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

gridgroup.exe: gridgroup.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

inputmenu.exe: inputmenu.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

recentmenu.exe: recentmenu.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

tabsgroup.exe: tabsgroup.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

workdialog.exe: workdialog.cpp flw.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INC) $(LDFLAGS)

#-------------------------------------------------------------------------------

clean:
	rm -f *.o *.exe *.ps

#-------------------------------------------------------------------------------

run_file: file.exe
	./file.exe

run_json: json.exe
	./json.exe

run_dialog: dialog.exe
	./dialog.exe

run_gridgroup: gridgroup.exe
	./gridgroup.exe

run_inputmenu: inputmenu.exe
	./inputmenu.exe

run_recentmenu: recentmenu.exe
	./recentmenu.exe

run_tabsgroup: tabsgroup.exe
	./tabsgroup.exe

