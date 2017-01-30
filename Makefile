
include makefile.common

GCC=gcc
MOC=/usr/lib/x86_64-linux-gnu/qt4/bin/moc

CFLAGS=-g -O2 -D_linux_ -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
DESTDIR=
PREFIX=/usr
ENABLE_GUI=yes
LIBDIR=$(PREFIX)/lib
BINDIR=$(PREFIX)/bin
DATADIR=$(PREFIX)/share
FFMPEG_CFLAGS= 
FFMPEG_LIBS=-lavcodec -lavutil  
INSTALL=/usr/bin/install -c
OBJCOPY=objcopy
LD=ld
BUILDINFO_ARCH_NAME=$(shell $(GCC) -dumpmachine)
BUILDINFO_BUILD_DATE=$(shell date)

ifeq ($(ENABLE_GUI),yes)
OUT_GUI=out/makemkv
endif

all: out/libdriveio.so.0 out/libmakemkv.so.1 $(OUT_GUI) out/libmmbd.so.0
	@echo "type \"sudo make install\" to install"

clean:
	-rm -rf out tmp

install: out/libdriveio.so.0 out/libmakemkv.so.1 $(OUT_GUI) out/libmmbd.so.0
	$(INSTALL) -D -m 644 out/libdriveio.so.0 $(DESTDIR)$(LIBDIR)/libdriveio.so.0
	$(INSTALL) -D -m 644 out/libmakemkv.so.1 $(DESTDIR)$(LIBDIR)/libmakemkv.so.1
	$(INSTALL) -D -m 644 out/libmmbd.so.0 $(DESTDIR)$(LIBDIR)/libmmbd.so.0
ifeq ($(DESTDIR),)
	ldconfig
endif
ifeq ($(ENABLE_GUI),yes)
	$(INSTALL) -D -m 755 out/makemkv $(DESTDIR)$(BINDIR)/makemkv
	$(INSTALL) -D -m 644 makemkvgui/share/makemkv.desktop $(DESTDIR)$(DATADIR)/applications/makemkv.desktop
	$(INSTALL) -D -m 644 makemkvgui/share/icons/16x16/makemkv.png $(DESTDIR)$(DATADIR)/icons/hicolor/16x16/apps/makemkv.png
	$(INSTALL) -D -m 644 makemkvgui/share/icons/22x22/makemkv.png $(DESTDIR)$(DATADIR)/icons/hicolor/22x22/apps/makemkv.png
	$(INSTALL) -D -m 644 makemkvgui/share/icons/32x32/makemkv.png $(DESTDIR)$(DATADIR)/icons/hicolor/32x32/apps/makemkv.png
	$(INSTALL) -D -m 644 makemkvgui/share/icons/64x64/makemkv.png $(DESTDIR)$(DATADIR)/icons/hicolor/64x64/apps/makemkv.png
	$(INSTALL) -D -m 644 makemkvgui/share/icons/128x128/makemkv.png $(DESTDIR)$(DATADIR)/icons/hicolor/128x128/apps/makemkv.png
endif

out/%: out/%.full
	$(OBJCOPY) --strip-all --strip-debug --strip-unneeded --discard-all $< $@ 

out/libdriveio.so.0.full:
	mkdir -p out
	$(GCC) $(CFLAGS) -D_REENTRANT -shared -Wl,-z,defs -o$@ $(LIBDRIVEIO_INC) $(LIBDRIVEIO_SRC) \
	-fPIC -Xlinker -dy -Xlinker --version-script=libdriveio/src/libdriveio.vers \
	-Xlinker -soname=libdriveio.so.0 -lc -lstdc++

out/libmakemkv.so.1.full: tmp/gen_buildinfo.h
	mkdir -p out
	$(GCC) $(CFLAGS) -D_REENTRANT -shared -Wl,-z,defs -o$@ $(LIBEBML_INC) $(LIBEBML_DEF) $(LIBMATROSKA_INC) \
	$(LIBMAKEMKV_INC) $(SSTRING_INC) $(MAKEMKVGUI_INC) $(LIBABI_INC) $(LIBFFABI_INC) $(LIBDCADEC_DEF) \
	$(LIBEBML_SRC) $(LIBMATROSKA_SRC) $(LIBMAKEMKV_SRC) $(GLIBC_SRC) $(SSTRING_SRC) \
	$(LIBABI_SRC) $(LIBABI_SRC_LINUX) $(LIBFFABI_SRC) $(LIBDCADEC_SRC) \
	-DHAVE_BUILDINFO_H -Itmp $(FFMPEG_CFLAGS) \
	-fPIC -Xlinker -dy -Xlinker --version-script=libmakemkv/src/libmakemkv.vers \
	-Xlinker -soname=libmakemkv.so.1 -lc -lstdc++ -lcrypto -lz -lexpat $(FFMPEG_LIBS) -lm -lrt

out/libmmbd.so.0.full:
	mkdir -p out
	$(GCC) $(CFLAGS) -D_REENTRANT -shared -Wl,-z,defs -o$@ $(MAKEMKVGUI_INC) $(LIBMMBD_INC) \
	$(LIBMAKEMKV_INC) $(SSTRING_INC) $(LIBABI_INC) $(LIBMMBD_SRC) $(LIBMMBD_SRC_LINUX) $(SSTRING_SRC) \
	-fPIC -Xlinker -dy -Xlinker --version-script=libmmbd/src/libmmbd.vers \
	-Xlinker -soname=libmmbd.so.0 -lc -lstdc++ -lrt -lpthread -lcrypto

out/makemkv.full: $(MAKEMKVGUI_GEN) $(MAKEMKVGUI_SRC) $(MAKEMKVGUI_SRC_LINUX) tmp/gen_buildinfo.h
	mkdir -p out
	$(GCC) $(CFLAGS) -o$@ $(MAKEMKVGUI_INC) $(LIBMAKEMKV_INC) $(SSTRING_INC) $(LIBDRIVEIO_INC) $(LIBABI_INC) \
	$(MAKEMKVGUI_SRC) $(MAKEMKVGUI_SRC_LINUX) $(MAKEMKVGUI_GEN) $(SSTRING_SRC) $(LIBDRIVEIO_SRC_PUB) \
	-DHAVE_BUILDINFO_H -Itmp \
	-DQT_SHARED -I/usr/include/qt4 -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtDBus -I/usr/include/qt4/QtXml   -lc -lstdc++ \
	-lQtGui -lQtDBus -lQtXml -lQtCore   -lpthread -lz -lrt

tmp/gen_buildinfo.h:
	mkdir -p tmp
	echo "#define BUILDINFO_ARCH_NAME \"$(BUILDINFO_ARCH_NAME)\"" >> $@
	echo "#define BUILDINFO_BUILD_DATE \"$(BUILDINFO_BUILD_DATE)\"" >> $@

tmp/moc_%.cpp : makemkvgui/src/%.h
	mkdir -p tmp
	$(MOC) -o $@ $<

tmp/image_data.o : makemkvgui/bin/image_data.bin
	mkdir -p tmp
	$(LD) -r -b binary -o $@ $<

