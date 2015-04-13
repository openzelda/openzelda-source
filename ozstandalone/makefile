# Lux Engine Makefile
# Build Options
# BUILDOS - windows, linux, apple
# OPTIMIZE - FULL, SOME, NONE
# BUILDDEBUG - TRUE, FALSE
# PLATFORMBITS 64, 32
# CC (Compiler)
# BIN (Binary output)
# CUSTOMSETTINGS - Custom build
# CUSTOMOBJECTS - Uses custom_platform_main.cpp, custom_project_media.cpp and custom_lux_screen.cpp instead of platform_main.cpp, project_media.cpp and lux_screen.cpp

#Default Settings
include setting.mk

BIN = luxengine
DOWNLOADER_MODE = none

COMPILER_LIBS += $(OPTIMIZER) $(DEBUG) -L'../ozengine/lib' $(PLATFORM_LIBS)
COMPILER_FLAGS += $(OPTIMIZER) $(DEBUG) -I'../ozengine/elix/src' -I'../ozengine/src' -I'../ozengine/src/$(PLATFORM_DIRECTORY)' -I'../ozengine/include'  -DTIXML_USE_STL -D$(PLATFORM) -DPLATFORMBITS=$(PLATFORMBITS) $(PLATFORM_FLAGS)
COMPILER_FLAGSPP += -Wno-write-strings -fno-access-control -fno-exceptions -fno-rtti $(COMPILER_FLAGS)

#Read platform and custom settings
include makefiles/$(BUILDOS)-standalone.make
ifneq ($(CUSTOMSETTINGS), )
	include custom/$(CUSTOMSETTINGS).make
endif

#main code
OBJ += $(OBJDIR)/config.o $(OBJDIR)/css.o $(OBJDIR)/dialog.o $(OBJDIR)/engine.o $(OBJDIR)/entity.o $(OBJDIR)/entity_manager.o $(OBJDIR)/entity_section.o $(OBJDIR)/gui_settings.o
OBJ += $(OBJDIR)/gui_base.o $(OBJDIR)/gui_theme.o $(OBJDIR)/language.o $(OBJDIR)/map_object.o $(OBJDIR)/misc_functions.o $(OBJDIR)/mokoi_game.o $(OBJDIR)/player.o
OBJ += $(OBJDIR)/save_system.o $(OBJDIR)/widget.o $(OBJDIR)/world.o $(OBJDIR)/object_effect.o $(OBJDIR)/sprite_sheet.o $(OBJDIR)/graphics_system.o $(OBJDIR)/lux_virtual_sprite.o
OBJ += $(OBJDIR)/portal.o $(OBJDIR)/game_info.o $(OBJDIR)/display.o $(OBJDIR)/display_functions.o $(OBJDIR)/layers.o $(OBJDIR)/lux_canvas.o $(OBJDIR)/lux_sprite.o $(OBJDIR)/map.o
OBJ += $(OBJDIR)/world_section.o $(OBJDIR)/map_screen.o $(OBJDIR)/masks.o $(OBJDIR)/test_code.o $(OBJDIR)/lux_polygon.o $(OBJDIR)/map_xml_reader.o $(OBJDIR)/game_config.o
OBJ += $(OBJDIR)/options.o

#Platform
OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/audio.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/core.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/graphics_native.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/worker.o
OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_controls.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_functions.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_media.o
OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_config.o

#FFI
OBJ += $(OBJDIR)/entity_systems/ffi_system.o $(OBJDIR)/entity_systems/ffi_object.o $(OBJDIR)/entity_systems/ffi_collisions.o
OBJ += $(OBJDIR)/entity_systems/ffi_path.o $(OBJDIR)/entity_systems/ffi_entities.o $(OBJDIR)/entity_systems/ffi_player.o
OBJ += $(OBJDIR)/entity_systems/ffi_functions.o $(OBJDIR)/entity_systems/ffi_shaders.o $(OBJDIR)/entity_systems/ffi_layer.o
OBJ += $(OBJDIR)/entity_systems/ffi_spritesheet.o $(OBJDIR)/entity_systems/ffi_map.o $(OBJDIR)/entity_systems/ffi_world.o
OBJ += $(OBJDIR)/entity_systems/ffi_game.o $(OBJDIR)/entity_systems/ffi_input.o $(OBJDIR)/entity_systems/ffi_mask.o

