"""Dia2Png SCons Builder: Build a PNG file given a DIA file.

Support the specification of a scalefactor in the DIA2PNGDPI Environment variable.

"""

import os
import SCons.Builder

def dia_getSize(env,source):
    size = None
    for line in popen(env['DIACOM']+" -e /proc/self/fd/1 -t eps "+source[0],"r"):
        if line.startswith("%%BoundingBox:"):
            size=map(int,line.split()[4:])
    return size

def dia2png_generator(source, target, env, for_signature):
    if for_signature:
        return "$DIACOM -t png -s $DIA2PNGDPI $TARGET $SOURCE"
    size = dia_getSize(env,source)
    if not size: return None;
    size[0] = size[0]*72/int(env['DIA2PNGDPI'])
    size[1] = size[1]*72/int(env['$DIA2PNGDPI'])
    return env.Action("$DIACOM -t png -s %dx%d $TARGET $SOURCE" % size)

Dia2Png = SCons.Builder.Builder(suffix = ".png",
                                src_suffix = ".dia",
                                generator = dia2png_generator,
                                single_source = 1)

def generate(env):
    env['BUILDERS']['Dia2Png'] = Dia2Png
    env['DIACOM'] = "dia"
    env['DIA2PNGDPI'] = 115

def exists(env):
    return 1
