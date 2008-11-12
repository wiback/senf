#----------------------------------------------------------------------
# Some SCONS shortcuts
#----------------------------------------------------------------------

CONCURRENCY_LEVEL ?= 2

ifdef final
  FINAL = "final=1"
endif

SCONS=scons -j $(CONCURRENCY_LEVEL) $(FINAL)

default: build

build:
	$(SCONS)

clean:
	$(SCONS) --clean all

all_docs all_tests all:
	$(SCONS) $@
	
%/test %/doc:
	$(SCONS) $@

#----------------------------------------------------------------------
# Subversion stuff
#----------------------------------------------------------------------

svn_version:
	@svnversion

#----------------------------------------------------------------------
# Building SENF requires some debian packages
#----------------------------------------------------------------------
DEB_BASE   = build-essential

# This line parses the 'Build-Depends' entry from debian/control
DEB_SENF   = $(shell perl -an -F'[:,]' -e '					\
	       	         BEGIN{ $$,=" " }					\
	       	         $$P=0 if /^\S/;					\
			 map {s/\(.*\)//} @F;					\
	       	         print @F if $$P;					\
	       	         if (/^Build-Depends:/) { print @F[1..$$\#F]; $$P=1 }'	\
	       	     debian/control | xargs echo)

prerequisites:
	aptitude install $(DEB_BASE) $(DEB_SENF)

package:
	$(SCONS) deb
