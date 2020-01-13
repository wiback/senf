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


import os
import SCons.Builder, SCons.Action

def dia_getSize(env,source):
    size = None
    for line in os.popen(env['DIACOM']+" -e /proc/self/fd/1 -t eps "+str(source[0]),"r"):
        if line.startswith("%%BoundingBox:"):
            size=map(int,line.split()[3:])
            break
    return size

def dia2png_generator(source, target, env, for_signature):
    if for_signature:
        return "$DIACOM -t png -s $DIA2PNGDPI,$DIA2PNGMAXWIDTH $TARGET $SOURCE"
    size = dia_getSize(env,source)
    if not size: return None;
    size[0] = size[0]*int(env['DIA2PNGDPI'])/72
    size[1] = size[1]*int(env['DIA2PNGDPI'])/72
    if size[0] > env['DIA2PNGMAXWIDTH']:
        size[1] = size[1]*env['DIA2PNGMAXWIDTH']/size[0]
        size[0] = env['DIA2PNGMAXWIDTH']
    return SCons.Action.Action("$DIACOM -t png -s %dx%d -e $TARGET $SOURCE" % tuple(size))

Dia2Png = SCons.Builder.Builder(suffix = ".png",
                                src_suffix = ".dia",
                                generator = dia2png_generator,
                                single_source = 1)

def generate(env):
    env['BUILDERS']['Dia2Png'] = Dia2Png
    env['DIACOM'] = "dia"
    env['DIA2PNGDPI'] = 115
    env['DIA2PNGMAXWIDTH'] = 800

def exists(env):
    return env.Detect("dia")