#Tinyxml
OBJ += $(OBJDIR)/tinyxml/tinyxml2.o $(OBJDIR)/tinyxml/tinyxml2ext.o

#elix
OBJ += $(OBJDIR)/elix/elix_directory.o $(OBJDIR)/elix/elix_endian.o  $(OBJDIR)/elix/elix_file.o $(OBJDIR)/elix/elix_string.o $(OBJDIR)/elix/elix_path.o $(OBJDIR)/elix/elix_png.o $(OBJDIR)/elix/elix_program.o

#Download code
OBJ += $(OBJDIR)/downloader-$(DOWNLOADER_MODE).o

#Custom code
ifeq ($(CUSTOMOBJECTS), TRUE)
	OBJ += $(OBJDIR)/custom_platform_main.o $(OBJDIR)/custom_lux_screen.o $(OBJDIR)/custom_project_media.o
else
	OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_main.o $(OBJDIR)/lux_screen.o $(OBJDIR)/project_media.o
endif

#Pawn
ifeq ($(INCLUDE_PAWN), TRUE)
	COMPILER_FLAGS += -DPAWN_VERSION=4 -DAMXTIME_NOIDLE -DAMX_NOPROPLIST -DAMXCONSOLE_NOIDLE -DAMX_NODYNALOAD  -DFIXEDPOINT -DFLOATPOINT -Wno-unused-local-typedefs

	OBJ += $(OBJDIR)/entity_systems/pawn_core.o $(OBJDIR)/entity_systems/pawn_float.o $(OBJDIR)/entity_systems/pawn_string.o
	OBJ += $(OBJDIR)/entity_systems/pawn_functions.o $(OBJDIR)/entity_systems/pawn_graphics.o $(OBJDIR)/entity_systems/pawn_player.o
	OBJ += $(OBJDIR)/entity_systems/pawn_entity.o $(OBJDIR)/entity_systems/pawn_maps.o $(OBJDIR)/entity_systems/pawn_game.o
	OBJ += $(OBJDIR)/entity_systems/pawn_network.o  $(OBJDIR)/entity_systems/pawn_online.o $(OBJDIR)/entity_systems/pawn_console.o
	OBJ += $(OBJDIR)/entity_systems/pawn_time.o $(OBJDIR)/entity_systems/pawn_system.o $(OBJDIR)/entity_systems/pawn_input.o
	OBJ += $(OBJDIR)/entity_systems/pawn_fixed.o $(OBJDIR)/entity_systems/pawn.o
	OBJ += $(OBJDIR)/scripting/amx4/amx.o $(OBJDIR)/scripting/amx4/amxcons.o $(OBJDIR)/scripting/amx4/amxcore.o
	
	ifdef ASM
		COMPILER_FLAGS += -DAMX_ASM
		OBJ += $(OBJDIR)/scripting/amx4/amxexecn.o
	else
		ifdef PAWNSIMPLEEXEC

		else
			COMPILER_FLAGS += -DAMX_ALTCORE
			OBJ += $(OBJDIR)/scripting/amx4/amxexec_gcc.o
		endif
	endif
	
	ifeq ($(PLATFORMBITS), 64)
		COMPILER_FLAGS += -DPAWN_CELL_SIZE=64 -DHAVE_I64
	endif

endif

