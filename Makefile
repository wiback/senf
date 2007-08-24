#----------------------------------------------------------------------
# Some SCONS shortcuts
#----------------------------------------------------------------------
SCONS=scons

default: build

build:
	$(SCONS)

clean:
	$(SCONS) --clean

all_docs all_tests:
	$(SCONS) $@

#----------------------------------------------------------------------
# Subversion stuff
#----------------------------------------------------------------------
SVN_REVISION = $(shell svn info|grep '^Revision: '|awk '{print $$2}')

svn_version:
	@echo $(SVN_REVISION)

#----------------------------------------------------------------------
# Building SENF requires some debian packages
#----------------------------------------------------------------------
DEB_BASE   = scons build-essential binutils-dev
DEB_BOOST  = libboost-dev libboost-test-dev
DEB_BOOST += libboost-date-time-dev libboost-regex-dev libboost-thread-dev
DEB_DOC    = doxygen dia tidy xsltproc graphviz

prerequisites:
	aptitude install $(DEB_BASE) $(DEB_BOOST) $(DEB_DOC)

#----------------------------------------------------------------------
# Debian package info
#----------------------------------------------------------------------
PKG_NAME=senf-dev
PKG_ARCH=i386
PKG_VERS=0.0.$(SVN_REVISION)-1
PKG_FILE = $(PKG_NAME)-$(PKG_VERS)_$(PKG_ARCH).deb

#----------------------------------------------------------------------
# Debian package structure
#----------------------------------------------------------------------
DEB_TOP = ./debian
DEB_CTL = $(DEB_TOP)/DEBIAN
DEB_LIB = $(DEB_TOP)/usr/lib/senf
DEB_INC = $(DEB_TOP)/usr/include/senf
DEB_DOC = $(DEB_TOP)/usr/share/doc/senf

#----------------------------------------------------------------------
# Debian package content
#----------------------------------------------------------------------
SENF_LIBS = *.a
SENF_HDRS = $$(find -path './XXXdebian' -prune \
                 -o -iname \*.h \
                 -o -iname \*.hh \
                 -o -iname \*.ih \
                 -o -iname \*.c \
                 -o -iname \*.cc \
                 -o -iname \*.ct \
                 -o -iname \*.cci \
                 -o -iname \*.cti \
                 -o -iname \*.mpp \
)

package: $(PKG_FILE)
$(PKG_FILE): build
	rm -rf $(DEB_TOP)
	mkdir -p $(DEB_CTL) $(DEB_INC) $(DEB_LIB) $(DEB_DOC)
	find $(DEB_TOP) -type d | xargs chmod 755
	tar cf - $(SENF_HDRS) | (cd $(DEB_INC) && tar xf -)
	tar cf - $(SENF_LIBS) | (cd $(DEB_LIB) && tar xf -)
	sed -e 's,PKG_VERSION,$(PKG_VERS),' control > $(DEB_CTL)/control
	$(MAKE) deb-doc
	dpkg-deb --build debian $(PKG_FILE)

#----------------------------------------------------------------------
# Extract documentation files from source tree
#----------------------------------------------------------------------
deb-doc:
	rsync -rz \
		--filter="- debian/*" \
		--filter="- .svn" \
		--filter="+ */" \
		--filter="+ *.html" \
		--filter="+ *.css" \
		--filter="+ *.png" \
		--filter="+ *.php" \
		--filter="+ *.idx" \
		--filter="+ *.log" \
		--filter="- *" \
		. $(DEB_DOC)

