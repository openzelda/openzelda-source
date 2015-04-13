CUSTOMOBJECTS = TRUE
CUSTOMPATH = ../custom/ozengine

BIN = oz$(BINEXT)

PLATFORM_FLAGS += -DPROGRAM_NAME="\"Open Zelda\""
PLATFORM_FLAGS += -DPROGRAM_DOCUMENTS="\"openzelda-quests\""
PLATFORM_FLAGS += -DPROGRAM_VERSION_STABLE="\"2.0\""
PLATFORM_FLAGS += -DDIRECTORY_FILE="\"edit.zelda\""
PLATFORM_FLAGS += -DGUI_PORTAL_URL="\"openzelda.net/games/1.9/\""
PLATFORM_FLAGS += -DPACKAGE_GET_URL="\"http://openzelda.net/get/?\""
PLATFORM_FLAGS += -DCUSTOMOBJECTS=TRUE


ifeq ($(BUILDOS),windows)
	RES_SOURCE = $(CUSTOMPATH)/openzelda-standalone.rc
endif