# OpenGL
ifeq ($(OPENGL), TRUE)
	COMPILER_FLAGSPP += -DOPENGLENABLED
	OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/graphics_opengl.o
	OBJ += $(OBJDIR)/gles/gles_display.o $(OBJDIR)/gles/shaders.o
	OBJ += $(OBJDIR)/gles/draw_vertex1.inc.o $(OBJDIR)/gles/draw_vertex2.inc.o
	OBJ += $(OBJDIR)/gles/shaders_desktop.o $(OBJDIR)/gles/shaders_gles.o $(OBJDIR)/gles/shaders_none.o
endif

# Network
ifeq ($(NETWORK), TRUE)
	COMPILER_FLAGS += -DNETWORKENABLED
	OBJ += $(OBJDIR)/enet/callbacks.o $(OBJDIR)/enet/host.o  $(OBJDIR)/enet/packet.o $(OBJDIR)/enet/peer.o $(OBJDIR)/enet/protocol.o $(OBJDIR)/enet/list.o 
	OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/core_network.o 
	OBJ += $(OBJDIR)/map_network.o
endif

# Box2d
OBJ += $(OBJDIR)/Box2D/Collision/b2BroadPhase.o $(OBJDIR)/Box2D/Collision/b2CollideCircle.o $(OBJDIR)/Box2D/Collision/b2CollideEdge.o $(OBJDIR)/Box2D/Collision/b2CollidePolygon.o
OBJ += $(OBJDIR)/Box2D/Collision/b2Collision.o $(OBJDIR)/Box2D/Collision/b2Distance.o $(OBJDIR)/Box2D/Collision/b2DynamicTree.o $(OBJDIR)/Box2D/Collision/b2TimeOfImpact.o
OBJ += $(OBJDIR)/Box2D/Collision/Shapes/b2ChainShape.o $(OBJDIR)/Box2D/Collision/Shapes/b2CircleShape.o $(OBJDIR)/Box2D/Collision/Shapes/b2EdgeShape.o $(OBJDIR)/Box2D/Collision/Shapes/b2PolygonShape.o
OBJ += $(OBJDIR)/Box2D/Common/b2BlockAllocator.o $(OBJDIR)/Box2D/Common/b2Draw.o $(OBJDIR)/Box2D/Common/b2Math.o $(OBJDIR)/Box2D/Common/b2Settings.o $(OBJDIR)/Box2D/Common/b2StackAllocator.o $(OBJDIR)/Box2D/Common/b2Timer.o
OBJ += $(OBJDIR)/Box2D/Dynamics/b2Body.o $(OBJDIR)/Box2D/Dynamics/b2ContactManager.o $(OBJDIR)/Box2D/Dynamics/b2Fixture.o $(OBJDIR)/Box2D/Dynamics/b2Island.o $(OBJDIR)/Box2D/Dynamics/b2World.o $(OBJDIR)/Box2D/Dynamics/b2WorldCallbacks.o
OBJ += $(OBJDIR)/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.o $(OBJDIR)/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.o $(OBJDIR)/Box2D/Dynamics/Contacts/b2CircleContact.o $(OBJDIR)/Box2D/Dynamics/Contacts/b2Contact.o
OBJ += $(OBJDIR)/Box2D/Dynamics/Contacts/b2ContactSolver.o $(OBJDIR)/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.o $(OBJDIR)/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.o $(OBJDIR)/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.o
OBJ += $(OBJDIR)/Box2D/Dynamics/Contacts/b2PolygonContact.o
OBJ += $(OBJDIR)/Box2D/Dynamics/Joints/b2DistanceJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2FrictionJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2GearJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2Joint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2MotorJoint.o
OBJ += $(OBJDIR)/Box2D/Dynamics/Joints/b2MouseJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2PrismaticJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2PulleyJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2RevoluteJoint.o
OBJ += $(OBJDIR)/Box2D/Dynamics/Joints/b2RopeJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2WeldJoint.o $(OBJDIR)/Box2D/Dynamics/Joints/b2WheelJoint.o
OBJ += $(OBJDIR)/Box2D/Rope/b2Rope.o

