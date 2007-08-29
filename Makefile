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

package:
	$(SCONS) deb
