COMPILER_LIBS =
COMPILER_FLAGS =

ifeq ($(OBJDIR), )
	OBJDIR = ./objects
endif

ifeq ($(BUILDDIR), )
	BUILDDIR = ./bin
endif

ifeq ($(INSTALLDIR), )
	INSTALLDIR = /opt/mokoi
endif

#Plaform bits
ifeq (${shell uname -m}, x86_64)
	PLATFORMBITS = 64
endif
ifeq (${shell uname -m}, x86)
	PLATFORMBITS = 32
endif
ifeq (${shell uname -m}, i686)
	PLATFORMBITS = 32
endif
ifeq (${shell uname -m}, i386)
	PLATFORMBITS = 32
endif

ifeq ($(PLATFORMBITS), )
	PLATFORMBITS = other
endif


#Build platform & target
ifeq ($(BUILDOS), )
	BUILDOS	= windows
	ifeq ($(BUILDPLATFORM), Linux)
		BUILDOS	= linux
	endif
	ifeq ($(BUILDPLATFORM), Apple)
		BUILDOS	= apple
	endif
	ifeq ($(BUILDPLATFORM), Darwin)
		BUILDOS	= apple
	endif
	ifeq ($(BUILDPLATFORM), BeOS)
		BUILDOS	= beos
	endif
	ifeq ($(BUILDPLATFORM), Haiku)
		BUILDOS	= beos
	endif
	ifeq ($(BUILDPLATFORM), Syllable)
		BUILDOS	= syllable
	endif
endif

ifeq ($(OPTIMIZE), FULL)
	OPTIMIZER	= -O3
else
	ifeq ($(OPTIMIZE), SOME)
		OPTIMIZER	= -O1
	else
		ifeq ($(OPTIMIZE), NONE)
			OPTIMIZER	= -O0 -DNDEBUG
		else
			OPTIMIZER	= 
		endif
	endif
endif

ifeq ($(BUILDDEBUG), TRUE)
	DEBUG = -g3
	OPTIMIZER = -O0 -Wall  -DDEBUG -Wformat -Wno-unused-variable
	MESSAGE = with debugging support
else
	ifneq ($(OPTIMIZE), NONE)
		OPTIMIZER +=  -DNDEBUG -Wformat 
	endif
endif



buildall: all



info:
	@echo --------------------------------
	@echo Build Platform: $(BUILDPLATFORM)
	@echo Target Platform: $(BUILDOS)/$(PLATFORMBITS)
	@echo Debug Build? $(BUILDDEBUG)
	@echo Build Flags: $(COMPILER_FLAGS)
	@echo Build Libs: $(COMPILER_LIBS)