OBJ += $(PLATFORM_OBJECTS)

COMPILER_FLAGS += $(CFLAGS)
COMPILER_FLAGSPP += $(CPPFLAGS) -DDOWNLOADER_MODE=$(DOWNLOADER_MODE)
COMPILER_LIBS += $(LDFLAGS)


ifeq ($(PLATFORMBITS), 64)
	COMPILER_FLAGS += -m64
	COMPILER_LIBS += -m64
endif
ifeq ($(PLATFORMBITS), 32)
	COMPILER_FLAGS += -m32
	COMPILER_LIBS += -m32
endif


RM = rm -f

.PHONY: all-before all clean


all: all-before  $(BIN) $(FINALOUTPUT)


all-before:
	@echo --------------------------------
	@echo Building $(BIN) $(PROGRAM_VERSION)
	@echo Target Platform: $(BUILDOS)/$(PLATFORMBITS)

clean:
	@echo --------------------------------
	@echo Cleaning $(BIN)
	@${RM} $(OBJ)

clean-bin:
	@echo --------------------------------
	@echo Cleaning $(BIN)
	@${RM} $(BUILDDIR)/$(BIN)

$(OBJDIR)/%.o : ../ozengine/src/%.cpp
	@echo Compiling $@ $(MESSAGE)
	@-$(MKDIR) $(dir $@)
	@$(CPP) -c $(COMPILER_FLAGSPP) -o $@ $<

$(OBJDIR)/%.o : ../ozengine/$(CUSTOMPATH)/%.cpp
	@echo Compiling $@ $(MESSAGE)
	@-$(MKDIR) $(dir $@)
	@$(CPP) -c $(COMPILER_FLAGSPP) -o $@ $<

$(OBJDIR)/elix/%.o : ../ozengine/elix/src/%.cpp
	@echo Compiling $@ $(MESSAGE)
	@-$(MKDIR) $(dir $@)
	@$(CPP) -c $(COMPILER_FLAGSPP) -o $@ $<

$(OBJDIR)/%.o : ../ozengine/src/%.c
	@echo Compiling $@ $(MESSAGE)
	@-$(MKDIR) $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) -o $@ $<

$(OBJDIR)/%.o : ../ozengine/src/%.asm
	@echo Compiling $@ $(MESSAGE) [ASM]
	@$(ASM) -O1 -f $(ASMTYPE) -o $@ $< -I$(dir $<)

$(BIN): $(OBJ) $(EXTRABINARIES)
	@echo --------------------------------
	@-$(MKDIR) $(BUILDDIR)
	@$(CPP) $(OBJ) -o $(BUILDDIR)/$(BIN) $(COMPILER_LIBS)

install: $(BIN)
	@echo --------------------------------
	@echo Installing $< to $(INSTALLDIR)
	@-$(MKDIR) $(INSTALLDIR)
	@-$(MKDIR) $(INSTALLDIR)/bin
	@cp $(BUILDDIR)/$(BIN) $(INSTALLDIR)/bin/
	@cp -rv ./share/ $(INSTALLDIR)



input_header:
	$(CPP) -o $(OBJDIR)/update_input_header.o -c update_input_header.cpp
	$(CPP) -o $(OBJDIR)/update_input_header.exe $(OBJDIR)/update_input_header.o -s
	-$(RM) $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_media.o
	$(OBJDIR)/update_input_header.exe keyboard
	$(OBJDIR)/update_input_header.exe joystick_generic
	$(OBJDIR)/update_input_header.exe joystick_xbox360
	$(OBJDIR)/update_input_header.exe joystick_dreamcast dc
	$(OBJDIR)/update_input_header.exe mouse_generic

compiler:
	@$(MAKE) -C ../meg/meg_pawn BUILDDIR=$(CURDIR)/$(BUILDDIR) INSTALLDIR=$(CURDIR)/$(INSTALLDIR)
