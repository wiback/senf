#
# Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
#                    Network Research Group (NET)
#                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
#                    Contact: http://wiback.org
#
# This file is part of the SENF code tree.
# It is licensed under the 3-clause BSD License (aka New BSD License).
# See LICENSE.txt in the top level directory for details or visit
# https://opensource.org/licenses/BSD-3-Clause
#


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
