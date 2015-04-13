export BASE_DIRECTORY=$(CURDIR)
export INSTALLDIR=/opt/openzelda
export BUILDDIR=../openzelda/bin
export BUILDCUSTOM=TRUE
export CUSTOMSETTINGS=openzelda
export SKIPMODULES=FALSE

SUBDIRS := questdesigner ozengine
MAKE ?= make

all: $(patsubst %, _dir_%, $(SUBDIRS)) info

pre: 
	git submodule update --init

$(patsubst %, _dir_%, $(SUBDIRS)): 
	@$(MAKE) -C $(patsubst _dir_%, %, $@) all CUSTOMPATH=../custom/$(patsubst _dir_%,%,$@)

clean: $(patsubst %, _clean_dir_%, $(SUBDIRS))

$(patsubst %, _clean_dir_%, $(SUBDIRS)):
	@$(MAKE) -C $(patsubst _clean_dir_%, %, $@) clean

dist: $(patsubst %, _dist_dir_%, $(SUBDIRS))

$(patsubst %, _dist_dir_%, $(SUBDIRS)):
	@$(MAKE) -C $(patsubst _dist_dir_%, %, $@) dist

install:
	@mkdir -p $(INSTALLDIR)/bin/
	@mkdir -p $(INSTALLDIR)/share/
	cp -R $(BASE_DIRECTORY)/openzelda/bin/* $(INSTALLDIR)/bin
	cp -R $(BASE_DIRECTORY)/openzelda/share/* $(INSTALLDIR)/share

package-linux:
	# Ubuntu 10.04 LTS 32-bit
	#fpm -t deb -s dir -C ./resources --prefix /opt/openzelda -n openzelda -v 1.8.0 --category Games -d 'libsdl1.2debian' -d 'libsdl-mixer1.2' -d 'libsdl-gfx1.2-4' -d 'libsdl-image1.2' -d 'libgtk2.0-0 > 2.19' -d 'libgtksourceview2.0-0' -d 'libsoup2.4-1' -m 'Luke <luke@openzelda.net>' --url 'http://openzelda.net' --license ZLIB --vendor '' --description 'Game Creator'   --after-install resources/share/openzelda-2.0/scripts/linux-install.sh --before-remove resources/share/openzelda-2.0/scripts/linux-remove.sh bin share
	#tar -jcf openzelda_1.8.0_i386.tar.bz2 -C resources bin share
	# Ubuntu 12.04 64 bit
	fpm -t deb -s dir -C ./resources --prefix /opt/openzelda -n openzelda -v 1.9.0 --category Games -d 'libsdl1.2debian' -d 'libsdl-mixer1.2' -d 'libsdl-gfx1.2-4' -d 'libsdl-image1.2' -d 'libgtk-3-0' -d 'libgtksourceview-3.0-0' -d 'libsoup2.4-1' -m 'Luke <luke@openzelda.net>' --url 'http://openzelda.net' --license ZLIB --vendor '' --description 'Game Creator'   --after-install resources/share/openzelda-2.0/scripts/linux-install.sh --before-remove resources/share/openzelda-2.0/scripts/linux-remove.sh bin share
	tar -jcf openzelda_1.9.0_amd64.tar.bz2 -C resources bin share

package-osx:
	~/.local/bin/gtk-mac-bundler editor/custom/questdesigner.bundle
	chmod -R -w ../Quest\ Designer.app/Contents/Resources/share/openzelda-2.0/examples/

	chmod +x engine/custom/packageopenzelda.sh
	./engine/custom/packageopenzelda.sh
	zip -9 -y -r ../openzelda_1.9.0_i386_osx.zip "../Quest Designer.app" "../OpenZelda.app" -x*.git* -x*.svn* -x*.DS_Store*

package-windows:


clone-git:
	git clone https://github.com/openzelda/content-package openzelda/share/openzelda-2.0/packages/OpenZelda-2_0-Beta1.package/ 
	git clone https://github.com/lukesalisbury/luxengine ozengine/ 
	git clone https://github.com/lukesalisbury/elix elix/ 
	git clone https://github.com/lukesalisbury/meg questdesigner/


info:
	@echo Build Directory: $(BUILDDIR)
	@echo Install Directory: $(INSTALLDIR)
	@echo Projects: $(SUBDIRS)
	@echo Make: $(MAKE)
	@echo Engine Binary: $(BUILDDIR)/openzelda
	@echo Quest Designer Binary: $(BUILDDIR)/quest_designer
