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

