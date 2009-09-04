#----------------------------------------------------------------------
# Some SCONS shortcuts
#----------------------------------------------------------------------
CONCURRENCY_LEVEL ?= $(shell grep process /proc/cpuinfo | wc -l)

ifdef nice
  SCONS_ARGS += CXX='nice -n $(nice) g++'
endif
ifdef final
  SCONS_ARGS += "final="$(final)
endif
ifdef debug
  SCONS_ARGS += "debug="$(debug)
endif

SCONS=./tools/scons -j $(CONCURRENCY_LEVEL) $(SCONS_ARGS)

# DON'T call this default ... default is a valid scons target ;-)
build: default

TARGETS := prepare default examples all_tests all_docs all install_all \
	deb debsrc debbin linklint fixlinks valgrind lcov
ALLTARGETS := $(TARGETS) package build

$(TARGETS):
	$(SCONS) $@

clean:
	$(SCONS) --clean all

package: deb
test_coverage: lcov

#----------------------------------------------------------------------
# subdirectory build targets
#----------------------------------------------------------------------

%/test %/doc:
	$(SCONS) $@

%/build:
	$(SCONS) $*

#----------------------------------------------------------------------
# remote compile targets
#----------------------------------------------------------------------

CWD = $(shell pwd)
$(ALLTARGETS:%=%@%):
	ssh $* "cd $(CWD) && $(MAKE) SCONS_ARGS=\"$(SCONS_ARGS)\" $(firstword $(subst @, ,$@))"

#----------------------------------------------------------------------
# Subversion stuff
#----------------------------------------------------------------------

svn_version:
	@v=`svnversion`; if [ $$v=="exported" ]; then gitsvnversion else echo $v; fi

version: svn_version

#----------------------------------------------------------------------
# Building SENF requires some debian packages
#----------------------------------------------------------------------
DEB_BASE   = build-essential

# This line parses the 'Build-Depends' entry from debian/control
DEB_SENF   = $(shell perl -alnF'[:,]' -e '							\
			BEGIN{$$,=" "} END{splice @R,0,1; print @R}				\
			map {s/\(.*\)|\|.*//; s/[ \n\t]//g} @F;					\
			push @R,grep {/./} @F if (/^Build-Depends:/i.../^\S/)!~/(^|E0)$$/;'	\
				debian/control)

prerequisites:
	aptitude install $(DEB_BASE) $(DEB_SENF)
