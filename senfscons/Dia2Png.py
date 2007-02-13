## \file
# \brief Bia2Png builder

## \package senfscons.Dia2Png
# \brief Build a PNG file from a DIA file
#
# This builder will convert a given DIA file into a PNG image. The
# size of the target file is specified by giving a preferred DPI value
# and a maximum width. The Builder will automatically fetch the
# correct aspect ratio from the dia file.
#
# \par Construction Envrionment Variables:
# <table class="senf">
# <tr><td>\c DIACOM</td><td>dia command, defaults to \c diak</td></tr>
# <tr><td>\c DIA2PNGDPI</td><td>resolution of converted image, defaults to 115</td></tr>
# <tr><td>\c DIA2PNGMAXWIDTH</td><td>maximum image width, defaults to 800</td></tr>
# </table>
#
# \ingroup builder

import os
import SCons.Builder

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
    return env.Action("$DIACOM -t png -s %dx%d -e $TARGET $SOURCE" % tuple(size))

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
