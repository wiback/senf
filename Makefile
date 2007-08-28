#----------------------------------------------------------------------
# Some SCONS shortcuts
#----------------------------------------------------------------------
SCONS=scons

default: build

build:
	$(SCONS)

clean:
	$(SCONS) --clean all

all_docs all_tests:
	$(SCONS) $@

#----------------------------------------------------------------------
# Subversion stuff
#----------------------------------------------------------------------
SVN_REVISION = $(shell svnversion)

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

package:
	$(SCONS) deb
