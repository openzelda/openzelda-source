ifeq ($(SUPPORTPATH), )
$(error Please set SUPPORTPATH variable before running make. )
endif

LDFLAGS += -L"$(SUPPORTPATH)/lib"
CPPFLAGS += -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/SDL2"

#Settings
INCLUDE_PAWN = TRUE
NETWORK = FALSE
DOWNLOADER_MODE = none
OPENGL = TRUE
BUILDDEBUG = FALSE
OPTIMIZE = SOME

CPP = g++
CC = gcc

ASMTYPE = win32
ASM = 

BIN  = windows.exe
BINEXT = .exe
OBJDIR = ./objects-standalone

ifeq ($(PLATFORMBITS), 64)
	RES_OUTPUT = pe-x86-64
	OBJDIR += 64
else
	RES_OUTPUT = pe-i386
	PLATFORM_FLAGS += -march=i586
endif

#Static Settings
PLATFORM = __GNUWIN32__
PLATFORM_DIRECTORY = platform/sdl2

PLATFORM_LIBS = -mwindows -lmingw32 -static -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2 -Wl,-Bdynamic -lws2_32 -lcurldll -lssl.dll -lcrypto.dll
PLATFORM_LIBS += -lopengl32 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid
PLATFORM_FLAGS = -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DNO_ZLIB -DUSE_SDL2 -DDISPLAYMODE_OPENGL -DSTANDALONE -Dmain=SDL_main
ifeq ($(NETWORK), TRUE)
	PLATFORM_OBJECTS += $(OBJDIR)/enet/win32.o
endif

