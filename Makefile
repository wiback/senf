#----------------------------------------------------------------------
# Some SCONS shortcuts
#----------------------------------------------------------------------

SCONS=./tools/scons

# DON'T call this default ... default is a valid scons target ;-)
build: default
.PHONY: build

clean: message
	$(SCONS) --clean all
.PHONY: clean

Makefile:
	@true

# Forward everything else to scons
%: message
	$(SCONS) $@

message:
	@echo "================================================================="
	@echo "SENF uses SCons as it's build system, so instead of calling"
	@echo "    $$ make <target>"
	@echo
	@echo "use"
	@echo "    $$ ./tools/scons <target>"
	@echo
	@echo "To get help on valid targets an configuration variables, call"
	@echo "    $$ ./tools/scons -h"
	@echo
	@echo "To get help on scons command line options, call"
	@echo "    $$ ./tools/scons -H"
	@echo
	@echo "(interesting options: -c, -U, -j <num-cpus>, --config=force)"
	@echo "================================================================="
	sleep 5

prerequisites:
	./debian/install-depends.sh
.PHONY: prerequisites
