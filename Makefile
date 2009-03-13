#----------------------------------------------------------------------
# Some SCONS shortcuts
#----------------------------------------------------------------------

CONCURRENCY_LEVEL ?= 2

ifdef final
  SCONS_ARGS += "final="$(final)
endif
ifdef debug
  SCONS_ARGS += "debug="$(debug)
endif
ifdef profile
  SCONS_ARGS += "profile="$(profile)
endif

SCONS=scons -j $(CONCURRENCY_LEVEL) $(SCONS_ARGS)

default: build

build:
	$(SCONS)

clean:
	$(SCONS) --clean all
	find ./ -name \*.gcno | xargs rm -f
	find ./ -name \*.gcda | xargs rm -f
	find ./ -name \*.gcov | xargs rm -f
	rm -f test_coverage.info
	rm -rf /doc/test_coverage

all_docs all_tests all:
	$(SCONS) $@
	
%/test %/doc:
	$(SCONS) $@

%/build:
	$(SCONS) $*


#----------------------------------------------------------------------
# test coverage
#----------------------------------------------------------------------
test_coverage:
	$(SCONS) debug=1 EXTRA_CCFLAGS="-fprofile-arcs -ftest-coverage" EXTRA_LIBS="gcov" all_tests
	ln -s ../../boost/ include/senf/  # ugly work-around
	lcov --directory . --capture --output-file /tmp/test_coverage.info --base-directory .
#	lcov --output-file /tmp/test_coverage.info.tmp --extract test_coverage.info \*/senf/\*
	lcov --output-file test_coverage.info --remove /tmp/test_coverage.info \*/include/\*
	genhtml --output-directory doc/test_coverage --title "all_tests" test_coverage.info
	rm /tmp/test_coverage.info
	rm include/senf/boost

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
