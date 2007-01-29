

#CXX = g++
CFLAGS = -g -Wall `pkg-config --cflags poppler gtkmm-2.4 libgnomeuimm-2.6 gnome-vfsmm-2.6 libglademm-2.4 gconfmm-2.6` -DDATADIR="\"$(DATADIR)\""
LIBS = -g `pkg-config --libs poppler gtkmm-2.4 libgnomeuimm-2.6 gnome-vfsmm-2.6 libglademm-2.4 gconfmm-2.6` -lboost_regex -Llibbibutils -lbibutils

TARGET = referencer
OBJECTS = TagWindow.o TagList.o  DocumentList.o Document.o BibData.o DocumentProperties.o Preferences.o Utility.o BibUtils.o Transfer.o

PREFIX=/usr/local
DATADIR=$(PREFIX)/share/$(TARGET)
BINDIR=$(PREFIX)/bin

DATAFILES =\
	data/*.glade\
	data/thumbnail_frame.png\
	data/referencer.svg\
	data/introduction.html

all: $(TARGET)

.C.o: ${patsubst %.C, %.h, $<}
	$(CXX) $(CFLAGS) -o ${patsubst %.C, %.o, $<} -c $<

libbibutils/libbibutils.a:
	$(MAKE) -C libbibutils libbibutils.a
	# Remove the .o files in favour of the .a file
	$(MAKE) -C libbibutils clean

$(TARGET): $(OBJECTS) libbibutils/libbibutils.a
	$(CXX) -o $(TARGET) $(OBJECTS) $(LIBS) 

install: all
	install -d $(DESTDIR)$(BINDIR)
	install -s $(TARGET) $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(DATADIR)
	install -m 0644 $(DATAFILES) $(DESTDIR)$(DATADIR)
	install -m 0644 data/referencer.svg $(PREFIX)/share/pixmaps
	install -m 0644 referencer.desktop $(PREFIX)/share/applications

uninstall:
	rm -f  $(DESTDIR)$(BINDIR)/$(TARGET)
	rm -rf $(DESTDIR)$(DATADIR)

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	$(MAKE) -C libbibutils realclean
