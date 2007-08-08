## \file
# \brief CopyToDir builder

## \package senfscons.CopyToDir
# \brief Copy source file(s) to a given directory
#
# \ingroup builder

import os.path
import SCons.Builder, SCons.Defaults

def emitter(source, target, env):
    return ([ os.path.join(str(target[0]),source[0].name) ], source)

CopyToDir = SCons.Builder.Builder(emitter = emitter,
                                  action = SCons.Defaults.Copy("$TARGET","$SOURCE"),
                                  single_source = True)

def generate(env):
    env['BUILDERS']['CopyToDir'] = CopyToDir

def exists(env):
    return 1
