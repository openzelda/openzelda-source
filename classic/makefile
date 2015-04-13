# Project: OpenZelda
# Makefile created by Dev-C++ 5.1.0.0

BUILDPLATFORM = ${shell uname}

CPP     = g++
CC      = gcc
WINDRES =
RES     =
SOFILE	= libozc.so
SO      = build/$(SOFILE)
SOLNK	= -shared
BIN     = build/classicopenzelda
RM      = rm -f

ifeq ($(BUILDPLATFORM), Darwin)

	CPP      = g++ -arch i386 
	CC       = gcc -arch i386 
	LIBS     = -static-libgcc -F"$(HOME)/Library/Frameworks" -framework Cocoa -framework SDL2_mixer -framework SDL2 -framework OpenGL -lz  -fmessage-length=0  -mmmx -Wl,-rpath -Wl,\$$ORIGIN
	INCS     = -F$(HOME)/Library/Frameworks/ -I $(HOME)/Library/Frameworks/SDL2_mixer.framework/Headers -I $(HOME)/Library/Frameworks/SDL2.framework/Headers -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5
	CXXFLAGS = $(INCS) -DAPPLE -Wno-write-strings -fno-access-control -fno-exceptions -fno-rtti
	CFLAGS   = $(INCS) -DAPPLE -std=gnu99
	SOLNK	 = -dynamiclib
${shell ln -s $(HOME)/Library/Frameworks/SDL2.framework/Headers $(HOME)/Library/Frameworks/SDL2.framework/Headers/SDL2}
${shell ln -s $(HOME)/Library/Frameworks/SDL2_mixer.framework/Headers $(HOME)/Library/Frameworks/SDL2_mixer.framework/Headers/SDL2}

else
	ifeq ($(BUILDPLATFORM), Linux)
		LIBS     = `sdl2-config --libs` -lz -fmessage-length=0 -m32 -Wl,-rpath -Wl,\$$ORIGIN
		INCS     = `sdl2-config --cflags`
		CXXFLAGS = $(INCS) -DLINUX -m32 -Wno-write-strings -fno-access-control -fno-exceptions -fno-rtti -Dmain=SDL_main -std=c++11
		CFLAGS   = $(INCS) -DLINUX -m32 -std=gnu99
	else
		BUILDPLATFORM = WINDOWS
		CPP      = g++.exe
		CC       = gcc.exe
		WINDRES  = windres.exe
		RES      = obj/OpenZelda_private.res
		BIN      = build/classicopenzelda.exe
		SOFILE	 = libozc.dll
		SO		 = build/$(SOFILE)
		RM       = rm.bat
		LIBS     = -static-libgcc -Wl,--enable-auto-import -L"lib" -L"C:/dev/supportlibs/lib" -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32 -lz -fmessage-length=0 -mmmx -m32 -std=gnu99

		INCS     = -I"include" -I"C:/dev/supportlibs/include"
		CXXFLAGS = $(INCS) -D__GNUWIN32__ -DWIN32 -D_WINDOWS -m32 -Wno-write-strings -fno-access-control -fno-exceptions -fno-rtti -Dmain=SDL_main -std=c++11
		CFLAGS   = $(INCS) -D__GNUWIN32__ -DWIN32 -D_WINDOWS -m32 -std=gnu99

	endif

endif

#CXXFLAGS += -ggdb -DDEBUG
#CFLAGS   += -ggdb -DDEBUG
#LIBS	 += -ggdb

LIBS	 += -Os


include build.mk

ifeq ($(BUILDPLATFORM), WINDOWS)
obj/OpenZelda_private.res: src/OpenZelda_private.rc
	$(WINDRES) -i src/OpenZelda_private.rc --input-format=rc -o obj/OpenZelda_private.res -O coff -F pe-i386 --include-dir src/
endif

