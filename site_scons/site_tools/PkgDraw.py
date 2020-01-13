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


import os, os.path
import SCons.Builder, SCons.Action

libdir = os.path.join(os.path.dirname(os.path.dirname(__file__)),'lib')

PkgDraw = SCons.Builder.Builder(
    suffix = ".png",
    src_suffix = ".hh",
    action = SCons.Action.Action("$PKGDRAWCOM"),
    single_source = 1)

def generate(env):
    env['BUILDERS']['PkgDraw'] = PkgDraw
    env['PKGDRAW'] = "%s/pkgdraw" % libdir
    env['PKGDRAWCOM'] = "$PKGDRAW $SOURCE $TARGET $PKGDRAWPACKETS -- $CPPFLAGS $_CPPINCFLAGS $_CPPDEFFLAGS"
    env['PKGDRAWPACKETS'] = ''

def exists(env):
    return env.Detect("#/doclib/pkgdraw")
