
OBJ := obj/PauseStack.o obj/FontList.o obj/EntityList.o obj/Animations.o
OBJ += obj/CGame.o obj/Counters.o obj/CGraphics.o obj/CSprite.o
OBJ += obj/CSpriteSheet.o obj/BitmapTextBuffer.o obj/ShapeBuffer.o
OBJ += obj/TextBuffer.o obj/CDisplay.o obj/CVirtualArchive.o
OBJ += obj/CVirtualFile.o obj/Boundary.o obj/CGroup.o
OBJ += obj/CQuestLoader.o obj/CScreen.o obj/Script.o
OBJ += obj/CEntity.o obj/SaveEntity.o obj/CAnimation.o
OBJ += obj/AMX/amx.o obj/AMX/amxcons.o obj/AMX/amxcore.o obj/AMX/amxtime.o
OBJ += obj/AMX/float.o obj/CEntString.o obj/CTextBox.o obj/CCounter.o
OBJ += obj/CNightDay.o obj/CFade.o obj/CWipe.o obj/GameOver.o
OBJ += obj/CSaveLoad.o obj/SaveLoadFunctions.o obj/CMusic.o
OBJ += obj/CSFX.o obj/CSound.o obj/Hash.o obj/FS_Functions.o
OBJ += obj/CheckKeys.o obj/GeneralFunctions.o obj/Main.o
OBJ += obj/Socket.o obj/SocketScript.o obj/Texture.o
OBJ += obj/TTFfont.o

LINKOBJ = obj/Launcher.o $(RES)

.PHONY: all all-before all-after clean clean-custom

all: all-before  $(BIN) all-after

clean: clean-custom
	$(RM) $(OBJ) $(BIN)


$(SO): $(OBJ)
	$(CPP) $(OBJ) -o $(SO) $(LIBS) $(SOLNK)


$(BIN): $(SO) $(LINKOBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)
	strip $(BIN)
	strip $(SO)

obj/%.o : src/%.cpp
	$(CPP) -c $(CXXFLAGS) -DLIBOZFILE="\"$(SOFILE)\"" -o $@ $<

obj/%.o : src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<
